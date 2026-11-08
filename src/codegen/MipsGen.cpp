#include <cctype>
#include "MipsGen.hpp"

#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <sstream>

// Code generation scheme
// ----------------------
// * Every variable and every temporary has a slot in the stack frame ($fp-relative); globals live in
//   the data segment. Each TAC instruction loads its operands into $t0/$t1/$t2, computes, and stores the
//   result back, so the generated code is simple and always correct (it is not fast).
// * Frame layout:      8($fp)+   parameters (the first argument is at the lowest address)
//                      4($fp)    saved $ra          0($fp)   saved $fp
//                      -4($fp)-  locals and temporaries
// * Calls: the caller pushes the arguments right to left (a method's object pointer last), calls with
//   jal, then pops them. A function returning a struct by value gets one more, hidden, argument: the
//   address of the result. The result of every other function is in $v0.
// * Runtime functions (printf, malloc, ...) use the same convention and are written in assembly
//   (Runtime.cpp); they read argument 0 at 0($sp).

namespace {

// Where a variable or temporary lives and how it is accessed.
struct Loc {
    bool global = false;
    int off = 0;               // frame offset from $fp (not global)
    std::string label;         // data label (global)
    long bytes = 4;            // size of the object
    int access = 4;            // bytes read / written when it is used as a scalar: 1, 2 or 4
    bool aggr = false;         // struct / class / union object: copied as a whole
    bool array = false;        // array object: its name is its address
    bool isRef = false;
};

long roundUp(long v, long a) { return (v + a - 1) / a * a; }

// A C identifier or label fragment made safe for the assembler: Class::method -> Class__method.
std::string sanitize(const std::string& s) {
    std::string r;
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (c == ':' && i + 1 < s.size() && s[i + 1] == ':') { r += "__"; ++i; }
        else if (isalnum((unsigned char)c) || c == '_') r += c;
        else r += '_';
    }
    return r;
}

// Value of an escape sequence starting after the backslash at s[i]; advances i past it.
int decodeEscape(const std::string& s, size_t& i) {
    char c = s[i++];
    switch (c) {
        case 'n': return 10;
        case 't': return 9;
        case 'r': return 13;
        case 'a': return 7;
        case 'b': return 8;
        case 'f': return 12;
        case 'v': return 11;
        case 'x': {
            int v = 0;
            while (i < s.size() && isxdigit((unsigned char)s[i])) {
                char h = s[i++];
                v = v * 16 + (isdigit((unsigned char)h) ? h - '0' : (tolower(h) - 'a' + 10));
            }
            return v & 255;
        }
        default:
            if (c >= '0' && c <= '7') {
                int v = c - '0';
                for (int k = 0; k < 2 && i < s.size() && s[i] >= '0' && s[i] <= '7'; ++k) v = v * 8 + (s[i++] - '0');
                return v & 255;
            }
            return (unsigned char)c;        // \\ \' \" \?
    }
}

// Bytes of a string literal (given with its quotes), without the terminating NUL.
std::vector<int> decodeString(const std::string& lit) {
    std::vector<int> out;
    size_t i = 1;
    while (i + 1 < lit.size()) {
        if (lit[i] == '\\' && i + 1 < lit.size() - 1) { ++i; out.push_back(decodeEscape(lit, i)); }
        else out.push_back((unsigned char)lit[i++]);
    }
    return out;
}

// Value of an integer or character literal; false if the operand is anything else.
bool parseLiteral(const std::string& op, long& v) {
    if (op.empty()) return false;
    if (op[0] == '\'') {
        if (op.size() < 3) return false;
        size_t i = 1;
        if (op[i] == '\\') { ++i; v = decodeEscape(op, i); }
        else v = (unsigned char)op[i];
        if (v > 127) v -= 256;
        return true;
    }
    size_t i = (op[0] == '-') ? 1 : 0;
    if (i >= op.size() || !isdigit((unsigned char)op[i])) return false;
    for (size_t k = i; k < op.size(); ++k) {
        char c = op[k];
        if (!(isxdigit((unsigned char)c) || c == 'x' || c == 'X')) return false;   // 1.5, 1e3, 10L ...
    }
    char* end = nullptr;
    v = strtol(op.c_str(), &end, 0);
    return end && *end == '\0';
}

