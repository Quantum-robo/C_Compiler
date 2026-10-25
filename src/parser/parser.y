%code requires {
    struct Operand;
    struct Declarator;
    struct DeclSpec;
    struct Param;
    struct ParamList;
    struct InitNode;
    struct Ctx;
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <set>
#include "../ir/TAC.hpp"
#include "../semantic/Types.hpp"

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

void yyerror(const char* s);
int has_error = 0;

/* ------------------------------------------------------------------
   Diagnostics
   ------------------------------------------------------------------ */

static int sem_errors = 0;
static int sem_warnings = 0;

static void semError(const std::string& msg, int line = 0) {
    has_error = 1;
    ++sem_errors;
    fprintf(stderr, "SEMANTIC ERROR: %s at line %d\n", msg.c_str(), line ? line : yylineno);
}

static void semWarn(const std::string& msg, int line = 0) {
    ++sem_warnings;
    fprintf(stderr, "WARNING: %s at line %d\n", msg.c_str(), line ? line : yylineno);
}

/* ------------------------------------------------------------------
   Semantic values
   ------------------------------------------------------------------ */

// An expression result. Reads and writes through pointers / array indexes are
// deferred so that an assignment can turn them into a store instead of a load.
struct Operand {
    enum Kind { VALUE, DEREF, INDEX } kind = VALUE;
    std::string place;                 // VALUE: the name/temp/literal
                                       // DEREF: the pointer being dereferenced
                                       // INDEX: the array / pointer base
    std::string index;                 // INDEX: flattened element index
    std::vector<std::string> dims;     // array dimensions (from the symbol table)
    size_t used = 0;                   // INDEX: subscripts applied so far
    // member access (for method calls)
    bool isMember = false;
    bool arrow = false;
    std::string member;
    Operand* objOp = nullptr;
    // semantic information
    Type ty;                           // static type (unknown if it could not be determined)
    bool lvalue = false;               // designates an object that can be assigned to / addressed
    bool isStr = false;                // string literal
    bool isCall = false;               // result of a function call
    bool notCallable = false;          // names a variable that cannot be called
    bool undeclared = false;           // identifier not found in any scope (reported when used)
    std::string name;                  // source identifier, for diagnostics
    int line = 0;
};

struct Param {
    std::string name;
    std::string type;
    std::vector<std::string> dims;
    bool isRef = false;
    int ptr = 0;
    bool isFuncPtr = false;
    Type ty;
};

struct ParamList {
    std::vector<Param> items;
};

struct Declarator {
    std::string name;
    std::vector<std::string> dims;
    bool isFunc = false;
    bool isFuncPtr = false;            // int (*fp)(int)
    bool isRef = false;                // declared with '&'
    bool paren = false;                // was written in parentheses
    int ptr = 0;                       // number of '*'
    std::vector<Param> params;
};

struct DeclSpec {
    std::string type;
    bool isStatic = false;
    bool isTypedef = false;
    bool isConst = false;
    bool isExtern = false;
};

struct InitNode {
    std::string value;
    Operand* op = nullptr;             // leaf not yet evaluated (a reference needs its address, not its value)
    bool isList = false;
    std::vector<InitNode*> items;
    Type ty;                           // leaf type, kept after evaluation for the initializer checks
    bool isStr = false;
    bool zero = false;
};

struct Ctx {
    std::string a, b, c;
};

/* ------------------------------------------------------------------
   Symbol table and parser state
   ------------------------------------------------------------------ */

struct Symbol {
    Type ty;
    std::vector<std::string> dims;
    std::string mangled;               // name used in the IR (static locals are qualified)
    bool isRef = false;                // reference: holds an address, every use is implicitly dereferenced
    int ptr = 0;
    bool isFuncPtr = false;
    bool isExtern = false;
    bool isEnumConst = false;
    bool hasValue = false;             // enum constant whose numeric value is known
    long value = 0;
};

struct FuncSig {
    std::vector<Param> params;
    Type ret;
    bool returnsRef = false;
    bool defined = false;
};

static std::vector<std::map<std::string, Symbol> > scopes(1);

static void pushScope() { scopes.push_back(std::map<std::string, Symbol>()); }
static void popScope() { if (scopes.size() > 1) scopes.pop_back(); }

static void declareSym(const std::string& name, const Symbol& s) {
    std::map<std::string, Symbol>& sc = scopes.back();
    std::map<std::string, Symbol>::iterator it = sc.find(name);
    if (it != sc.end() && !it->second.isExtern && !s.isExtern)
        semError("redeclaration of '" + name + "'");
    sc[name] = s;
}

static Symbol* lookupSym(const std::string& name) {
    for (size_t i = scopes.size(); i-- > 0;) {
        std::map<std::string, Symbol>::iterator it = scopes[i].find(name);
        if (it != scopes[i].end()) return &it->second;
    }
    return nullptr;
}

struct AggrFrame {
    std::string name;
    bool memberCtx;
    int access;
};

struct PendingName {                   // used inside a method, resolved when the class ends
    std::string cls, name;
    int line;
};

static std::string cur_func;           // IR label of the function being parsed
static std::string cur_aggr;           // enclosing class/struct/union name
static bool in_func = false;
static bool in_member_ctx = false;     // directly inside an aggregate body (not in a method)
static bool saved_member_ctx = false;
static int cur_access = 0;             // 0 public, 1 protected, 2 private
static std::vector<AggrFrame> aggr_stack;
static std::vector<PendingName> pendingNames;
static std::map<std::string, std::set<std::string> > classMethods;
static std::map<std::string, std::vector<std::string> > aggrMembers;
static std::map<std::string, std::map<std::string, Type> > aggrMemberTypes;
static std::map<std::string, std::map<std::string, int> > memberAccess;
static std::set<std::string> enumTags;
static std::vector<DeclSpec*> spec_stack;
static DeclSpec defaultSpec;

struct SwitchCtx {
    std::string value, test, end, def;
    std::vector<std::pair<std::string, std::string> > cases;   // (case value, label)
    std::set<std::string> seen;
    bool hasDefault = false;
};
static std::vector<SwitchCtx> switch_stack;

// Control-flow label stacks (loops use loop_stack, defined with the loop helpers)
std::stack<std::string> label_stack;                // pending labels of if/else
std::stack<std::string> break_stack;
std::stack<std::string> continue_stack;
struct CallFrame {
    std::string callee;                // IR name of the function being called ("" if unknown)
    std::vector<std::string> args;
};
std::stack<CallFrame> arg_stack;

// IR name -> signature of every declared/defined function
static std::map<std::string, FuncSig> funcSigs;
static std::set<std::string> calledBeforeDecl;    // callees seen before any declaration
static std::set<std::string> implicitWarned;
static bool cur_returns_ref = false;              // inside a function returning T&
static Type cur_ret;                              // return type of the function being parsed

// labels are function scoped
static std::set<std::string> fn_labels;
static std::vector<std::pair<std::string, int> > fn_gotos;

static bool inMethod() { return in_func && !cur_aggr.empty() && !in_member_ctx; }

static bool isMemberOf(const std::string& cls, const std::string& name) {
    std::map<std::string, std::vector<std::string> >::iterator it = aggrMembers.find(cls);
    if (it == aggrMembers.end()) return false;
    for (size_t i = 0; i < it->second.size(); ++i)
        if (it->second[i] == name) return true;
    return false;
}

// Library functions and macros that normally come from headers (#include lines are ignored).
static bool isBuiltinName(const std::string& n) {
    static const char* names[] = {
        "printf", "scanf", "fprintf", "fscanf", "sprintf", "snprintf", "sscanf", "puts", "putchar",
        "getchar", "gets", "fgets", "fputs", "fopen", "fclose", "fread", "fwrite", "fflush", "fgetc",
        "fputc", "feof", "remove", "rename", "malloc", "calloc", "realloc", "free", "exit", "abort",
        "atexit", "atoi", "atof", "atol", "abs", "labs", "rand", "srand", "time", "clock", "sqrt",
        "pow", "sin", "cos", "tan", "asin", "acos", "atan", "atan2", "exp", "log", "log10", "floor",
        "ceil", "fabs", "fmod", "round", "strlen", "strcpy", "strncpy", "strcat", "strncat", "strcmp",
        "strncmp", "strchr", "strrchr", "strstr", "strtok", "memset", "memcpy", "memmove", "memcmp",
        "toupper", "tolower", "isalpha", "isdigit", "isalnum", "isspace", "isupper", "islower",
        "ispunct", "perror", "system", "assert", "qsort", "getenv",
        "NULL", "EOF", "stdin", "stdout", "stderr", "INT_MAX", "INT_MIN", "UINT_MAX", "LONG_MAX",
        "LONG_MIN", "CHAR_BIT", "CHAR_MAX", "CHAR_MIN", "SHRT_MAX", "SHRT_MIN", "UCHAR_MAX",
        "RAND_MAX", "EXIT_SUCCESS", "EXIT_FAILURE", "M_PI", "M_E", "BUFSIZ", "SEEK_SET", "SEEK_CUR",
        "SEEK_END", "FLT_MAX", "FLT_MIN", "DBL_MAX", "DBL_MIN", "CLOCKS_PER_SEC"
    };
    static std::set<std::string> table(names, names + sizeof(names) / sizeof(names[0]));
    return table.count(n) > 0;
}

// Inside a method, bare member names refer to this->member.
static std::string resolveName(const std::string& n) {
    for (size_t i = scopes.size(); i-- > 0;) {
        std::map<std::string, Symbol>::iterator it = scopes[i].find(n);
        if (it != scopes[i].end()) {
            if (i == 0 && inMethod() && isMemberOf(cur_aggr, n)) break;   // member shadows global
            return it->second.mangled;
        }
    }
    if (inMethod() && isMemberOf(cur_aggr, n)) return "this->" + n;
    return n;
}

static void beginAggr(const std::string& name, bool isClass) {
    AggrFrame f;
    f.name = cur_aggr;
    f.memberCtx = in_member_ctx;
    f.access = cur_access;
    aggr_stack.push_back(f);
    if (aggrMembers.count(name)) semError("redefinition of '" + name + "'");
    cur_aggr = name;
    in_member_ctx = true;
    cur_access = isClass ? 2 : 0;      // class members default to private
    aggrMembers[name];
}

// Names used inside method bodies that were not yet known when the body was
// parsed (members declared later in the class) must exist by the end of the class.
static void endAggr() {
    std::set<std::string> reported;
    for (size_t i = 0; i < pendingNames.size();) {
        if (pendingNames[i].cls == cur_aggr) {
            const std::string& n = pendingNames[i].name;
            if (!isMemberOf(cur_aggr, n) && !classMethods[cur_aggr].count(n) && reported.insert(n).second)
                semError("'" + n + "' was not declared in this scope", pendingNames[i].line);
            pendingNames.erase(pendingNames.begin() + i);
        } else {
            ++i;
        }
    }
    if (aggr_stack.empty()) return;
    cur_aggr = aggr_stack.back().name;
    in_member_ctx = aggr_stack.back().memberCtx;
    cur_access = aggr_stack.back().access;
    aggr_stack.pop_back();
}

static void popSpec() { if (!spec_stack.empty()) spec_stack.pop_back(); }
static DeclSpec* currentSpec() { return spec_stack.empty() ? &defaultSpec : spec_stack.back(); }

// Build a Type from the declaration specifier text and declarator shape.
static Type typeFromName(const std::string& raw, int ptr, int arr, bool cb) {
    if (enumTags.count(raw)) return makeType("int", ptr, arr, false, cb);
    std::string b = normalizeBase(raw);
    return makeType(b, ptr, arr, !isBuiltinBase(b), cb);
}

/* ------------------------------------------------------------------
   Expression helpers
   ------------------------------------------------------------------ */

static bool isNumber(const std::string& s) {
    if (s.empty() || s.size() > 15) return false;
    size_t i = (s[0] == '-') ? 1 : 0;
    if (i >= s.size()) return false;
    for (; i < s.size(); ++i)
        if (!isdigit((unsigned char)s[i])) return false;
    return true;
}

static Operand* valueOp(const std::string& place) {
    Operand* o = new Operand();
    o->place = place;
    return o;
}

static Operand* typedOp(const std::string& place, const Type& ty) {
    Operand* o = valueOp(place);
    o->ty = ty;
    return o;
}

static bool isZeroLit(const Operand* o) { return o->kind == Operand::VALUE && o->place == "0"; }

// An undeclared name is reported when it is actually used (a callee is exempt).
// Inside a method it may be a member declared later in the class, so it is
// assumed to be one and verified when the class ends.
static void use(Operand* o) {
    if (!o || !o->undeclared) return;
    o->undeclared = false;
    if (inMethod()) {
        o->place = "this->" + o->name;
        PendingName p;
        p.cls = cur_aggr;
        p.name = o->name;
        p.line = o->line;
        pendingNames.push_back(p);
        return;
    }
    semError("'" + o->name + "' was not declared in this scope", o->line);
}

static std::string mulStr(const std::string& a, const std::string& b) {
    if (b.empty()) return a;
    if (isNumber(a) && isNumber(b)) return std::to_string(std::stoll(a) * std::stoll(b));
    if (a == "1") return b;
    if (b == "1") return a;
    if (a == "0" || b == "0") return "0";
    std::string t = tacGen.newTemp();
    tacGen.emit("*", a, b, t);
    return t;
}

static std::string addStr(const std::string& a, const std::string& b) {
    if (isNumber(a) && isNumber(b)) return std::to_string(std::stoll(a) + std::stoll(b));
    if (a == "0") return b;
    if (b == "0") return a;
    std::string t = tacGen.newTemp();
    tacGen.emit("+", a, b, t);
    return t;
}

static size_t totalDims(const Operand* o) { return o->dims.empty() ? 1 : o->dims.size(); }

// Number of elements spanned by one step of the subscript at position 'from'.
static std::string restStride(const std::vector<std::string>& dims, size_t from) {
    std::string s = "1";
    for (size_t k = from; k < dims.size(); ++k) s = mulStr(s, dims[k]);
    return s;
}

// Produce the value of an operand, emitting the load if it is still deferred.
static std::string rval(Operand* o) {
    use(o);
    if (o->kind == Operand::VALUE) return o->place;
    std::string t = tacGen.newTemp();
    if (o->kind == Operand::DEREF) {
        tacGen.emit("deref", o->place, "", t);
    } else if (o->used < totalDims(o)) {
        // a sub-array (row) decays to its address
        tacGen.emit("addridx", o->place, mulStr(o->index, restStride(o->dims, o->used)), t);
    } else {
        tacGen.emit("idxload", o->place, o->index, t);
    }
    return t;
}

static std::string addrOf(Operand* o) {
    use(o);
    if (o->kind == Operand::DEREF) return o->place;
    std::string t = tacGen.newTemp();
    if (o->kind == Operand::VALUE) {
        tacGen.emit("addr", o->place, "", t);
    } else {
        std::string idx = (o->used < totalDims(o)) ? mulStr(o->index, restStride(o->dims, o->used)) : o->index;
        tacGen.emit("addridx", o->place, idx, t);
    }
    return t;
}

static void store(Operand* lhs, const std::string& v) {
    use(lhs);
    if (lhs->kind == Operand::VALUE) tacGen.emit("=", v, "", lhs->place);
    else if (lhs->kind == Operand::DEREF) tacGen.emit("store", v, "", lhs->place);
    else tacGen.emit("idxstore", lhs->place, lhs->index, v);
}

/* ---- type checks ---- */

static void checkAssign(const Type& dst, Operand* src, const std::string& ctx) {
    std::string why;
    Compat c = assignCompat(dst, src->ty, isZeroLit(src), why);
    if (c == COMPAT_ERR) semError("incompatible types in " + ctx + ": " + why);
    else if (c == COMPAT_WARN) semWarn(ctx + ": " + why);
}

static void checkCondition(Operand* o) {
    if (!o) return;
    if (isAggrVal(o->ty)) semError("used '" + typeName(o->ty) + "' value where a scalar is required");
    else if (isVoidT(o->ty)) semError("void value used where a scalar is required");
}

static void checkModifiable(Operand* o, const std::string& what) {
    if (!o->lvalue) {
        semError("lvalue required as " + what);
        return;
    }
    if (known(o->ty)) {
        if (o->ty.arr > 0) semError("cannot modify array '" + (o->name.empty() ? o->place : o->name) + "' (" + what + ")");
        else if (isConstObject(o->ty)) semError("cannot modify read-only object (type '" + typeName(o->ty) + "') in " + what);
    }
}

static Operand* binop(const char* op, Operand* a, Operand* b) {
    std::string l = rval(a);
    std::string r = rval(b);
    Type res;
    if (!binaryOpType(op, a->ty, b->ty, res))
        semError(std::string("invalid operands to binary '") + op + "' (have '" + typeName(a->ty) +
                 "' and '" + typeName(b->ty) + "')");
    if ((!strcmp(op, "/") || !strcmp(op, "%")) && isZeroLit(b)) semWarn("division by zero");
    std::string t = tacGen.newTemp();
    tacGen.emit(op, l, r, t);
    return typedOp(t, res);
}

static Operand* bitNot(Operand* a) {
    if (known(a->ty) && !isIntegral(a->ty))
        semError("wrong type argument to bit-complement (type '" + typeName(a->ty) + "')");
    std::string v = rval(a);
    std::string t = tacGen.newTemp();
    tacGen.emit("~", v, "", t);
    return typedOp(t, isIntegral(a->ty) ? makeType("int") : Type());
}

static Operand* logNot(Operand* a) {
    if (known(a->ty) && !isScalar(a->ty))
        semError("wrong type argument to '!' (type '" + typeName(a->ty) + "')");
    std::string v = rval(a);
    std::string t = tacGen.newTemp();
    tacGen.emit("!", v, "", t);
    return typedOp(t, makeType("int"));
}

static Operand* negate(Operand* a) {
    if (known(a->ty) && !isArith(a->ty))
        semError("wrong type argument to unary minus (type '" + typeName(a->ty) + "')");
    Type rt = isIntegral(a->ty) && a->ty.base != "long" ? makeType("int") : (isArith(a->ty) ? a->ty : Type());
    if (a->kind == Operand::VALUE && isNumber(a->place)) {
        const std::string& p = a->place;
        return typedOp(p[0] == '-' ? p.substr(1) : "-" + p, rt);
    }
    std::string v = rval(a);
    std::string t = tacGen.newTemp();
    tacGen.emit("uminus", v, "", t);
    return typedOp(t, rt);
}

static Operand* unaryPlus(Operand* a) {
    if (known(a->ty) && !isArith(a->ty))
        semError("wrong type argument to unary plus (type '" + typeName(a->ty) + "')");
    return a;
}

static Operand* derefOp(Operand* e) {
    use(e);
    Operand* o = new Operand();
    o->kind = Operand::DEREF;
    o->lvalue = true;
    if (known(e->ty)) {
        if (levels(e->ty) == 0) {
            semError("invalid type argument of unary '*' (have '" + typeName(e->ty) + "')");
        } else {
            o->ty = e->ty;
            if (o->ty.arr > 0) o->ty.arr--; else o->ty.ptr--;
        }
    }
    // the load (or the store, if this is an assignment target) happens later
    o->place = rval(e);
    return o;
}

static Operand* addrOp(Operand* e) {
    use(e);
    if (!e->lvalue) semError("lvalue required as unary '&' operand");
    Type rt;
    if (known(e->ty) && e->ty.arr == 0) {
        rt = e->ty;
        rt.ptr++;
    }
    return typedOp(addrOf(e), rt);
}

static Operand* identOp(const std::string& n) {
    std::string place = resolveName(n);
    Operand* o = valueOp(place);
    o->name = n;
    o->line = yylineno;
    Symbol* s = lookupSym(n);
    if (s && place == s->mangled) {
        o->dims = s->dims;
        o->ty = s->ty;
        o->lvalue = !s->isEnumConst;
        if (s->isEnumConst && s->hasValue) o->place = std::to_string(s->value);   // enumerators are compile-time numbers
        o->notCallable = !s->isFuncPtr && s->ptr == 0 && s->dims.empty();
        // A reference to an array behaves like the array's base pointer, so it
        // is only dereferenced implicitly when it refers to a single object.
        if (s->isRef && s->dims.empty()) o->kind = Operand::DEREF;
    } else if (place.compare(0, 6, "this->") == 0) {
        o->lvalue = true;
        std::map<std::string, std::map<std::string, Type> >::iterator mt = aggrMemberTypes.find(cur_aggr);
        if (mt != aggrMemberTypes.end()) {
            std::map<std::string, Type>::iterator ft = mt->second.find(n);
            if (ft != mt->second.end()) o->ty = ft->second;
        }
    } else if (funcSigs.count(n) || isBuiltinName(n) || (inMethod() && classMethods[cur_aggr].count(n))) {
        o->lvalue = true;                // a function or library name: type unknown
    } else {
        o->undeclared = true;
        o->lvalue = true;
    }
    return o;
}

// base[idx]; consecutive subscripts of a multi-dimensional array are folded
// into one row-major element index.
static Operand* subscript(Operand* base, Operand* idxOp) {
    std::string idx = rval(idxOp);
    use(base);
    Operand* r = new Operand();
    r->kind = Operand::INDEX;
    r->lvalue = true;
    r->line = yylineno;
    if (known(idxOp->ty) && !isIntegral(idxOp->ty)) semError("array subscript is not an integer");
    if (known(base->ty)) {
        if (levels(base->ty) == 0) {
            semError("subscripted value is neither array nor pointer (type '" + typeName(base->ty) + "')");
        } else {
            r->ty = base->ty;
            if (r->ty.arr > 0) r->ty.arr--; else r->ty.ptr--;
        }
    }
    if (base->kind == Operand::INDEX && base->used < totalDims(base)) {
        size_t k = base->used;
        r->place = base->place;
        r->dims = base->dims;
        r->used = k + 1;
        r->index = addStr(mulStr(base->index, base->dims[k]), idx);
        return r;
    }
    if (base->kind == Operand::VALUE) r->dims = base->dims;
    r->place = rval(base);
    r->used = 1;
    r->index = idx;
    return r;
}

static int accessOf(const std::string& cls, const std::string& m) {
    std::map<std::string, std::map<std::string, int> >::iterator it = memberAccess.find(cls);
    if (it == memberAccess.end()) return 0;
    std::map<std::string, int>::iterator jt = it->second.find(m);
    return jt == it->second.end() ? 0 : jt->second;
}

static Operand* member(Operand* base, const std::string& m, bool arrow) {
    use(base);
    Operand* r = new Operand();
    r->isMember = true;
    r->arrow = arrow;
    r->member = m;
    r->line = yylineno;

    std::string cls;
    const Type& bt = base->ty;
    if (known(bt)) {
        if (arrow) {
            if (bt.aggr && levels(bt) == 1) cls = bt.base;
            else semError("base operand of '->' is not a pointer to a structure or class (type '" + typeName(bt) + "')");
        } else {
            if (isAggrVal(bt)) cls = bt.base;
            else if (bt.aggr && levels(bt) == 1) semError("'" + typeName(bt) + "' is a pointer; did you mean to use '->'?");
            else semError("request for member '" + m + "' in something not a structure or union (type '" + typeName(bt) + "')");
        }
    }

    Type mty;
    if (!cls.empty() && aggrMembers.count(cls)) {
        bool field = isMemberOf(cls, m);
        bool method = classMethods[cls].count(m) > 0;
        bool inside = inMethod() && cur_aggr == cls;
        if (!field && !method) {
            if (!inside) semError("'" + cls + "' has no member named '" + m + "'");
        } else {
            int lvl = accessOf(cls, m);
            if (lvl != 0 && !inside)
                semError("'" + m + "' is " + (lvl == 1 ? "protected" : "private") + " within this context");
            if (field) {
                std::map<std::string, Type>& mt = aggrMemberTypes[cls];
                std::map<std::string, Type>::iterator ft = mt.find(m);
                if (ft != mt.end()) mty = ft->second;
            }
        }
    }
    if (known(mty) && bt.cb) mty.cb = true;      // a member of a const object is const
    r->ty = mty;
    r->lvalue = arrow ? true : base->lvalue;

    if (arrow) {
        std::string p = rval(base);
        r->place = p + "->" + m;
        r->objOp = valueOp(p);
        r->objOp->ty = base->ty;
    } else if (base->kind == Operand::DEREF) {
        r->place = base->place + "->" + m;
        r->objOp = base;
    } else if (base->kind == Operand::INDEX) {
        r->place = base->place + "[" + base->index + "]." + m;
        r->objOp = base;
    } else {
        r->place = base->place + "." + m;
        r->objOp = base;
    }
    return r;
}

static std::string classOf(Operand* obj) {
    if (!obj) return "";
    if (obj->place == "this") return cur_aggr;
    return (known(obj->ty) && obj->ty.aggr) ? obj->ty.base : std::string();
}

// Does this callee resolve to a class method (explicit object or implicit this)?
// Pure: emits no code, so it can also be used before the arguments are parsed.
static bool resolveMethod(Operand* f, std::string& target) {
    if (f->kind != Operand::VALUE) return false;
    if (f->isMember) {
        std::string cls = classOf(f->objOp);
        if (!cls.empty() && classMethods[cls].count(f->member)) {
            target = cls + "::" + f->member;
            return true;
        }
        return false;
    }
    if (inMethod() && classMethods[cur_aggr].count(f->place) && !lookupSym(f->place)) {
        target = cur_aggr + "::" + f->place;
        return true;
    }
    // unknown name inside a method: assume a method declared later in the class
    if (inMethod() && f->undeclared) {
        target = cur_aggr + "::" + f->name;
        return true;
    }
    return false;
}

static std::string calleeName(Operand* f) {
    std::string t;
    if (resolveMethod(f, t)) return t;
    return f->kind == Operand::VALUE ? f->place : std::string();
}

static bool isLiteralPlace(const std::string& p) {
    if (p.empty()) return false;
    char c = p[0];
    return !(isalpha((unsigned char)c) || c == '_');
}

// Address to bind a reference to. A literal has no address, so it is copied
// into a temporary first (const int &r = 5).
static std::string refTarget(Operand* o) {
    if (o->kind == Operand::VALUE && isLiteralPlace(o->place)) {
        std::string t = tacGen.newTemp();
        tacGen.emit("=", o->place, "", t);
        return addrOf(valueOp(t));
    }
    return addrOf(o);
}

// Reference parameters / results are only known once the function is declared,
// so remember calls made earlier and warn if they turn out to be affected.
static void noteCall(const std::string& callee) {
    if (!callee.empty() && !funcSigs.count(callee)) calledBeforeDecl.insert(callee);
}

static void pushArg(Operand* o) {
    if (arg_stack.empty()) return;
    CallFrame& fr = arg_stack.top();
    size_t i = fr.args.size();
    std::map<std::string, FuncSig>::iterator it = funcSigs.find(fr.callee);
    const Param* p = (it != funcSigs.end() && i < it->second.params.size()) ? &it->second.params[i] : nullptr;
    bool byRef = p && p->isRef;
    if (p) {
        use(o);
        std::string what = "argument " + std::to_string(i + 1) + " of '" + fr.callee + "'";
        if (byRef && !p->ty.cb) {
            if (!o->lvalue)
                semError("cannot bind non-const reference to an rvalue in " + what);
            else if (isConstObject(o->ty))
                semError("binding a non-const reference to a const object discards qualifiers in " + what);
        }
        checkAssign(p->ty, o, what);
    }
    fr.args.push_back(byRef ? refTarget(o) : rval(o));
}

// Arguments are pushed right to left; for methods the object pointer is pushed last.
static Operand* makeCall(Operand* f, const std::vector<std::string>& args) {
    std::string target;
    std::string thisArg;
    bool method = resolveMethod(f, target);

    if (f->notCallable) semError("'" + f->name + "' is not a function and cannot be called");

    if (method) {
        if (f->isMember) {
            thisArg = f->arrow ? rval(f->objOp) : addrOf(f->objOp);
        } else {
            thisArg = "this";
            if (f->undeclared) {
                f->undeclared = false;
                PendingName p;
                p.cls = cur_aggr;
                p.name = f->name;
                p.line = f->line;
                pendingNames.push_back(p);
            }
        }
    } else {
        if (f->undeclared) {
            f->undeclared = false;
            if (implicitWarned.insert(f->name).second)
                semWarn("implicit declaration of function '" + f->name + "'", f->line);
        }
        target = rval(f);
    }

    std::map<std::string, FuncSig>::iterator si = funcSigs.find(target);
    const FuncSig* sig = si == funcSigs.end() ? nullptr : &si->second;
    if (sig && args.size() != sig->params.size()) {
        semError(std::string(args.size() > sig->params.size() ? "too many" : "too few") +
                 " arguments to function '" + target + "' (expected " + std::to_string(sig->params.size()) +
                 ", got " + std::to_string(args.size()) + ")");
    }

    for (size_t i = args.size(); i-- > 0;) tacGen.emit("param", args[i], "", "");
    if (method) tacGen.emit("param", thisArg, "", "");
    std::string t = tacGen.newTemp();
    tacGen.emit("call", target, std::to_string(args.size() + (method ? 1 : 0)), t);
    Operand* result = valueOp(t);
    result->isCall = true;
    if (sig) result->ty = sig->ret;
    // a function returning T& yields an address; the result is implicitly dereferenced
    if (sig && sig->returnsRef) {
        result->kind = Operand::DEREF;
        result->lvalue = true;
    }
    return result;
}

// return statement checks
static void checkReturn(Operand* o) {
    if (!in_func) return;
    bool hasValue = !(o->kind == Operand::VALUE && o->place.empty());
    if (isVoidT(cur_ret)) {
        if (hasValue) {
            bool voidCall = o->isCall && (!known(o->ty) || isVoidT(o->ty));
            if (!voidCall) semError("return with a value, in function returning void");
        }
    } else if (known(cur_ret)) {
        if (!hasValue) {
            semWarn("return with no value, in function returning non-void");
        } else {
            checkAssign(cur_ret, o, "return");
            if (cur_returns_ref && !o->lvalue && !cur_ret.cb)
                semError("cannot return an rvalue from a function returning a non-const reference");
        }
    }
}

// Operand of a return statement: functions returning T& return an address.
static std::string returnValue(Operand* o) {
    if (cur_returns_ref && !(o->kind == Operand::VALUE && o->place.empty())) return refTarget(o);
    return rval(o);
}

static void checkIncDec(Operand* o) {
    use(o);
    checkModifiable(o, "increment/decrement operand");
    if (known(o->ty) && !isScalar(o->ty))
        semError("wrong type argument to increment/decrement (type '" + typeName(o->ty) + "')");
}

static Operand* preIncDec(Operand* o, const char* op) {
    checkIncDec(o);
    std::string cur = rval(o);
    std::string t = tacGen.newTemp();
    tacGen.emit(op, cur, "1", t);
    store(o, t);
    return o->kind == Operand::VALUE ? o : typedOp(t, o->ty);
}

static Operand* postIncDec(Operand* o, const char* op) {
    checkIncDec(o);
    std::string old;
    if (o->kind == Operand::VALUE) {
        old = tacGen.newTemp();
        tacGen.emit("=", o->place, "", old);
    } else {
        old = rval(o);
    }
    std::string nv = tacGen.newTemp();
    tacGen.emit(op, o->kind == Operand::VALUE ? o->place : old, "1", nv);
    store(o, nv);
    return typedOp(old, o->ty);
}

static Operand* assign(Operand* lhs, const std::string& op, Operand* rhs) {
    use(lhs);
    checkModifiable(lhs, "left operand of assignment");
    std::string bop = (op == "=") ? "" : op.substr(0, op.size() - 1);   // "+=" -> "+"
    if (bop.empty()) {
        checkAssign(lhs->ty, rhs, "assignment");
    } else {
        Type res;
        if (!binaryOpType(bop, lhs->ty, rhs->ty, res)) {
            semError("invalid operands to '" + op + "' (have '" + typeName(lhs->ty) + "' and '" +
                     typeName(rhs->ty) + "')");
        } else {
            std::string why;
            if (known(lhs->ty) && known(res) && assignCompat(lhs->ty, res, false, why) == COMPAT_ERR)
                semError("incompatible types in '" + op + "': " + why);
        }
        if ((bop == "/" || bop == "%") && isZeroLit(rhs)) semWarn("division by zero");
    }
    std::string v = rval(rhs);
    if (!bop.empty()) {
        std::string cur = rval(lhs);
        std::string t = tacGen.newTemp();
        tacGen.emit(bop, cur, v, t);
        v = t;
    }
    store(lhs, v);
    return lhs->kind == Operand::VALUE ? lhs : typedOp(v, lhs->ty);
}

static Type commonType(Operand* a, Operand* b) {
    const Type& x = a->ty;
    const Type& y = b->ty;
    if (!known(x) || !known(y)) return Type();
    if (isArith(x) && isArith(y)) return arithResult(x, y);
    if (levels(x) == levels(y) && x.base == y.base && x.aggr == y.aggr) return pointerResult(x);
    if (isPointerT(x) && isZeroLit(b)) return pointerResult(x);
    if (isPointerT(y) && isZeroLit(a)) return pointerResult(y);
    return Type();
}

/* ------------------------------------------------------------------
   Declaration helpers
   ------------------------------------------------------------------ */

static long long prodDims(const std::vector<std::string>& dims, size_t from) {
    long long p = 1;
    for (size_t k = from; k < dims.size(); ++k) {
        if (!isNumber(dims[k])) return 1;
        p *= std::stoll(dims[k]);
    }
    return p;
}

// Flatten a (possibly nested) brace initializer into per-element stores.
static void emitInitList(const std::string& base, const std::vector<std::string>& dims,
                         InitNode* n, size_t level, long long off) {
    long long stride = prodDims(dims, level + 1);
    for (size_t i = 0; i < n->items.size(); ++i) {
        InitNode* c = n->items[i];
        if (c->isList) {
            emitInitList(base, dims, c, level + 1, off + (long long)i * stride);
        } else {
            bool strLeaf = !c->value.empty() && c->value[0] == '"' && level + 1 < dims.size();
            long long idx = strLeaf ? off + (long long)i * stride : off + (long long)i;
            tacGen.emit("idxstore", base, std::to_string(idx), c->value);
        }
    }
}

// Evaluate a leaf initializer's expression (emits any pending load).
static void finalizeLeaf(InitNode* n) {
    if (!n->isList && n->op) {
        n->ty = n->op->ty;
        n->isStr = n->op->isStr;
        n->zero = isZeroLit(n->op);
        n->value = rval(n->op);
        n->op = nullptr;
    }
}

static void emitInitial(const std::string& target, const Symbol& sym, InitNode* init) {
    if (!init->isList) {
        finalizeLeaf(init);
        tacGen.emit("=", init->value, "", target);
        return;
    }
    if (sym.dims.empty()) {
        std::map<std::string, std::vector<std::string> >::iterator it = aggrMembers.find(sym.ty.base);
        if (it != aggrMembers.end()) {
            for (size_t i = 0; i < init->items.size() && i < it->second.size(); ++i)
                if (!init->items[i]->isList)
                    tacGen.emit("=", init->items[i]->value, "", target + "." + it->second[i]);
            return;
        }
    }
    emitInitList(target, sym.dims, init, 0, 0);
}

// Type-check every scalar leaf of a brace initializer against the element type.
static void checkInitLeaves(InitNode* n, const Type& elem) {
    for (size_t i = 0; i < n->items.size(); ++i) {
        InitNode* c = n->items[i];
        if (c->isList) { checkInitLeaves(c, elem); continue; }
        if (c->isStr && elem.base == "char" && !elem.aggr && elem.ptr == 0) continue;   // "ab" initialises a char row
        std::string why;
        Compat r = assignCompat(elem, c->ty, c->zero, why);
        if (r == COMPAT_ERR) semError("incompatible types in initialization: " + why);
        else if (r == COMPAT_WARN) semWarn("initialization: " + why);
    }
}

static void normalizeParams(std::vector<Param>& ps) {
    // f(void) means no parameters
    if (ps.size() == 1 && ps[0].name.empty() && ps[0].type == "void" && ps[0].ptr == 0 &&
        ps[0].dims.empty() && !ps[0].isRef)
        ps.clear();
}

static void recordSignature(const std::string& name, Declarator* d, DeclSpec* sp, bool defined) {
    FuncSig sig;
    sig.params = d->params;
    sig.returnsRef = d->isRef;
    sig.ret = typeFromName(sp->type, d->ptr, 0, sp->isConst);
    sig.defined = defined;

    std::map<std::string, FuncSig>::iterator it = funcSigs.find(name);
    if (it != funcSigs.end()) {
        const FuncSig& old = it->second;
        if (old.params.size() != sig.params.size()) {
            semError("conflicting declaration of '" + name + "' (different number of parameters)");
        } else if (known(old.ret) && known(sig.ret) && typeName(old.ret) != typeName(sig.ret)) {
            semError("conflicting return type for '" + name + "' ('" + typeName(sig.ret) + "' vs '" +
                     typeName(old.ret) + "')");
        } else if (defined && old.defined) {
            semError("redefinition of '" + name + "'");
        }
        sig.defined = defined || old.defined;
    }

    bool any = sig.returnsRef;
    for (size_t i = 0; i < sig.params.size(); ++i) any = any || sig.params[i].isRef;
    if (any && calledBeforeDecl.count(name))
        semWarn("'" + name + "' uses references but was called before it was declared; "
                "those earlier calls pass values instead of addresses");
    funcSigs[name] = sig;
}

static void declareVar(Declarator* d, DeclSpec* sp, InitNode* init) {
    if (sp->isTypedef) return;
    bool member = !cur_aggr.empty() && in_member_ctx;

    if (d->isFunc) {
        // prototype: remember its signature for argument and reference checks
        recordSignature(member ? cur_aggr + "::" + d->name : d->name, d, sp, false);
        if (member) {
            classMethods[cur_aggr].insert(d->name);
            memberAccess[cur_aggr][d->name] = cur_access;
        }
        return;
    }

    Type ty = d->isFuncPtr ? Type() : typeFromName(sp->type, d->ptr, (int)d->dims.size(), sp->isConst);

    // declaration checks
    if (known(ty)) {
        if (isVoidT(ty) && !d->isRef) semError("variable '" + d->name + "' declared void");
        if (ty.aggr && ty.ptr == 0 && !d->isRef && !sp->isExtern && !aggrMembers.count(ty.base))
            semError("variable '" + d->name + "' has incomplete type '" + ty.base + "'");
    }
    for (size_t i = 0; i < d->dims.size(); ++i)
        if (isNumber(d->dims[i]) && std::stoll(d->dims[i]) < 0)
            semError("size of array '" + d->name + "' is negative");

    if (member) {
        if (isMemberOf(cur_aggr, d->name)) semError("duplicate member '" + d->name + "'");
        aggrMembers[cur_aggr].push_back(d->name);
        aggrMemberTypes[cur_aggr][d->name] = ty;
        memberAccess[cur_aggr][d->name] = cur_access;
        return;
    }

    Symbol s;
    s.ty = ty;
    s.dims = d->dims;
    s.mangled = d->name;
    s.isRef = d->isRef;
    s.ptr = d->ptr;
    s.isFuncPtr = d->isFuncPtr;
    s.isExtern = sp->isExtern;
    bool staticLocal = sp->isStatic && in_func;
    if (staticLocal) s.mangled = cur_func + "." + d->name;
    declareSym(d->name, s);

    if (init) {
        // initializer type checks (the leaf is still unevaluated here)
        if (init->isList) {
            if (ty.arr > 0 && known(ty)) {
                Type elem = ty;
                elem.arr = 0;
                if (d->dims.size() == 1 && isNumber(d->dims[0]) &&
                    (long long)init->items.size() > std::stoll(d->dims[0]))
                    semError("excess elements in array initializer for '" + d->name + "'");
                checkInitLeaves(init, elem);
            }
        } else if (init->op) {
            Operand* o = init->op;
            use(o);
            if (d->isRef) {
                if (!ty.cb && !o->lvalue)
                    semError("cannot bind non-const reference '" + d->name + "' to an rvalue");
                else if (!ty.cb && isConstObject(o->ty))
                    semError("binding non-const reference '" + d->name + "' to a const object discards qualifiers");
                checkAssign(ty, o, "initialization of '" + d->name + "'");
            } else if (ty.arr > 0) {
                bool charStr = o->isStr && ty.base == "char" && !ty.aggr && ty.arr == 1 && ty.ptr == 0;
                if (known(ty) && !charStr)
                    semError("invalid initializer for array '" + d->name + "'");
            } else {
                checkAssign(ty, o, "initialization of '" + d->name + "'");
            }
        }

        if (staticLocal) tacGen.setForceGlobal(true);
        if (s.isRef && !init->isList && init->op) {
            // bind the reference: it stores the address of what it refers to
            std::string addr = refTarget(init->op);
            tacGen.emit("=", addr, "", s.mangled);
        } else {
            emitInitial(s.mangled, s, init);
        }
        if (staticLocal) tacGen.setForceGlobal(false);
    }
}

static void beginFunction(Declarator* d) {
    DeclSpec* sp = currentSpec();
    bool isMethod = !cur_aggr.empty() && in_member_ctx;
    cur_func = isMethod ? cur_aggr + "::" + d->name : d->name;
    if (isMethod) {
        classMethods[cur_aggr].insert(d->name);
        memberAccess[cur_aggr][d->name] = cur_access;
    }
    saved_member_ctx = in_member_ctx;
    in_member_ctx = false;
    in_func = true;
    fn_labels.clear();
    fn_gotos.clear();
    pushScope();
    for (size_t i = 0; i < d->params.size(); ++i) {
        const Param& p = d->params[i];
        if (p.name.empty()) continue;
        Symbol s;
        s.ty = p.ty;
        s.dims = p.dims;
        s.mangled = p.name;
        s.isRef = p.isRef;
        s.ptr = p.ptr;
        s.isFuncPtr = p.isFuncPtr;
        declareSym(p.name, s);
    }
    recordSignature(cur_func, d, sp, true);
    cur_returns_ref = d->isRef;
    cur_ret = typeFromName(sp->type, d->ptr, 0, sp->isConst);
    tacGen.emit("func_begin", "", "", cur_func);
}

static void endFunction() {
    for (size_t i = 0; i < fn_gotos.size(); ++i)
        if (!fn_labels.count(fn_gotos[i].first))
            semError("label '" + fn_gotos[i].first + "' used but not defined in function '" + cur_func + "'",
                     fn_gotos[i].second);
    tacGen.emit("func_end", "", "", cur_func);
    popScope();
    in_func = false;
    cur_returns_ref = false;
    cur_ret = Type();
    in_member_ctx = saved_member_ctx;
    cur_func.clear();
    popSpec();
}

static void applyStorage(DeclSpec* s, const std::string& sc) {
    if (sc == "static") s->isStatic = true;
    else if (sc == "typedef") s->isTypedef = true;
    else if (sc == "extern") s->isExtern = true;
}

// Integer value of a constant operand: a decimal or hex literal (optionally negative) or a
// simple character literal. Anything else (an expression the compiler did not fold) is
// reported as "not constant".
static bool constIntValue(const Operand* o, long& v) {
    if (o->kind != Operand::VALUE) return false;
    const std::string& p = o->place;
    if (p.size() >= 3 && p[0] == '\'' && p[p.size() - 1] == '\'') {
        if (p.size() == 3) { v = (unsigned char)p[1]; return true; }
        if (p.size() == 4 && p[1] == 92) {   // backslash escape
            switch (p[2]) {
                case 'n': v = 10; return true;
                case 't': v = 9; return true;
                case 'r': v = 13; return true;
                case '0': v = 0; return true;
                case 92: v = 92; return true;
                case 39: v = 39; return true;
            }
        }
        return false;
    }
    size_t i = (!p.empty() && p[0] == '-') ? 1 : 0;
    bool hex = p.size() > i + 2 && p[i] == '0' && (p[i + 1] == 'x' || p[i + 1] == 'X');
    if (hex) {
        for (size_t k = i + 2; k < p.size(); ++k)
            if (!isxdigit((unsigned char)p[k])) return false;
    } else if (!isNumber(p)) {
        return false;
    }
    v = strtol(p.c_str(), nullptr, hex ? 16 : 10);
    return true;
}

// Enumerator values: NAME takes the previous value + 1, NAME = expr takes expr.
// If an initializer cannot be evaluated the value stays unknown (for it and for the
// enumerators that follow), and the name is kept in the IR as before.
static long enum_next = 0;
static bool enum_next_known = true;

static void declareEnumConst(const std::string& name, Operand* init) {
    Symbol s;
    s.ty = makeType("int");
    s.mangled = name;
    s.isEnumConst = true;
    long v = 0;
    bool known_v = false;
    if (init == nullptr) {
        known_v = enum_next_known;
        v = enum_next;
    } else if (constIntValue(init, v)) {
        known_v = true;
    } else {
        semWarn("value of enumerator '" + name + "' is not a simple integer constant; the name is kept in the IR");
    }
    s.hasValue = known_v;
    s.value = v;
    enum_next_known = known_v;
    enum_next = v + 1;
    declareSym(name, s);
}

static void defineLabel(const std::string& name) {
    if (!fn_labels.insert(name).second) semError("duplicate label '" + name + "'");
}

/* ------------------------------------------------------------------
   Loop support. Each loop pushes one context. break/continue have their own
   stacks because a switch also accepts break.
   ------------------------------------------------------------------ */

struct LoopCtx {
    std::string start, cond, body, incr, end;
};
static std::vector<LoopCtx> loop_stack;

static void enterLoop(const LoopCtx& c, const std::string& continueTo) {
    loop_stack.push_back(c);
    continue_stack.push(continueTo);
    break_stack.push(c.end);
}

static LoopCtx leaveLoop() {
    LoopCtx c = loop_stack.back();
    loop_stack.pop_back();
    continue_stack.pop();
    break_stack.pop();
    return c;
}

// while (cond) body
static void whileBegin() {
    LoopCtx c;
    c.start = tacGen.newLabel();
    c.end = tacGen.newLabel();
    tacGen.emit("label", "", "", c.start);
    enterLoop(c, c.start);
}

static void whileCond(Operand* cond) {
    checkCondition(cond);
    tacGen.emit("ifFalse", rval(cond), "", loop_stack.back().end);
}

static void whileEnd() {
    LoopCtx c = leaveLoop();
    tacGen.emit("goto", "", "", c.start);
    tacGen.emit("label", "", "", c.end);
}

// do body while (cond);
static void doBegin() {
    LoopCtx c;
    c.start = tacGen.newLabel();
    c.cond = tacGen.newLabel();
    c.end = tacGen.newLabel();
    tacGen.emit("label", "", "", c.start);
    enterLoop(c, c.cond);
}

static void doCondLabel() {
    tacGen.emit("label", "", "", loop_stack.back().cond);
}

static void doEnd(Operand* cond) {
    checkCondition(cond);
    std::string v = rval(cond);
    LoopCtx c = leaveLoop();
    tacGen.emit("if", v, "", c.start);
    tacGen.emit("label", "", "", c.end);
}

// for (init; cond; incr) body  -- laid out as: cond, [exit test], goto body, incr, goto cond, body, goto incr
static void forBegin() {
    LoopCtx c;
    c.cond = tacGen.newLabel();
    c.body = tacGen.newLabel();
    c.incr = tacGen.newLabel();
    c.end = tacGen.newLabel();
    tacGen.emit("label", "", "", c.cond);
    enterLoop(c, c.incr);
}

static void forCond(Operand* cond) {
    checkCondition(cond);
    std::string v = rval(cond);
    const LoopCtx& c = loop_stack.back();
    if (!v.empty()) tacGen.emit("ifFalse", v, "", c.end);
    tacGen.emit("goto", "", "", c.body);
    tacGen.emit("label", "", "", c.incr);
}

static void forIncr(Operand* incr) {
    rval(incr);
    const LoopCtx& c = loop_stack.back();
    tacGen.emit("goto", "", "", c.cond);
    tacGen.emit("label", "", "", c.body);
}

static void forEnd() {
    LoopCtx c = leaveLoop();
    tacGen.emit("goto", "", "", c.incr);
    tacGen.emit("label", "", "", c.end);
    popScope();
}

/* ------------------------------------------------------------------
   switch support: the dispatch tests are emitted after the body and
   reached through a jump from the top.
   ------------------------------------------------------------------ */

static void beginSwitch(Operand* e) {
    if (known(e->ty) && !isIntegral(e->ty))
        semError("switch quantity is not an integer (type '" + typeName(e->ty) + "')");
    SwitchCtx c;
    c.value = rval(e);
    c.test = tacGen.newLabel();
    c.end = tacGen.newLabel();
    switch_stack.push_back(c);
    break_stack.push(c.end);
    tacGen.emit("goto", "", "", c.test);
}

static void addCase(Operand* e) {
    std::string value = rval(e);
    std::string lbl = tacGen.newLabel();
    tacGen.emit("label", "", "", lbl);
    if (switch_stack.empty()) {
        semError("case label not within a switch statement");
        return;
    }
    SwitchCtx& sw = switch_stack.back();
    if (!sw.seen.insert(value).second) semError("duplicate case value '" + value + "'");
    sw.cases.push_back(std::make_pair(value, lbl));
}

static void addDefault() {
    std::string lbl = tacGen.newLabel();
    tacGen.emit("label", "", "", lbl);
    if (switch_stack.empty()) {
        semError("default label not within a switch statement");
        return;
    }
    SwitchCtx& sw = switch_stack.back();
    if (sw.hasDefault) semError("multiple default labels in one switch");
    sw.hasDefault = true;
    sw.def = lbl;
}

static void endSwitch() {
    if (switch_stack.empty()) return;
    SwitchCtx c = switch_stack.back();
    switch_stack.pop_back();
    break_stack.pop();
    tacGen.emit("goto", "", "", c.end);
    tacGen.emit("label", "", "", c.test);
    for (size_t i = 0; i < c.cases.size(); ++i) {
        std::string t = tacGen.newTemp();
        tacGen.emit("==", c.value, c.cases[i].first, t);
        tacGen.emit("if", t, "", c.cases[i].second);
    }
    tacGen.emit("goto", "", "", c.def.empty() ? c.end : c.def);
    tacGen.emit("label", "", "", c.end);
}

%}

