#include "Optimizer.hpp"
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace {

typedef std::vector<TACInstruction> Code;

/* ------------------------------------------------------------------
   Integer literals and compile-time arithmetic (32-bit, like a MIPS word)
   ------------------------------------------------------------------ */

long long wrap32(long long v) { return (long long)(int32_t)(uint32_t)(v & 0xffffffffLL); }

std::string num(long long v) { return std::to_string(wrap32(v)); }

// Value of the escape sequence that starts after the backslash at s[i]; advances i past it.
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
            return (unsigned char)c;
    }
}

// An integer operand: decimal or hex (optionally negative) or a character literal. Floating literals,
// strings and names are not.
bool intLiteral(const std::string& op, long long& v) {
    if (op.empty()) return false;
    if (op[0] == '\'') {
        if (op.size() < 3) return false;
        size_t i = 1;
        long long x;
        if (op[i] == '\\') { ++i; x = decodeEscape(op, i); }
        else x = (unsigned char)op[i++];
        if (i >= op.size() || op[i] != '\'') return false;
        v = x > 127 ? x - 256 : x;                   // char is signed
        return true;
    }
    size_t i = (op[0] == '-') ? 1 : 0;
    if (i >= op.size() || !isdigit((unsigned char)op[i])) return false;
    for (size_t k = i; k < op.size(); ++k)
        if (!(isxdigit((unsigned char)op[k]) || op[k] == 'x' || op[k] == 'X')) return false;   // 1.5, 2e3, 10L
    char* end = nullptr;
    v = strtoll(op.c_str(), &end, 0);
    if (!end || *end != '\0') return false;
    v = wrap32(v);
    return true;
}

// a op b on 32-bit signed words, as the generated MIPS code would compute it. False if it cannot be folded
// (division by zero, or the one overflowing division, are left for run time).
bool foldBinary(const std::string& op, long long a, long long b, long long& r) {
    if (op == "+") r = a + b;
    else if (op == "-") r = a - b;
    else if (op == "*") r = a * b;
    else if (op == "/" || op == "%") {
        if (b == 0 || (a == INT32_MIN && b == -1)) return false;
        r = (op == "/") ? a / b : a % b;             // both truncate toward zero, like MIPS div
    }
    else if (op == "<<") r = (long long)(uint32_t)((uint32_t)a << (b & 31));   // sllv uses the low 5 bits
    else if (op == ">>") r = (int32_t)a >> (b & 31);                          // srav: arithmetic shift
    else if (op == "&") r = a & b;
    else if (op == "|") r = a | b;
    else if (op == "^") r = a ^ b;
    else if (op == "==") r = a == b;
    else if (op == "!=") r = a != b;
    else if (op == "<") r = a < b;
    else if (op == "<=") r = a <= b;
    else if (op == ">") r = a > b;
    else if (op == ">=") r = a >= b;
    else return false;
    r = wrap32(r);
    return true;
}

bool foldUnary(const std::string& op, long long a, long long& r) {
    if (op == "uminus") r = -a;
    else if (op == "~") r = ~a;
    else if (op == "!") r = (a == 0);
    else return false;
    r = wrap32(r);
    return true;
}

bool commutative(const std::string& op) {
    return op == "+" || op == "*" || op == "&" || op == "|" || op == "^" || op == "==" || op == "!=";
}

/* ------------------------------------------------------------------
   Instruction kinds
   ------------------------------------------------------------------ */

// Every op that is not one of these is an arithmetic, logical or comparison operator (binary, or unary
// when arg2 is empty), computed from its operands only.
bool isArith(const std::string& op) {
    static const std::set<std::string> other = {
        "=", "label", "goto", "if", "ifFalse", "return", "func_begin", "func_end", "param", "call",
        "addr", "deref", "store", "idxload", "idxstore", "addridx", "zero", ""};
    return !other.count(op);
}

// Does the instruction write its 'result' field (a variable or temporary)?
bool definesResult(const TACInstruction& in) {
    const std::string& op = in.op;
    if (op == "call") return !in.result.empty();
    return op == "=" || op == "addr" || op == "addridx" || op == "deref" || op == "idxload" || isArith(op);
}