bool looksLikeFloat(const std::string& op) {
    if (op.empty()) return false;
    size_t i = (op[0] == '-') ? 1 : 0;
    return i < op.size() && (isdigit((unsigned char)op[i]) || op[i] == '.') && op.find_first_of(".eE") != std::string::npos &&
           op.compare(i, 2, "0x") != 0 && op.compare(i, 2, "0X") != 0;
}

// Names a program may use without declaring them (they normally come from headers).
bool builtinConstant(const std::string& n, long& v) {
    static const std::map<std::string, long> t = {
        {"NULL", 0}, {"EOF", -1}, {"INT_MAX", 2147483647L}, {"INT_MIN", -2147483647L - 1},
        {"CHAR_BIT", 8}, {"CHAR_MAX", 127}, {"CHAR_MIN", -128}, {"SHRT_MAX", 32767}, {"SHRT_MIN", -32768},
        {"UCHAR_MAX", 255}, {"EXIT_SUCCESS", 0}, {"EXIT_FAILURE", 1}, {"RAND_MAX", 2147483647L},
        {"LONG_MAX", 2147483647L}, {"LONG_MIN", -2147483647L - 1}, {"UINT_MAX", -1}, {"BUFSIZ", 512},
        {"SEEK_SET", 0}, {"SEEK_CUR", 1}, {"SEEK_END", 2}};
    std::map<std::string, long>::const_iterator it = t.find(n);
    if (it == t.end()) return false;
    v = it->second;
    return true;
}

// One compilation unit: the user's program or the runtime library (which is itself compiled from C source).
class Gen {
public:
    // 'pfx' is put in front of every label this unit defines ("" for the program, "R" for the runtime), so
    // the two units cannot clash. 'externs' maps the names of functions defined by the other unit to their
    // labels. 'only', when given, restricts the output to these functions (the runtime is pruned).
    Gen(const TACGenerator& t, const LayoutTable& l, std::vector<std::string>& e, const std::string& prefix,
        const std::map<std::string, std::string>& ext, const std::set<std::string>* onlyFuncs, bool entryPoint)
        : tac(t), layouts(l), errors(e), pfx(prefix), externs(ext), only(onlyFuncs), isProgram(entryPoint) {}

    bool generate();                               // false if errors were reported
    std::string dataText() const { return data.str(); }
    std::string codeText() const { return text.str(); }
    const std::set<std::string>& externsUsed() const { return usedExterns; }

private:
    const TACGenerator& tac;
    const LayoutTable& layouts;
    std::vector<std::string>& errors;
    std::string pfx;
    const std::map<std::string, std::string>& externs;
    const std::set<std::string>* only;
    bool isProgram;                                // emits the entry point and requires main

    std::ostringstream text;                       // the code segment
    std::ostringstream data;                       // the data segment
    std::set<std::string> usedExterns;             // functions of the other unit that this one refers to
    std::map<std::string, Loc> globals;            // data-segment variables by IR name
    std::set<std::string> userFuncs;               // IR names of the functions defined by this unit
    std::vector<std::pair<std::string, std::vector<int> > > strings;   // string pool: label, bytes
    std::map<std::string, std::string> stringLabel;                     // literal text -> label

    // state of the function being generated
    std::map<std::string, Loc> locs;
    std::string fname;                             // sanitized function name
    bool hiddenRet = false;                        // returns a struct by value
    long pending = 0;                              // bytes pushed for the call being prepared

    void error(const std::string& msg) { errors.push_back(msg); }
    void ins(const std::string& s) { text << "    " << s << "\n"; }
    void label(const std::string& s) { text << s << ":\n"; }

    Loc makeLoc(const VarInfo& v) const;
    const Loc* find(const std::string& name) const;
    // Label of a function: one of this unit, or one defined by the other unit (then it is recorded).
    std::string funcLabel(const std::string& name) {
        if (!userFuncs.count(name)) {
            std::map<std::string, std::string>::const_iterator it = externs.find(name);
            if (it != externs.end()) { usedExterns.insert(name); return it->second; }
        }
        return pfx + "F_" + sanitize(name);
    }
    bool isFunc(const std::string& name) const { return userFuncs.count(name) || externs.count(name); }
    std::string stringLabelFor(const std::string& lit);