%union {
    char* str;
    Operand* op;
    Declarator* decl;
    DeclSpec* spec;
    Param* param;
    ParamList* params;
    InitNode* init;
    Ctx* ctx;
    int flag;
}

/* Tokens */
%token KW_AUTO KW_BREAK KW_BOOL KW_CASE KW_CHAR KW_CLASS KW_CONST KW_CONTINUE
%token KW_DEFAULT KW_DELETE KW_DO KW_DOUBLE KW_ELSE KW_ENUM KW_EXTERN KW_FALSE
%token KW_FLOAT KW_FOR KW_GOTO KW_IF KW_INLINE KW_INT KW_LONG KW_NEW
%token KW_PUBLIC KW_PRIVATE KW_PROTECTED KW_REGISTER KW_RESTRICT KW_RETURN
%token KW_SHORT KW_SIGNED KW_SIZEOF KW_STATIC KW_STRUCT KW_SWITCH KW_THIS
%token KW_TRUE KW_TYPEDEF KW_UNION KW_UNSIGNED KW_VOID KW_VOLATILE KW_WHILE

%token <str> IDENTIFIER
%token <str> INTEGER_LITERAL HEX_LITERAL FLOAT_LITERAL CHAR_LITERAL STRING_LITERAL

%token PLUS MINUS STAR SLASH PERCENT
%token ASSIGN PLUS_ASSIGN MINUS_ASSIGN MULTIPLY_ASSIGN DIVIDE_ASSIGN MODULO_ASSIGN
%token BITWISE_AND_ASSIGN BITWISE_OR_ASSIGN BITWISE_XOR_ASSIGN
%token EQUAL NOT_EQUAL LESS LESS_EQUAL GREATER GREATER_EQUAL
%token LOGICAL_AND LOGICAL_OR LOGICAL_NOT
%token BITWISE_AND BITWISE_OR BITWISE_XOR BITWISE_NOT
%token INCREMENT DECREMENT SHIFT_LEFT SHIFT_RIGHT
%token SHIFT_LEFT_ASSIGN SHIFT_RIGHT_ASSIGN
%token ARROW