// The operand fields an instruction reads. 'subst' says whether a use may be replaced by a constant or by
// another variable: the callee of a call, the variable of '&x' and the array of 'a[i]' must keep their names.
struct Use { std::string* field; bool subst; };

std::vector<Use> usesOf(TACInstruction& in) {
    std::vector<Use> u;
    const std::string& op = in.op;
    if (op == "=" || op == "deref" || op == "param" || op == "return" || op == "if" || op == "ifFalse") {
        u.push_back({&in.arg1, true});
    } else if (op == "idxload" || op == "addridx") {
        u.push_back({&in.arg1, false});
        u.push_back({&in.arg2, true});
    } else if (op == "store") {
        u.push_back({&in.arg1, true});
        u.push_back({&in.result, true});
    } else if (op == "idxstore") {
        u.push_back({&in.arg1, false});
        u.push_back({&in.arg2, true});
        u.push_back({&in.result, true});
    } else if (op == "call" || op == "addr" || op == "zero") {
        u.push_back({&in.arg1, false});
    } else if (isArith(op)) {
        u.push_back({&in.arg1, true});
        if (!in.arg2.empty()) u.push_back({&in.arg2, true});
    }
    for (size_t i = u.size(); i-- > 0;)
        if (u[i].field->empty()) u.erase(u.begin() + i);
    return u;
}

/* ------------------------------------------------------------------
   What the optimiser knows about the names of one function
   ------------------------------------------------------------------ */

struct FuncCtx {
    std::map<std::string, const VarInfo*> vars;   // parameters and locals
    std::map<std::string, Type> temps;
    std::set<std::string> globalArrays;
    std::set<std::string> addrTaken;              // '&x' or 'zero x' somewhere in the function

    // A one-word integer or pointer value (not char / short / bool, float / double, struct or array).
    static bool wordInt(const Type& t) {
        if (t.arr > 0 || (t.aggr && t.ptr == 0)) return false;
        if (t.ptr > 0) return true;
        return t.base == "int" || t.base == "long" || t.base == "signed" || t.base == "unsigned";
    }

    // Can a fact about this name be kept? Only for values nothing else can change: see Optimizer.hpp.
    bool tracked(const std::string& n) const {
        if (n.empty() || addrTaken.count(n)) return false;
        std::map<std::string, const VarInfo*>::const_iterator v = vars.find(n);
        if (v != vars.end()) {
            const VarInfo& vi = *v->second;
            return !vi.isRef && !vi.isGlobal && !vi.isStatic && vi.dims.empty() && wordInt(vi.ty);
        }
        std::map<std::string, Type>::const_iterator t = temps.find(n);
        return t != temps.end() && wordInt(t->second);
    }

    bool isTemp(const std::string& n) const { return temps.count(n) > 0 && !vars.count(n); }

    // An array whose address never changes (a local or global array, not an array parameter).
    bool fixedArray(const std::string& n) const {
        std::map<std::string, const VarInfo*>::const_iterator v = vars.find(n);
        if (v != vars.end()) return !v->second->dims.empty() && !v->second->isParam && !v->second->isRef;
        return globalArrays.count(n) > 0;
    }
};

/* ------------------------------------------------------------------
   Propagation, folding and common subexpressions inside each basic block
   ------------------------------------------------------------------ */

struct Avail {                                    // 'holder' contains the value of  a op b
    std::string op, a, b;
    int sz;
    std::string holder;
};

struct Facts {
    std::map<std::string, std::string> konst;     // x -> the constant it holds
    std::map<std::string, std::string> copy;      // x -> the variable it is a copy of
    std::vector<Avail> avail;

    void clear() { konst.clear(); copy.clear(); avail.clear(); }