    bool loadValue(const std::string& op, const char* reg);
    bool addrOfName(const std::string& name, const char* reg);
    void storeValue(const char* reg, const std::string& name);
    void emitCopy(long n);                         // copy n bytes from ($t0) to ($t1)
    void pushReg(const char* reg);
    static bool fitsImm(long v) { return v >= -32768 && v <= 32767; }
    // Memory operand for a frame slot. SPIM cannot encode offsets beyond 16 bits, so a distant slot
    // (a frame with a big local array) is reached through $t9.
    std::string frameRef(int off) {
        if (fitsImm(off)) return std::to_string(off) + "($fp)";
        ins("li $t9, " + std::to_string(off));
        ins("addu $t9, $t9, $fp");
        return "0($t9)";
    }

    void beginFunction(const FuncInfo& f, bool hidden);
    void epilogue();
    void genFunction(const FuncInfo& f, const std::vector<TACInstruction>& code, size_t& pos, bool isInit);
    void genInstr(const TACInstruction& in);
    void genBinary(const TACInstruction& in);
    void genCall(const TACInstruction& in);
    void genReturn(const TACInstruction& in);
    void genLoadMem(const TACInstruction& in, bool indexed, bool addrOnly);
    void genStoreMem(const TACInstruction& in, bool indexed);
    void loadFromAddr(const std::string& result, int sz);   // value at ($t0) into the result
    std::string userLabel(const std::string& n) const { return pfx + "U_" + fname + "_" + sanitize(n); }

    void emitData();
};

Loc Gen::makeLoc(const VarInfo& v) const {
    Loc l;
    bool ptrLike = v.isRef || (v.isParam && !v.dims.empty());
    l.isRef = v.isRef;
    if (ptrLike) {
        l.bytes = 4;
    } else if (!v.dims.empty()) {
        l.array = true;
        l.bytes = layouts.objectSize(v.ty, v.dims);
    } else if (v.ty.aggr && v.ty.ptr == 0) {
        l.aggr = true;
        l.bytes = layouts.baseSize(v.ty);
    } else {
        long s = layouts.valueSize(v.ty);
        l.bytes = s;
        l.access = (v.isParam || s >= 4) ? 4 : (int)s;      // arguments are pushed as whole words
        if (l.bytes < 4) l.bytes = 4;
        if (l.bytes > 4) l.access = 4;                       // double: not supported yet
    }
    if (l.bytes < 1) l.bytes = 1;
    return l;
}

const Loc* Gen::find(const std::string& name) const {
    std::map<std::string, Loc>::const_iterator it = locs.find(name);
    if (it != locs.end()) return &it->second;
    it = globals.find(name);
    return it == globals.end() ? nullptr : &it->second;
}

std::string Gen::stringLabelFor(const std::string& lit) {
    std::map<std::string, std::string>::iterator it = stringLabel.find(lit);
    if (it != stringLabel.end()) return it->second;
    std::string l = pfx + "S_" + std::to_string(strings.size());
    strings.push_back(std::make_pair(l, decodeString(lit)));
    stringLabel[lit] = l;
    return l;
}

// Put the value of an operand into a register. An array (or a function name) yields its address.
bool Gen::loadValue(const std::string& op, const char* reg) {
    long v;
    if (parseLiteral(op, v)) { ins(std::string("li ") + reg + ", " + std::to_string(v)); return true; }
    if (!op.empty() && op[0] == '"') { ins(std::string("la ") + reg + ", " + stringLabelFor(op)); return true; }
    if (looksLikeFloat(op)) {
        error("floating-point values are not supported yet (" + op + ")");
        ins(std::string("li ") + reg + ", 0");
        return false;
    }
    const Loc* l = find(op);
    if (l) {
        if (l->array || l->aggr) return addrOfName(op, reg);
        const char* mn = l->access == 1 ? "lb " : (l->access == 2 ? "lh " : "lw ");
        if (l->global) ins(std::string(mn) + reg + ", " + l->label);
        else ins(std::string(mn) + reg + ", " + frameRef(l->off));
        return true;
    }
    if (builtinConstant(op, v)) { ins(std::string("li ") + reg + ", " + std::to_string(v)); return true; }
    if (isFunc(op)) { ins(std::string("la ") + reg + ", " + funcLabel(op)); return true; }
    error("unknown name '" + op + "' (not a variable, constant or function of this program)");
    ins(std::string("li ") + reg + ", 0");
    return false;
}