%token SEMICOLON COMMA DOT COLON QUESTION
%token LEFT_PAREN RIGHT_PAREN LEFT_BRACKET RIGHT_BRACKET LEFT_BRACE RIGHT_BRACE

/* Precedence rules */
%right ASSIGN PLUS_ASSIGN MINUS_ASSIGN MULTIPLY_ASSIGN DIVIDE_ASSIGN MODULO_ASSIGN
        BITWISE_AND_ASSIGN BITWISE_OR_ASSIGN BITWISE_XOR_ASSIGN
        SHIFT_LEFT_ASSIGN SHIFT_RIGHT_ASSIGN
%right QUESTION
%left LOGICAL_OR
%left LOGICAL_AND
%left BITWISE_OR
%left BITWISE_XOR
%left BITWISE_AND
%left EQUAL NOT_EQUAL
%left LESS GREATER LESS_EQUAL GREATER_EQUAL
%left SHIFT_LEFT SHIFT_RIGHT
%left PLUS MINUS
%left STAR SLASH PERCENT

/* Dangling-else resolution */
%nonassoc LOWER_THAN_ELSE
%nonassoc KW_ELSE

/* Non-terminal semantic types */
%type <decl> declarator direct_declarator
%type <init> initializer initializer_list
%type <spec> declaration_specifiers decl_head
%type <str> type_specifier storage_class type_qualifier assignment_operator string_literal
%type <flag> pointer_opt access_specifier
%type <param> parameter_declaration
%type <params> parameter_list parameter_list_opt
%type <op> expression_opt expression assignment_expression conditional_expression
%type <op> logical_or_expression logical_and_expression
%type <op> bitwise_or_expression bitwise_xor_expression bitwise_and_expression
%type <op> equality_expression relational_expression shift_expression
%type <op> additive_expression multiplicative_expression
%type <op> unary_expression postfix_expression primary_expression