    // x gets a new value: forget everything that depended on its old value.
    void kill(const std::string& x) {
        konst.erase(x);
        copy.erase(x);
        for (std::map<std::string, std::string>::iterator it = copy.begin(); it != copy.end();) {
            if (it->second == x) copy.erase(it++);
            else ++it;
        }
        for (size_t i = avail.size(); i-- > 0;)
            if (avail[i].holder == x || avail[i].a == x || avail[i].b == x) avail.erase(avail.begin() + i);
    }
};

class BlockOptimizer {
public:
    BlockOptimizer(const FuncCtx& c, OptStats& s) : ctx(c), stats(s) {}

    // One forward pass over the function. At every label (and after every jump) a new basic block starts
    // and all facts are forgotten. Inside a block: operands are replaced by known constants and copies,
    // operations on constants are folded, a repeated computation becomes a copy of the earlier result, and
    // a conditional jump on a constant becomes a plain jump (or disappears). Returns true if anything changed.
    bool run(Code& code) {
        changed = false;
        facts.clear();
        for (size_t i = 0; i < code.size(); ++i) {
            TACInstruction& in = code[i];
            const std::string& op = in.op;
            if (op == "label" || op == "func_begin" || op == "func_end" || op.empty()) {
                facts.clear();
                continue;
            }
            substituteUses(in);
            if (op == "goto" || op == "return") {
                facts.clear();
            } else if (op == "if" || op == "ifFalse") {
                resolveBranch(in);
                facts.clear();
            } else if (op == "=") {
                copyInstr(in);
            } else if (isArith(op)) {
                arithInstr(in);
            } else if (op == "addr") {
                if (ctx.tracked(in.result)) reuseOrRecord(in, "addr", in.arg1, "", 0);
                else facts.kill(in.result);
            } else if (op == "addridx") {
                if (ctx.tracked(in.result) && (ctx.fixedArray(in.arg1) || ctx.tracked(in.arg1)) && simple(in.arg2))
                    reuseOrRecord(in, "addridx", in.arg1, in.arg2, in.sz);
                else facts.kill(in.result);
            } else if (op == "deref" || op == "idxload" || op == "call") {
                if (!in.result.empty()) facts.kill(in.result);
            } else if (op == "zero") {
                facts.kill(in.arg1);
            } else if (op != "store" && op != "idxstore" && op != "param") {
                facts.clear();                            // unknown instruction: assume nothing
            }
        }
        return changed;
    }

private:
    const FuncCtx& ctx;
    OptStats& stats;
    Facts facts;
    bool changed = false;

    bool simple(const std::string& s) const {          // a constant, or a tracked variable
        long long v;
        return intLiteral(s, v) || ctx.tracked(s);
    }

    void substituteUses(TACInstruction& in) {
        std::vector<Use> uses = usesOf(in);
        for (size_t k = 0; k < uses.size(); ++k) {
            if (!uses[k].subst || !ctx.tracked(*uses[k].field)) continue;
            std::string& f = *uses[k].field;
            std::map<std::string, std::string>::iterator c = facts.konst.find(f);
            if (c != facts.konst.end()) { f = c->second; ++stats.constProp; changed = true; continue; }
            std::map<std::string, std::string>::iterator p = facts.copy.find(f);
            if (p != facts.copy.end()) { f = p->second; ++stats.copyProp; changed = true; }
        }
    }

    void resolveBranch(TACInstruction& in) {
        long long v;
        if (!intLiteral(in.arg1, v)) return;
        bool taken = (in.op == "if") ? v != 0 : v == 0;
        if (taken) { in.op = "goto"; in.arg1 = ""; }
        else in.op = "";                                  // never taken: removed by the clean-up
        ++stats.branches;
        changed = true;
    }

    // x = a : remember that x holds a constant, or is a copy of another tracked variable.
    void copyInstr(TACInstruction& in) {
        const std::string x = in.result;
        if (!ctx.tracked(x)) return;
        facts.kill(x);
        long long v;
        if (intLiteral(in.arg1, v)) facts.konst[x] = num(v);
        else if (ctx.tracked(in.arg1) && in.arg1 != x) facts.copy[x] = in.arg1;
    }