// Address of the variable itself (not of what a reference refers to).
bool Gen::addrOfName(const std::string& name, const char* reg) {
    const Loc* l = find(name);
    if (!l) {
        if (isFunc(name)) { ins(std::string("la ") + reg + ", " + funcLabel(name)); return true; }
        error("cannot take the address of '" + name + "'");
        ins(std::string("li ") + reg + ", 0");
        return false;
    }
    if (l->global) ins(std::string("la ") + reg + ", " + l->label);
    else if (fitsImm(l->off)) ins(std::string("addiu ") + reg + ", $fp, " + std::to_string(l->off));
    else { ins(std::string("li ") + reg + ", " + std::to_string(l->off)); ins(std::string("addu ") + reg + ", " + reg + ", $fp"); }
    return true;
}

void Gen::storeValue(const char* reg, const std::string& name) {
    const Loc* l = find(name);
    if (!l) { error("cannot assign to '" + name + "'"); return; }
    const char* mn = l->access == 1 ? "sb " : (l->access == 2 ? "sh " : "sw ");
    if (l->global) ins(std::string(mn) + reg + ", " + l->label);
    else ins(std::string(mn) + reg + ", " + frameRef(l->off));
}

void Gen::pushReg(const char* reg) {
    ins("addiu $sp, $sp, -4");
    ins(std::string("sw ") + reg + ", 0($sp)");
    pending += 4;
}

// Copy n bytes from the address in $t0 to the address in $t1 (both registers may be changed).
void Gen::emitCopy(long n) {
    if (n <= 0) return;
    if (n % 4 != 0) {                       // objects with an alignment below 4: byte by byte
        for (long i = 0; i < n; ++i) {
            ins("lb $t8, " + std::to_string(i) + "($t0)");
            ins("sb $t8, " + std::to_string(i) + "($t1)");
        }
        return;
    }
    if (n <= 64) {
        for (long i = 0; i < n; i += 4) {
            ins("lw $t8, " + std::to_string(i) + "($t0)");
            ins("sw $t8, " + std::to_string(i) + "($t1)");
        }
        return;
    }
    static int copyLoops = 0;
    std::string l = pfx + "copy_" + std::to_string(copyLoops++);
    ins("li $t7, " + std::to_string(n / 4));
    label(l);
    ins("lw $t8, 0($t0)");
    ins("sw $t8, 0($t1)");
    ins("addiu $t0, $t0, 4");
    ins("addiu $t1, $t1, 4");
    ins("addiu $t7, $t7, -1");
    ins("bnez $t7, " + l);
}

void Gen::beginFunction(const FuncInfo& f, bool hidden) {
    locs.clear();
    pending = 0;
    hiddenRet = hidden;
    fname = sanitize(f.name);

    int paramOff = 8 + (hidden ? 4 : 0);
    for (size_t i = 0; i < f.params.size(); ++i) {
        Loc l = makeLoc(f.params[i]);
        l.off = paramOff;
        paramOff += (int)(l.aggr ? roundUp(l.bytes, 4) : 4);
        locs[f.params[i].name] = l;
    }
    long cur = 0;
    for (size_t i = 0; i < f.locals.size(); ++i) {
        Loc l = makeLoc(f.locals[i]);
        cur += roundUp(l.bytes, 4);
        l.off = (int)-cur;
        locs[f.locals[i].name] = l;
    }
    for (size_t i = 0; i < f.temps.size(); ++i) {
        const Type& t = f.temps[i].second;
        Loc l;
        if (t.aggr && t.ptr == 0 && t.arr == 0) {
            l.aggr = true;
            l.bytes = layouts.baseSize(t);
            if (l.bytes < 1) l.bytes = 1;
        }
        cur += roundUp(l.bytes, 4);
        l.off = (int)-cur;
        locs[f.temps[i].first] = l;
    }
    long frame = roundUp(cur, 8);

    text << "\n";
    label(f.name == "_global_init" ? pfx + "__global_init" : funcLabel(f.name));
    ins("addiu $sp, $sp, -8");
    ins("sw $ra, 4($sp)");
    ins("sw $fp, 0($sp)");
    ins("move $fp, $sp");
    if (frame > 32767) { ins("li $t9, " + std::to_string(frame)); ins("subu $sp, $sp, $t9"); }
    else if (frame > 0) ins("addiu $sp, $sp, -" + std::to_string(frame));
}