%start program

%%

/* ============================================================
   TRANSLATION UNIT
   ============================================================ */

program
    : external_declaration
    | program external_declaration
    ;

external_declaration
    : function_definition
    | declaration
    | class_definition
    | struct_definition
    | union_definition
    | enum_definition
    | SEMICOLON
    | error SEMICOLON { yyerrok; }
    ;

/* ============================================================
   CLASS / STRUCT DEFINITIONS
   ============================================================ */

class_definition
    : KW_CLASS IDENTIFIER { beginAggr($2, true); } LEFT_BRACE class_member_list_opt RIGHT_BRACE SEMICOLON { endAggr(); }
    ;

struct_definition
    : KW_STRUCT IDENTIFIER { beginAggr($2, false); } LEFT_BRACE struct_member_list_opt RIGHT_BRACE SEMICOLON { endAggr(); }
    ;

union_definition
    : KW_UNION IDENTIFIER { beginAggr($2, false); } LEFT_BRACE struct_member_list_opt RIGHT_BRACE SEMICOLON { endAggr(); }
    ;

enum_definition
    : KW_ENUM IDENTIFIER { enumTags.insert($2); enum_next = 0; enum_next_known = true; } LEFT_BRACE enumerator_list RIGHT_BRACE SEMICOLON
    ;