    void arithInstr(TACInstruction& in) {
        const std::string x = in.result;
        if (!ctx.tracked(x)) return;
        long long a, b, r;
        bool unary = in.arg2.empty();
        if (intLiteral(in.arg1, a) &&
            (unary ? foldUnary(in.op, a, r) : (intLiteral(in.arg2, b) && foldBinary(in.op, a, b, r)))) {
            in.op = "=";
            in.arg1 = num(r);
            in.arg2 = "";
            ++stats.folded;
            changed = true;
            facts.kill(x);
            facts.konst[x] = in.arg1;
            return;
        }
        if (simple(in.arg1) && (unary || simple(in.arg2))) {
            std::string l = in.arg1, rr = in.arg2;
            if (commutative(in.op) && rr < l) std::swap(l, rr);
            reuseOrRecord(in, in.op, l, rr, 0);
        } else {
            facts.kill(x);
        }
    }

    // Common subexpression: if 'holder' already contains  a op b  in this block, x = holder; otherwise
    // remember that x now contains it.
    void reuseOrRecord(TACInstruction& in, const std::string& op, const std::string& a, const std::string& b, int sz) {
        const std::string x = in.result;
        for (size_t k = 0; k < facts.avail.size(); ++k) {
            const Avail& e = facts.avail[k];
            if (e.op != op || e.a != a || e.b != b || e.sz != sz) continue;
            std::string h = e.holder;
            in.op = "=";
            in.arg1 = h;
            in.arg2 = "";
            in.sz = 0;
            ++stats.cse;
            changed = true;
            facts.kill(x);
            if (h != x) facts.copy[x] = h;
            return;
        }
        facts.kill(x);
        if (x != a && x != b) facts.avail.push_back(Avail{op, a, b, sz, x});
    }
};

/* ------------------------------------------------------------------
   Dead code elimination
   ------------------------------------------------------------------ */

bool isJump(const std::string& op) { return op == "goto" || op == "if" || op == "ifFalse"; }

// The label a jump goes to, with "user:" in front of a source-level label (they have their own names).
std::string jumpKey(const TACInstruction& in) { return (in.arg1 == "user" ? "user:" : "") + in.result; }
std::string labelKey(const TACInstruction& in) { return (in.arg1 == "user" ? "user:" : "") + in.result; }

void removeMarked(Code& code) {
    Code out;
    for (size_t i = 0; i < code.size(); ++i)
        if (!code[i].op.empty()) out.push_back(code[i]);
    code.swap(out);
}

// Removes, until nothing changes: code after a goto / return that no label makes reachable; a jump to the
// very next instruction; compiler labels that no jump uses (so neighbouring blocks merge); 'x = x'; and every
// instruction without side effects whose result (a temporary or a tracked local) is never read. A call
// whose result is never read keeps the call but drops the result. Returns true if anything changed.
bool eliminateDeadCode(Code& code, const FuncCtx& ctx, OptStats& stats) {
    bool any = false;
    for (bool again = true; again;) {
        again = false;
        int removed = 0;
        bool dead = false;                            // after goto / return, before the next label
        for (size_t i = 0; i < code.size(); ++i) {
            TACInstruction& in = code[i];
            if (in.op == "label" || in.op == "func_end") dead = false;
            else if (dead && !in.op.empty()) { in.op = ""; ++removed; continue; }
            if (in.op == "goto" || in.op == "return") dead = true;
        }
        removeMarked(code);
        for (size_t i = 0; i + 1 < code.size(); ++i)    // a jump to the next instruction
            if (isJump(code[i].op) && code[i + 1].op == "label" && jumpKey(code[i]) == labelKey(code[i + 1])) {
                code[i].op = "";
                ++removed;
            }
        removeMarked(code);
        std::set<std::string> targets;
        std::map<std::string, int> uses;
        for (size_t i = 0; i < code.size(); ++i) {
            if (isJump(code[i].op)) targets.insert(jumpKey(code[i]));
            std::vector<Use> u = usesOf(code[i]);
            for (size_t k = 0; k < u.size(); ++k) ++uses[*u[k].field];
        }
        for (size_t i = 0; i < code.size(); ++i) {
            TACInstruction& in = code[i];
            if (in.op == "label" && in.arg1 != "user" && !targets.count(labelKey(in))) { in.op = ""; ++removed; continue; }
            if (!definesResult(in)) continue;
            const std::string& x = in.result;
            bool local = ctx.isTemp(x) || ctx.tracked(x);
            if (in.op == "=" && in.arg1 == x && ctx.tracked(x)) { in.op = ""; ++removed; continue; }
            if (!local || uses.count(x)) continue;
            if (in.op == "call") { if (ctx.tracked(x)) { in.result = ""; again = true; any = true; } continue; }
            in.op = "";
            ++removed;
        }
        removeMarked(code);
        if (removed > 0) { stats.deadCode += removed; again = true; any = true; }
    }
    return any;
}