void Gen::epilogue() {
    ins("move $sp, $fp");
    ins("lw $ra, 4($sp)");
    ins("lw $fp, 0($sp)");
    ins("addiu $sp, $sp, 8");
    ins("jr $ra");
}

void Gen::loadFromAddr(const std::string& result, int sz) {
    const Loc* r = find(result);
    if (!r) { error("unknown result '" + result + "'"); return; }
    if (r->aggr) {                                   // whole object: copy it into the result's slot
        addrOfName(result, "$t1");
        emitCopy(r->bytes);
        return;
    }
    ins(std::string(sz == 1 ? "lb" : (sz == 2 ? "lh" : "lw")) + " $t0, 0($t0)");
    storeValue("$t0", result);
}

// deref / idxload / addridx: compute the address in $t0, then load (or just keep it).
void Gen::genLoadMem(const TACInstruction& in, bool indexed, bool addrOnly) {
    if (indexed) {
        loadValue(in.arg1, "$t0");
        loadValue(in.arg2, "$t1");
        if (in.sz > 1) {
            if ((in.sz & (in.sz - 1)) == 0) {
                int sh = 0;
                while ((1 << sh) < in.sz) ++sh;
                ins("sll $t1, $t1, " + std::to_string(sh));
            } else {
                ins("li $t2, " + std::to_string(in.sz));
                ins("mul $t1, $t1, $t2");
            }
        }
        ins("addu $t0, $t0, $t1");
    } else {
        loadValue(in.arg1, "$t0");
        if (in.off != 0) ins("addiu $t0, $t0, " + std::to_string(in.off));
    }
    if (addrOnly) storeValue("$t0", in.result);
    else loadFromAddr(in.result, in.sz);
}

// store / idxstore: *(address) = value
void Gen::genStoreMem(const TACInstruction& in, bool indexed) {
    // the value is in arg1 (store) or in result (idxstore)
    const std::string& value = indexed ? in.result : in.arg1;
    if (indexed) {
        loadValue(in.arg1, "$t1");
        loadValue(in.arg2, "$t2");
        if (in.sz > 1) {
            if ((in.sz & (in.sz - 1)) == 0) {
                int sh = 0;
                while ((1 << sh) < in.sz) ++sh;
                ins("sll $t2, $t2, " + std::to_string(sh));
            } else {
                ins("li $t3, " + std::to_string(in.sz));
                ins("mul $t2, $t2, $t3");
            }
        }
        ins("addu $t1, $t1, $t2");
    } else {
        loadValue(in.result, "$t1");
        if (in.off != 0) ins("addiu $t1, $t1, " + std::to_string(in.off));
    }
    const Loc* v = find(value);
    if (v && v->aggr) {                              // whole object
        addrOfName(value, "$t0");
        emitCopy(v->bytes);
        return;
    }
    loadValue(value, "$t0");
    ins(std::string(in.sz == 1 ? "sb" : (in.sz == 2 ? "sh" : "sw")) + " $t0, 0($t1)");
}