enumerator_list
    : enumerator
    | enumerator_list COMMA enumerator
    ;

enumerator
    : IDENTIFIER { declareEnumConst($1, nullptr); }
    | IDENTIFIER ASSIGN conditional_expression { use($3); declareEnumConst($1, $3); }
    ;

class_member_list_opt
    : /* empty */
    | class_member_list
    ;

class_member_list
    : class_member
    | class_member_list class_member
    ;

class_member
    : access_specifier COLON { cur_access = $1; }
    | declaration
    | function_definition
    ;

struct_member_list_opt
    : /* empty */
    | struct_member_list
    ;

struct_member_list
    : struct_member
    | struct_member_list struct_member
    ;

struct_member
    : declaration
    | function_definition
    ;

access_specifier
    : KW_PUBLIC     { $$ = 0; }
    | KW_PROTECTED  { $$ = 1; }
    | KW_PRIVATE    { $$ = 2; }
    ;

/* ============================================================
   DECLARATIONS
   ============================================================ */

decl_head
    : declaration_specifiers { spec_stack.push_back($1); $$ = $1; }
    ;

declaration
    : decl_head init_declarator_list_opt SEMICOLON { popSpec(); }
    | decl_head error SEMICOLON { popSpec(); yyerrok; }
    ;

declaration_specifiers
    : type_specifier                          { $$ = new DeclSpec(); $$->type = $1; }
    | type_qualifier                          { $$ = new DeclSpec(); if (!strcmp($1, "const")) $$->isConst = true; }
    | storage_class                           { $$ = new DeclSpec(); applyStorage($$, $1); }
    | declaration_specifiers type_specifier   { $$ = $1; $$->type = $2; }
    | declaration_specifiers type_qualifier   { $$ = $1; if (!strcmp($2, "const")) $$->isConst = true; }
    | declaration_specifiers storage_class    { $$ = $1; applyStorage($$, $2); }
    ;