// Temporaries that no instruction mentions any more need no stack slot.
void dropUnusedTemps(const Code& code, FuncInfo& f, OptStats& stats) {
    std::set<std::string> seen;
    for (size_t i = 0; i < code.size(); ++i) {
        seen.insert(code[i].arg1);
        seen.insert(code[i].arg2);
        seen.insert(code[i].result);
    }
    std::vector<std::pair<std::string, Type> > kept;
    for (size_t i = 0; i < f.temps.size(); ++i) {
        if (seen.count(f.temps[i].first)) kept.push_back(f.temps[i]);
        else ++stats.deadTemps;
    }
    f.temps.swap(kept);
}

void buildContext(const Code& code, const FuncInfo& f, const TACGenerator& tac, FuncCtx& ctx) {
    for (size_t i = 0; i < f.params.size(); ++i) ctx.vars[f.params[i].name] = &f.params[i];
    for (size_t i = 0; i < f.locals.size(); ++i) ctx.vars[f.locals[i].name] = &f.locals[i];
    for (size_t i = 0; i < f.temps.size(); ++i) ctx.temps[f.temps[i].first] = f.temps[i].second;
    const std::vector<VarInfo>& g = tac.globalVars();
    for (size_t i = 0; i < g.size(); ++i)
        if (!g[i].dims.empty()) ctx.globalArrays.insert(g[i].name);
    for (size_t i = 0; i < code.size(); ++i)
        if (code[i].op == "addr" || code[i].op == "zero") ctx.addrTaken.insert(code[i].arg1);
}

void optimizeFunction(Code& code, FuncInfo& f, const TACGenerator& tac, OptStats& stats) {
    FuncCtx ctx;
    buildContext(code, f, tac, ctx);
    BlockOptimizer blocks(ctx, stats);
    for (int round = 0; round < 10; ++round) {
        bool changed = blocks.run(code);
        removeMarked(code);
        changed = eliminateDeadCode(code, ctx, stats) || changed;
        if (!changed) break;
    }
    dropUnusedTemps(code, f, stats);
}

}  // namespace

// Split the code into functions (func_begin ... func_end) and optimise each one with its own tables.
OptStats optimizeTAC(TACGenerator& tac) {
    OptStats stats;
    Code& code = tac.mutableCode();
    std::vector<FuncInfo>& funcs = tac.mutableFunctions();
    std::map<std::string, FuncInfo*> byName;
    for (size_t i = 0; i < funcs.size(); ++i) byName[funcs[i].name] = &funcs[i];
    Code out;
    size_t i = 0;
    while (i < code.size()) {
        if (code[i].op != "func_begin") { out.push_back(code[i++]); continue; }
        size_t j = i;
        while (j < code.size() && code[j].op != "func_end") ++j;
        Code body(code.begin() + i, code.begin() + (j < code.size() ? j + 1 : j));
        std::map<std::string, FuncInfo*>::iterator it = byName.find(code[i].result);
        if (it != byName.end()) optimizeFunction(body, *it->second, tac, stats);
        out.insert(out.end(), body.begin(), body.end());
        i = j < code.size() ? j + 1 : j;
    }
    code.swap(out);
    return stats;
}