void Gen::genBinary(const TACInstruction& in) {
    const std::string& op = in.op;
    if (in.arg2.empty()) {                           // unary: ~ !
        loadValue(in.arg1, "$t0");
        if (op == "~") ins("not $t0, $t0");
        else if (op == "!") ins("sltiu $t0, $t0, 1");
        else error("unsupported operator '" + op + "'");
        storeValue("$t0", in.result);
        return;
    }
    loadValue(in.arg1, "$t0");
    loadValue(in.arg2, "$t1");
    if (op == "+") ins("addu $t0, $t0, $t1");
    else if (op == "-") ins("subu $t0, $t0, $t1");
    else if (op == "*") ins("mul $t0, $t0, $t1");
    else if (op == "/") { ins("div $t0, $t1"); ins("mflo $t0"); }
    else if (op == "%") { ins("div $t0, $t1"); ins("mfhi $t0"); }
    else if (op == "&") ins("and $t0, $t0, $t1");
    else if (op == "|") ins("or $t0, $t0, $t1");
    else if (op == "^") ins("xor $t0, $t0, $t1");
    else if (op == "<<") ins("sllv $t0, $t0, $t1");
    else if (op == ">>") ins("srav $t0, $t0, $t1");
    else if (op == "<") ins("slt $t0, $t0, $t1");
    else if (op == ">") ins("slt $t0, $t1, $t0");
    else if (op == "<=") { ins("slt $t0, $t1, $t0"); ins("xori $t0, $t0, 1"); }
    else if (op == ">=") { ins("slt $t0, $t0, $t1"); ins("xori $t0, $t0, 1"); }
    else if (op == "==") { ins("subu $t0, $t0, $t1"); ins("sltiu $t0, $t0, 1"); }
    else if (op == "!=") { ins("subu $t0, $t0, $t1"); ins("sltu $t0, $zero, $t0"); }
    else error("unsupported operator '" + op + "'");
    storeValue("$t0", in.result);
}

void Gen::genCall(const TACInstruction& in) {
    const std::string& target = in.arg1;
    const Loc* res = in.result.empty() ? nullptr : find(in.result);
    bool aggrRet = res && res->aggr;
    if (aggrRet) {                                   // hidden argument: where the callee puts the result
        addrOfName(in.result, "$t0");
        pushReg("$t0");
    }
    if (userFuncs.count(target) || (!find(target) && externs.count(target))) {
        ins("jal " + funcLabel(target));
    } else if (find(target)) {                       // call through a function pointer
        loadValue(target, "$t0");
        ins("jalr $t0");
    } else {
        error("call to undefined function '" + target + "'");
    }
    if (pending > 0) ins("addiu $sp, $sp, " + std::to_string(pending));
    pending = 0;
    if (res && !aggrRet) storeValue("$v0", in.result);
}

void Gen::genReturn(const TACInstruction& in) {
    if (!in.arg1.empty()) {
        const Loc* v = find(in.arg1);
        if (hiddenRet && v && v->aggr) {
            ins("lw $t1, 8($fp)");
            addrOfName(in.arg1, "$t0");
            ins("move $v1, $t1");
            emitCopy(v->bytes);
            ins("move $v0, $v1");
        } else {
            loadValue(in.arg1, "$v0");
        }
    }
    epilogue();
}