storage_class
    : KW_AUTO       { $$ = strdup("auto"); }
    | KW_EXTERN     { $$ = strdup("extern"); }
    | KW_REGISTER   { $$ = strdup("register"); }
    | KW_STATIC     { $$ = strdup("static"); }
    | KW_TYPEDEF    { $$ = strdup("typedef"); }
    | KW_INLINE     { $$ = strdup("inline"); }
    ;

type_qualifier
    : KW_CONST      { $$ = strdup("const"); }
    | KW_VOLATILE   { $$ = strdup("volatile"); }
    | KW_RESTRICT   { $$ = strdup("restrict"); }
    ;

type_specifier
    : KW_VOID               { $$ = strdup("void"); }
    | KW_CHAR               { $$ = strdup("char"); }
    | KW_SHORT              { $$ = strdup("short"); }
    | KW_INT                { $$ = strdup("int"); }
    | KW_LONG               { $$ = strdup("long"); }
    | KW_FLOAT              { $$ = strdup("float"); }
    | KW_DOUBLE             { $$ = strdup("double"); }
    | KW_SIGNED             { $$ = strdup("signed"); }
    | KW_UNSIGNED           { $$ = strdup("unsigned"); }
    | KW_BOOL               { $$ = strdup("bool"); }
    | KW_STRUCT IDENTIFIER  { $$ = $2; }
    | KW_CLASS IDENTIFIER   { $$ = $2; }
    | KW_UNION IDENTIFIER   { $$ = $2; }
    | KW_ENUM IDENTIFIER    { $$ = $2; }
    ;

init_declarator_list_opt
    : /* empty */
    | init_declarator_list
    ;

init_declarator_list
    : init_declarator
    | init_declarator_list COMMA init_declarator
    ;

init_declarator
    : declarator                      { declareVar($1, currentSpec(), nullptr); }
    | declarator ASSIGN initializer   { declareVar($1, currentSpec(), $3); }
    ;

initializer
    : assignment_expression {
        $$ = new InitNode();
        $$->op = $1;
    }
    | LEFT_BRACE initializer_list comma_opt RIGHT_BRACE { $$ = $2; }
    ;

initializer_list
    : initializer {
        finalizeLeaf($1);
        $$ = new InitNode();
        $$->isList = true;
        $$->items.push_back($1);
    }
    | initializer_list COMMA initializer {
        finalizeLeaf($3);
        $$ = $1;
        $$->items.push_back($3);
    }
    ;

comma_opt
    : /* empty */
    | COMMA
    ;

declarator
    : pointer_opt direct_declarator {
        $$ = $2;
        $$->ptr += ($1 >> 1);
        if ($1 & 1) $$->isRef = true;
    }
    ;

/* value: (number of '*') << 1 | (1 if an '&' was seen, i.e. a reference) */
pointer_opt
    : /* empty */               { $$ = 0; }
    | STAR pointer_opt          { $$ = $2 + 2; }
    | BITWISE_AND pointer_opt   { $$ = $2 | 1; }
    ;

direct_declarator
    : IDENTIFIER {
        $$ = new Declarator();
        $$->name = $1;
    }
    | LEFT_PAREN declarator RIGHT_PAREN {
        $$ = $2;
        $$->paren = true;
    }
    | direct_declarator LEFT_BRACKET expression_opt RIGHT_BRACKET {
        $$ = $1;
        $$->dims.push_back(rval($3));
    }
    | direct_declarator LEFT_PAREN parameter_list_opt RIGHT_PAREN {
        $$ = $1;
        if ($$->paren && $$->ptr > 0) {
            $$->isFuncPtr = true;          // int (*fp)(int): a pointer to function, not a function
        } else {
            $$->isFunc = true;
            $$->params = $3->items;
            normalizeParams($$->params);
        }
    }
    ;

/* ============================================================
   FUNCTIONS / PARAMETERS
   ============================================================ */

function_definition
    : decl_head declarator {
        beginFunction($2);
    } compound_statement {
        endFunction();
    }
    ;

parameter_list_opt
    : /* empty */ { $$ = new ParamList(); }
    | parameter_list { $$ = $1; }
    ;

parameter_list
    : parameter_declaration {
        $$ = new ParamList();
        $$->items.push_back(*$1);
    }
    | parameter_list COMMA parameter_declaration {
        $$ = $1;
        $$->items.push_back(*$3);
    }
    ;

parameter_declaration
    : declaration_specifiers declarator {
        $$ = new Param();
        $$->name = $2->name;
        $$->type = $1->type;
        $$->dims = $2->dims;
        $$->isRef = $2->isRef;
        $$->ptr = $2->ptr;
        $$->isFuncPtr = $2->isFuncPtr || $2->isFunc;
        if (!$$->isFuncPtr) $$->ty = typeFromName($1->type, $2->ptr, (int)$2->dims.size(), $1->isConst);
    }
    | declaration_specifiers {
        $$ = new Param();
        $$->type = $1->type;
        $$->ty = typeFromName($1->type, 0, 0, $1->isConst);
    }
    ;

/* ============================================================
   STATEMENTS
   ============================================================ */

open_brace
    : LEFT_BRACE { pushScope(); }
    ;

compound_statement
    : open_brace block_items_opt RIGHT_BRACE { popScope(); }
    | open_brace error RIGHT_BRACE { popScope(); yyerrok; }
    ;

block_items_opt
    : /* empty */
    | block_items
    ;

block_items
    : block_item
    | block_items block_item
    ;

block_item
    : declaration
    | statement
    | struct_definition
    | union_definition
    | enum_definition
    ;

statement
    : compound_statement
    | expression_statement
    | selection_statement
    | iteration_statement
    | jump_statement
    | labeled_statement
    | error SEMICOLON { yyerrok; }
    ;

labeled_statement
    : IDENTIFIER COLON {
        defineLabel($1);
        tacGen.emit("label", "", "", $1);
    } statement
    | KW_CASE conditional_expression COLON {
        addCase($2);
    } statement
    | KW_DEFAULT COLON {
        addDefault();
    } statement
    ;

expression_statement
    : expression_opt SEMICOLON { use($1); }
    ;

if_cond
    : LEFT_PAREN expression RIGHT_PAREN {
        checkCondition($2);
        std::string cond = rval($2);
        std::string false_label = tacGen.newLabel();
        tacGen.emit("ifFalse", cond, "", false_label);
        label_stack.push(false_label);
    }
    ;

selection_statement
    : KW_IF if_cond statement %prec LOWER_THAN_ELSE {
        std::string false_label = label_stack.top();
        label_stack.pop();
        tacGen.emit("label", "", "", false_label);
    }
    | KW_IF if_cond statement KW_ELSE {
        std::string false_label = label_stack.top();
        label_stack.pop();
        std::string end_label = tacGen.newLabel();
        tacGen.emit("goto", "", "", end_label);
        tacGen.emit("label", "", "", false_label);
        label_stack.push(end_label);
    } statement {
        std::string end_label = label_stack.top();
        label_stack.pop();
        tacGen.emit("label", "", "", end_label);
    }
    | KW_SWITCH LEFT_PAREN expression RIGHT_PAREN {
        beginSwitch($3);
    } statement {
        endSwitch();
    }
    ;

/* 'for' opens its own scope so that  for (int i...)  can be repeated in one block */
for_open
    : KW_FOR LEFT_PAREN { pushScope(); }
    ;

for_init
    : expression_opt SEMICOLON { use($1); }
    | declaration
    ;

iteration_statement
    : KW_WHILE { whileBegin(); } LEFT_PAREN expression RIGHT_PAREN { whileCond($4); } statement { whileEnd(); }
    | KW_DO { doBegin(); } statement KW_WHILE { doCondLabel(); } LEFT_PAREN expression RIGHT_PAREN SEMICOLON { doEnd($7); }
    | for_open for_init { forBegin(); } expression_opt SEMICOLON { forCond($4); } expression_opt RIGHT_PAREN { forIncr($7); } statement { forEnd(); }
    ;

jump_statement
    : KW_GOTO IDENTIFIER SEMICOLON {
        fn_gotos.push_back(std::make_pair(std::string($2), yylineno));
        tacGen.emit("goto", "", "", $2);
    }
    | KW_BREAK SEMICOLON {
        if (break_stack.empty()) semError("break statement not within a loop or switch");
        else tacGen.emit("goto", "", "", break_stack.top());
    }
    | KW_CONTINUE SEMICOLON {
        if (continue_stack.empty()) semError("continue statement not within a loop");
        else tacGen.emit("goto", "", "", continue_stack.top());
    }
    | KW_RETURN expression_opt SEMICOLON {
        checkReturn($2);
        tacGen.emit("return", returnValue($2), "", "");
    }
    ;

/* ============================================================
   EXPRESSIONS
   ============================================================ */

expression_opt
    : /* empty */ { $$ = valueOp(""); }
    | expression  { $$ = $1; }
    ;

expression
    : assignment_expression { $$ = $1; }
    | expression COMMA assignment_expression { $$ = $3; }
    ;

assignment_expression
    : conditional_expression { $$ = $1; }
    | unary_expression assignment_operator assignment_expression {
        $$ = assign($1, $2, $3);
    }
    ;

assignment_operator
    : ASSIGN              { $$ = strdup("="); }
    | PLUS_ASSIGN         { $$ = strdup("+="); }
    | MINUS_ASSIGN        { $$ = strdup("-="); }
    | MULTIPLY_ASSIGN     { $$ = strdup("*="); }
    | DIVIDE_ASSIGN       { $$ = strdup("/="); }
    | MODULO_ASSIGN       { $$ = strdup("%="); }
    | BITWISE_AND_ASSIGN  { $$ = strdup("&="); }
    | BITWISE_OR_ASSIGN   { $$ = strdup("|="); }
    | BITWISE_XOR_ASSIGN  { $$ = strdup("^="); }
    | SHIFT_LEFT_ASSIGN   { $$ = strdup("<<="); }
    | SHIFT_RIGHT_ASSIGN  { $$ = strdup(">>="); }
    ;

/* Only the selected branch is evaluated. */
conditional_expression
    : logical_or_expression { $$ = $1; }
    | logical_or_expression QUESTION {
        Ctx* c = new Ctx();
        checkCondition($1);
        std::string cond = rval($1);
        c->a = tacGen.newTemp();
        c->b = tacGen.newLabel();
        c->c = tacGen.newLabel();
        tacGen.emit("ifFalse", cond, "", c->b);
        $<ctx>$ = c;
    } expression {
        Ctx* c = $<ctx>3;
        tacGen.emit("=", rval($4), "", c->a);
        tacGen.emit("goto", "", "", c->c);
        tacGen.emit("label", "", "", c->b);
    } COLON conditional_expression {
        Ctx* c = $<ctx>3;
        tacGen.emit("=", rval($7), "", c->a);
        tacGen.emit("label", "", "", c->c);
        $$ = typedOp(c->a, commonType($4, $7));
    }
    ;

/* && and || short-circuit and always yield 0 or 1. */
logical_or_expression
    : logical_and_expression { $$ = $1; }
    | logical_or_expression LOGICAL_OR {
        Ctx* c = new Ctx();
        checkCondition($1);
        std::string v = rval($1);
        c->b = tacGen.newLabel();
        tacGen.emit("if", v, "", c->b);
        $<ctx>$ = c;
    } logical_and_expression {
        Ctx* c = $<ctx>3;
        checkCondition($4);
        std::string v = rval($4);
        std::string t = tacGen.newTemp();
        std::string end_label = tacGen.newLabel();
        tacGen.emit("if", v, "", c->b);
        tacGen.emit("=", "0", "", t);
        tacGen.emit("goto", "", "", end_label);
        tacGen.emit("label", "", "", c->b);
        tacGen.emit("=", "1", "", t);
        tacGen.emit("label", "", "", end_label);
        $$ = typedOp(t, makeType("int"));
    }
    ;