void Gen::genInstr(const TACInstruction& in) {
    const std::string& op = in.op;
    if (op == "label") {
        label(in.arg1 == "user" ? userLabel(in.result) : pfx + in.result);
    } else if (op == "goto") {
        ins("j " + (in.arg1 == "user" ? userLabel(in.result) : pfx + in.result));
    } else if (op == "if" || op == "ifFalse") {
        loadValue(in.arg1, "$t0");
        ins(std::string(op == "if" ? "bnez" : "beqz") + " $t0, " + pfx + in.result);
    } else if (op == "return") {
        genReturn(in);
    } else if (op == "param") {
        const Loc* v = find(in.arg1);
        if (v && v->aggr) {                          // a struct is passed as a copy of the whole object
            long r = roundUp(v->bytes, 4);
            ins("addiu $sp, $sp, -" + std::to_string(r));
            addrOfName(in.arg1, "$t0");
            ins("move $t1, $sp");
            emitCopy(v->bytes);
            pending += r;
        } else {
            loadValue(in.arg1, "$t0");
            pushReg("$t0");
        }
    } else if (op == "call") {
        genCall(in);
    } else if (op == "=") {
        const Loc* r = find(in.result);
        if (r && r->aggr) {
            const Loc* s = find(in.arg1);
            if (!s || !s->aggr) { error("cannot assign '" + in.arg1 + "' to the object '" + in.result + "'"); return; }
            addrOfName(in.arg1, "$t0");
            addrOfName(in.result, "$t1");
            emitCopy(r->bytes);
        } else {
            loadValue(in.arg1, "$t0");
            storeValue("$t0", in.result);
        }
    } else if (op == "addr") {
        addrOfName(in.arg1, "$t0");
        storeValue("$t0", in.result);
    } else if (op == "deref") {
        genLoadMem(in, false, false);
    } else if (op == "idxload") {
        genLoadMem(in, true, false);
    } else if (op == "addridx") {
        genLoadMem(in, true, true);
    } else if (op == "store") {
        genStoreMem(in, false);
    } else if (op == "idxstore") {
        genStoreMem(in, true);
    } else if (op == "zero") {
        addrOfName(in.arg1, "$t0");
        long n = roundUp(in.sz, 4);
        if (n <= 64) {
            for (long i = 0; i < n; i += 4) ins("sw $zero, " + std::to_string(i) + "($t0)");
        } else {
            static int zeroLoops = 0;
            std::string l = pfx + "zero_" + std::to_string(zeroLoops++);
            ins("li $t7, " + std::to_string(n / 4));
            label(l);
            ins("sw $zero, 0($t0)");
            ins("addiu $t0, $t0, 4");
            ins("addiu $t7, $t7, -1");
            ins("bnez $t7, " + l);
        }
    } else if (op == "uminus") {
        loadValue(in.arg1, "$t0");
        ins("subu $t0, $zero, $t0");
        storeValue("$t0", in.result);
    } else {
        genBinary(in);
    }
}

// Generate one function whose instructions start at code[pos] (just after func_begin).
void Gen::genFunction(const FuncInfo& f, const std::vector<TACInstruction>& code, size_t& pos, bool isInit) {
    bool hidden = !isInit && f.ret.aggr && f.ret.ptr == 0 && !f.returnsRef;
    beginFunction(f, hidden);
    while (pos < code.size()) {
        const TACInstruction& in = code[pos++];
        if (in.op == "func_end") break;
        genInstr(in);
    }
    epilogue();
}

// Data segment: globals (zero-initialised; their initial values are set by __global_init) and strings.
void Gen::emitData() {
    data << "    .data\n";
    const std::vector<VarInfo>& gv = tac.globalVars();
    for (size_t i = 0; i < gv.size(); ++i) {
        const Loc& l = globals.find(gv[i].name)->second;
        data << "    .align 2\n" << l.label << ": .space " << roundUp(l.bytes, 4) << "\n";
    }
    for (size_t i = 0; i < strings.size(); ++i) {
        data << strings[i].first << ":";
        const std::vector<int>& b = strings[i].second;
        for (size_t k = 0; k <= b.size(); ++k) {
            if (k % 16 == 0) data << "\n    .byte ";
            else data << ", ";
            data << (k < b.size() ? b[k] : 0);
        }
        data << "\n";
    }
}

bool Gen::generate() {
    const std::vector<VarInfo>& gv = tac.globalVars();
    for (size_t i = 0; i < gv.size(); ++i) {
        Loc l = makeLoc(gv[i]);
        l.global = true;
        l.label = pfx + "G_" + sanitize(gv[i].name);
        globals[gv[i].name] = l;
    }
    const std::vector<FuncInfo>& funcs = tac.functions();
    std::map<std::string, const FuncInfo*> byName;
    for (size_t i = 0; i < funcs.size(); ++i) { userFuncs.insert(funcs[i].name); byName[funcs[i].name] = &funcs[i]; }

    text << "    .text\n";
    if (isProgram) {
        if (!userFuncs.count("main")) error("the program has no main function");
        // entry point: run the global initialisers (the runtime's too), then main; its result is the exit code
        text << "    .globl main\nmain:\n";
        ins("addiu $sp, $sp, -4");
        ins("sw $ra, 0($sp)");
        ins("jal __global_init");
        if (!externs.empty()) ins("jal R__global_init");
        ins("jal F_main");
        ins("move $a0, $v0");
        ins("li $v0, 17");
        ins("syscall");
    }

    size_t pos = 0;
    genFunction(tac.initInfo(), tac.initCode(), pos, true);

    const std::vector<TACInstruction>& code = tac.code();
    pos = 0;
    while (pos < code.size()) {
        const TACInstruction& in = code[pos++];
        if (in.op != "func_begin") continue;
        std::map<std::string, const FuncInfo*>::iterator it = byName.find(in.result);
        if (it == byName.end()) { error("no table for function '" + in.result + "'"); continue; }
        if (only && !only->count(in.result)) {          // pruned: skip the body
            while (pos < code.size() && code[pos].op != "func_end") ++pos;
            ++pos;
            continue;
        }
        genFunction(*it->second, code, pos, false);
    }
    emitData();
    return errors.empty();
}

// Functions reachable from 'roots' through the calls of the unit's own code.
std::set<std::string> reachable(const TACGenerator& unit, const std::set<std::string>& roots) {
    std::map<std::string, std::set<std::string> > calls;
    std::string cur;
    const std::vector<TACInstruction>& code = unit.code();
    for (size_t i = 0; i < code.size(); ++i) {
        if (code[i].op == "func_begin") cur = code[i].result;
        else if (code[i].op == "func_end") cur.clear();
        else if (!cur.empty()) {
            // a call, or a function name used as a value (its address is taken)
            if (code[i].op == "call") calls[cur].insert(code[i].arg1);
            else if (code[i].op == "=" || code[i].op == "param") calls[cur].insert(code[i].arg1);
        }
    }
    std::set<std::string> done;
    std::vector<std::string> work(roots.begin(), roots.end());
    while (!work.empty()) {
        std::string f = work.back();
        work.pop_back();
        if (!done.insert(f).second) continue;
        std::map<std::string, std::set<std::string> >::iterator it = calls.find(f);
        if (it == calls.end()) continue;
        for (std::set<std::string>::iterator t = it->second.begin(); t != it->second.end(); ++t)
            work.push_back(*t);
    }
    return done;
}

}  // namespace

// Hand-written syscall stubs the runtime's C code calls (see Runtime.cpp).
static const char* const kSysExterns[][2] = {
    {"__sys_putchar", "rt_sys_putchar"}, {"__sys_readline", "rt_sys_readline"},
    {"__sys_sbrk", "rt_sys_sbrk"},       {"__sys_exit", "rt_sys_exit"}};

bool generateMips(const TACGenerator& prog, const LayoutTable& progLayouts, const TACGenerator& rt,
                  const LayoutTable& rtLayouts, FILE* out, std::vector<std::string>& errors) {
    // the program refers to runtime functions through their "RF_" labels
    std::map<std::string, std::string> rtLabels;
    const std::vector<FuncInfo>& rtFuncs = rt.functions();
    for (size_t i = 0; i < rtFuncs.size(); ++i) rtLabels[rtFuncs[i].name] = "RF_" + sanitize(rtFuncs[i].name);

    Gen program(prog, progLayouts, errors, "", rtLabels, nullptr, true);
    if (!program.generate()) return false;

    // only the runtime functions the program uses (directly or through each other) are emitted
    std::set<std::string> needed = reachable(rt, program.externsUsed());
    std::map<std::string, std::string> sys;
    for (size_t i = 0; i < sizeof(kSysExterns) / sizeof(kSysExterns[0]); ++i) sys[kSysExterns[i][0]] = kSysExterns[i][1];
    Gen runtime(rt, rtLayouts, errors, "R", sys, &needed, false);
    if (!runtime.generate()) return false;

    fputs(program.dataText().c_str(), out);
    fputs(runtime.dataText().c_str(), out);
    fputs(program.codeText().c_str(), out);
    fputs(runtime.codeText().c_str(), out);
    fputs("\n", out);
    fputs(kSysStubsAsm, out);
    return true;
}