logical_and_expression
    : bitwise_or_expression { $$ = $1; }
    | logical_and_expression LOGICAL_AND {
        Ctx* c = new Ctx();
        checkCondition($1);
        std::string v = rval($1);
        c->b = tacGen.newLabel();
        tacGen.emit("ifFalse", v, "", c->b);
        $<ctx>$ = c;
    } bitwise_or_expression {
        Ctx* c = $<ctx>3;
        checkCondition($4);
        std::string v = rval($4);
        std::string t = tacGen.newTemp();
        std::string end_label = tacGen.newLabel();
        tacGen.emit("ifFalse", v, "", c->b);
        tacGen.emit("=", "1", "", t);
        tacGen.emit("goto", "", "", end_label);
        tacGen.emit("label", "", "", c->b);
        tacGen.emit("=", "0", "", t);
        tacGen.emit("label", "", "", end_label);
        $$ = typedOp(t, makeType("int"));
    }
    ;

bitwise_or_expression
    : bitwise_xor_expression { $$ = $1; }
    | bitwise_or_expression BITWISE_OR bitwise_xor_expression { $$ = binop("|", $1, $3); }
    ;

bitwise_xor_expression
    : bitwise_and_expression { $$ = $1; }
    | bitwise_xor_expression BITWISE_XOR bitwise_and_expression { $$ = binop("^", $1, $3); }
    ;

bitwise_and_expression
    : equality_expression { $$ = $1; }
    | bitwise_and_expression BITWISE_AND equality_expression { $$ = binop("&", $1, $3); }
    ;

equality_expression
    : relational_expression { $$ = $1; }
    | equality_expression EQUAL relational_expression { $$ = binop("==", $1, $3); }
    | equality_expression NOT_EQUAL relational_expression { $$ = binop("!=", $1, $3); }
    ;

relational_expression
    : shift_expression { $$ = $1; }
    | relational_expression LESS shift_expression { $$ = binop("<", $1, $3); }
    | relational_expression LESS_EQUAL shift_expression { $$ = binop("<=", $1, $3); }
    | relational_expression GREATER shift_expression { $$ = binop(">", $1, $3); }
    | relational_expression GREATER_EQUAL shift_expression { $$ = binop(">=", $1, $3); }
    ;

shift_expression
    : additive_expression { $$ = $1; }
    | shift_expression SHIFT_LEFT additive_expression { $$ = binop("<<", $1, $3); }
    | shift_expression SHIFT_RIGHT additive_expression { $$ = binop(">>", $1, $3); }
    ;

additive_expression
    : multiplicative_expression { $$ = $1; }
    | additive_expression PLUS multiplicative_expression { $$ = binop("+", $1, $3); }
    | additive_expression MINUS multiplicative_expression { $$ = binop("-", $1, $3); }
    ;

multiplicative_expression
    : unary_expression { $$ = $1; }
    | multiplicative_expression STAR unary_expression { $$ = binop("*", $1, $3); }
    | multiplicative_expression SLASH unary_expression { $$ = binop("/", $1, $3); }
    | multiplicative_expression PERCENT unary_expression { $$ = binop("%", $1, $3); }
    ;

unary_expression
    : postfix_expression { $$ = $1; }
    | INCREMENT unary_expression { $$ = preIncDec($2, "+"); }
    | DECREMENT unary_expression { $$ = preIncDec($2, "-"); }
    | BITWISE_AND unary_expression { $$ = addrOp($2); }
    | STAR unary_expression { $$ = derefOp($2); }
    | PLUS unary_expression { $$ = unaryPlus($2); }
    | MINUS unary_expression { $$ = negate($2); }
    | BITWISE_NOT unary_expression { $$ = bitNot($2); }
    | LOGICAL_NOT unary_expression { $$ = logNot($2); }
    | KW_SIZEOF unary_expression {
        // sizeof does not evaluate its operand, so no load is emitted for it
        use($2);
        std::string v;
        if ($2->kind == Operand::DEREF) v = "*" + $2->place;
        else if ($2->kind == Operand::INDEX && $2->used >= totalDims($2)) v = $2->place + "[" + $2->index + "]";
        else v = rval($2);
        std::string t = tacGen.newTemp();
        tacGen.emit("sizeof", v, "", t);
        $$ = typedOp(t, makeType("int"));
    }
    | KW_SIZEOF LEFT_PAREN type_specifier pointer_opt RIGHT_PAREN {
        std::string t = tacGen.newTemp();
        tacGen.emit("sizeof", $3, "", t);
        $$ = typedOp(t, makeType("int"));
    }
    | KW_NEW type_specifier {
        std::string t = tacGen.newTemp();
        tacGen.emit("new", $2, "", t);
        $$ = typedOp(t, typeFromName($2, 1, 0, false));
    }
    | KW_NEW type_specifier LEFT_BRACKET expression RIGHT_BRACKET {
        if (known($4->ty) && !isIntegral($4->ty)) semError("size in new-expression must be an integer");
        std::string n = rval($4);
        std::string t = tacGen.newTemp();
        tacGen.emit("newarr", $2, n, t);
        $$ = typedOp(t, typeFromName($2, 1, 0, false));
    }
    | KW_DELETE unary_expression {
        if (known($2->ty) && levels($2->ty) == 0) semError("type argument of 'delete' is not a pointer");
        tacGen.emit("delete", rval($2), "", "");
        $$ = typedOp("", makeType("void"));
    }
    | KW_DELETE LEFT_BRACKET RIGHT_BRACKET unary_expression {
        if (known($4->ty) && levels($4->ty) == 0) semError("type argument of 'delete[]' is not a pointer");
        tacGen.emit("deletearr", rval($4), "", "");
        $$ = typedOp("", makeType("void"));
    }
    ;

postfix_expression
    : primary_expression { $$ = $1; }
    | postfix_expression LEFT_BRACKET expression RIGHT_BRACKET {
        $$ = subscript($1, $3);
    }
    | postfix_expression LEFT_PAREN {
        CallFrame fr;
        fr.callee = calleeName($1);
        noteCall(fr.callee);
        arg_stack.push(fr);
    } argument_list_opt RIGHT_PAREN {
        std::vector<std::string> args = arg_stack.top().args;
        arg_stack.pop();
        $$ = makeCall($1, args);
    }
    | postfix_expression DOT IDENTIFIER {
        $$ = member($1, $3, false);
    }
    | postfix_expression ARROW IDENTIFIER {
        $$ = member($1, $3, true);
    }
    | postfix_expression INCREMENT { $$ = postIncDec($1, "+"); }
    | postfix_expression DECREMENT { $$ = postIncDec($1, "-"); }
    ;

argument_list_opt
    : /* empty */
    | argument_list
    ;

argument_list
    : assignment_expression                      { pushArg($1); }
    | argument_list COMMA assignment_expression  { pushArg($3); }
    ;

primary_expression
    : IDENTIFIER        { $$ = identOp($1); }
    | INTEGER_LITERAL   { $$ = typedOp($1, makeType("int")); }
    | HEX_LITERAL       { $$ = typedOp($1, makeType("int")); }
    | FLOAT_LITERAL     { $$ = typedOp($1, makeType("double")); }
    | CHAR_LITERAL      { $$ = typedOp($1, makeType("char")); }
    | string_literal    { $$ = typedOp($1, makeType("char", 1)); $$->isStr = true; }
    | KW_TRUE           { $$ = typedOp("1", makeType("bool")); }
    | KW_FALSE          { $$ = typedOp("0", makeType("bool")); }
    | KW_THIS {
        $$ = valueOp("this");
        if (inMethod()) $$->ty = makeType(cur_aggr, 1, 0, true);
        else semError("invalid use of 'this' outside of a member function");
    }
    | LEFT_PAREN expression RIGHT_PAREN { $$ = $2; }
    ;

string_literal
    : STRING_LITERAL { $$ = $1; }
    | string_literal STRING_LITERAL {
        std::string combined = std::string($1) + std::string($2);
        $$ = strdup(combined.c_str());
    }
    ;

%%

void yyerror(const char* s) {
    has_error = 1;
    fprintf(stderr, "SYNTAX ERROR: %s at line %d (near '%s')\n", s, yylineno, yytext);
}

int main(int argc, char** argv) {
    std::string input_path = "";
    std::string output_path = "";

    if (argc > 1) {
        input_path = argv[1];
        FILE* file = fopen(input_path.c_str(), "r");
        if (!file) {
            perror("Error opening file");
            return 1;
        }
        yyin = file;
    }

    if (argc > 2) {
        output_path = argv[2];
    } else if (!input_path.empty()) {
        // Default naming convention: replace .c extension with .tac
        size_t dot_pos = input_path.find_last_of('.');
        if (dot_pos != std::string::npos) {
            output_path = input_path.substr(0, dot_pos) + ".tac";
        } else {
            output_path = input_path + ".tac";
        }
    }

    int res = yyparse();

    if (res == 0 && !has_error) {
        printf("\n=> PARSE SUCCESSFUL: No syntax errors found.\n");
        printf("=> SEMANTIC CHECK PASSED: %d warning(s).\n", sem_warnings);
        tacGen.printTAC(stdout);

        if (!output_path.empty()) {
            if (tacGen.saveTAC(output_path)) {
                printf("\n=> IR saved to: %s\n", output_path.c_str());
            }
        }
        return 0;
    }

    if (sem_errors > 0)
        fprintf(stderr, "\n=> SEMANTIC ANALYSIS FAILED: %d error(s), %d warning(s). No IR generated.\n",
                sem_errors, sem_warnings);
    return 1;
}
