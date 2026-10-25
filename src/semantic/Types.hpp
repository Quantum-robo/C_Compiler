#pragma once
#include <string>
#include <set>

// A deliberately small C/C++ type model for the semantic checks.
//
// An empty base means "unknown" (a library call, a function pointer, anything the
// checker could not work out). Every check that involves an unknown type is
// skipped, so the checker never rejects a program just because it could not
// determine a type.
struct Type {
    std::string base;      // "void","bool","char","short","int","long","float","double",
                           // or the name of a struct/class/union
    int ptr = 0;           // pointer levels
    int arr = 0;           // array dimensions not yet subscripted
    bool aggr = false;     // base names a struct/class/union
    bool cb = false;       // base type is const-qualified (const int x, const char *p)
};

inline Type makeType(const std::string& base, int ptr = 0, int arr = 0, bool aggr = false, bool cb = false) {
    Type t;
    t.base = base;
    t.ptr = ptr;
    t.arr = arr;
    t.aggr = aggr;
    t.cb = cb;
    return t;
}

inline bool known(const Type& t) { return !t.base.empty(); }

// Pointer levels once an array has decayed to a pointer.
inline int levels(const Type& t) { return t.ptr + t.arr; }

inline bool isBuiltinBase(const std::string& b) {
    return b == "void" || b == "bool" || b == "char" || b == "short" || b == "int" ||
           b == "long" || b == "float" || b == "double";
}

inline std::string normalizeBase(const std::string& b) {
    if (b == "signed" || b == "unsigned") return "int";
    return b;
}

inline bool isVoidT(const Type& t) { return known(t) && !t.aggr && t.base == "void" && levels(t) == 0; }
inline bool isAggrVal(const Type& t) { return known(t) && t.aggr && levels(t) == 0; }
inline bool isPointerT(const Type& t) { return known(t) && levels(t) > 0; }

inline bool isIntegral(const Type& t) {
    return known(t) && !t.aggr && levels(t) == 0 &&
           (t.base == "bool" || t.base == "char" || t.base == "short" || t.base == "int" || t.base == "long");
}

inline bool isFloating(const Type& t) {
    return known(t) && !t.aggr && levels(t) == 0 && (t.base == "float" || t.base == "double");
}

inline bool isArith(const Type& t) { return isIntegral(t) || isFloating(t); }
inline bool isScalar(const Type& t) { return isArith(t) || isPointerT(t); }

// Is the object itself const (const int x, but not const int *p)?
inline bool isConstObject(const Type& t) { return known(t) && t.cb && levels(t) == 0; }

inline std::string typeName(const Type& t) {
    if (!known(t)) return "<unknown>";
    std::string s = t.cb ? "const " : "";
    s += t.base;
    for (int i = 0; i < t.ptr; ++i) s += "*";
    for (int i = 0; i < t.arr; ++i) s += "[]";
    return s;
}

// Usual arithmetic conversions (simplified).
inline Type arithResult(const Type& a, const Type& b) {
    if (a.base == "double" || b.base == "double") return makeType("double");
    if (a.base == "float" || b.base == "float") return makeType("float");
    if (a.base == "long" || b.base == "long") return makeType("long");
    return makeType("int");
}

inline Type pointerResult(const Type& t) {
    return makeType(t.base, levels(t), 0, t.aggr, t.cb);
}

enum Compat { COMPAT_OK, COMPAT_WARN, COMPAT_ERR };

// Can a value of type 'src' be assigned to / initialise an object of type 'dst'?
// srcZero: the source is the literal 0 (a null pointer constant).
inline Compat assignCompat(const Type& dst, const Type& src, bool srcZero, std::string& why) {
    if (!known(dst) || !known(src)) return COMPAT_OK;

    if (isVoidT(src)) {
        why = "void value not ignored as it ought to be";
        return COMPAT_ERR;
    }
    if (isVoidT(dst)) {
        why = "cannot convert '" + typeName(src) + "' to 'void'";
        return COMPAT_ERR;
    }

    int dl = levels(dst), sl = levels(src);
    std::string msg = "cannot convert '" + typeName(src) + "' to '" + typeName(dst) + "'";

    if (isAggrVal(dst)) {
        if (isAggrVal(src) && src.base == dst.base) return COMPAT_OK;
        why = msg;
        return COMPAT_ERR;
    }
    if (isAggrVal(src)) {
        why = msg;
        return COMPAT_ERR;
    }

    if (dl == 0) {
        if (sl == 0) return COMPAT_OK;                  // arithmetic <- arithmetic
        if (dst.base == "bool") return COMPAT_OK;       // pointer -> bool
        why = msg + " (makes integer from pointer without a cast)";
        return COMPAT_ERR;
    }

    // dst is a pointer
    if (sl == 0) {
        if (srcZero) return COMPAT_OK;                  // null pointer constant
        why = msg + " (makes pointer from integer without a cast)";
        return COMPAT_ERR;
    }
    bool dstVoidPtr = (dst.base == "void" && !dst.aggr && dl == 1);
    bool srcVoidPtr = (src.base == "void" && !src.aggr && sl == 1);
    if (dstVoidPtr || srcVoidPtr) return COMPAT_OK;
    if (dl == sl && dst.base == src.base && dst.aggr == src.aggr) {
        if (src.cb && !dst.cb && src.ptr + src.arr == 1) {
            why = "assignment discards 'const' qualifier from pointer target type";
            return COMPAT_WARN;
        }
        return COMPAT_OK;
    }
    if (dst.aggr || src.aggr) {
        why = "incompatible pointer types: " + msg;
        return COMPAT_ERR;
    }
    why = "incompatible pointer types: " + msg;
    return COMPAT_WARN;
}

// Result type of a binary operator; returns false if the operand types are invalid.
inline bool binaryOpType(const std::string& op, const Type& a, const Type& b, Type& out) {
    bool cmp = op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=";
    bool logic = op == "&&" || op == "||";
    out = (cmp || logic) ? makeType("int") : Type();

    if (!known(a) || !known(b)) return true;
    if (isVoidT(a) || isVoidT(b)) return false;
    if (isAggrVal(a) || isAggrVal(b)) return false;

    bool pa = isPointerT(a), pb = isPointerT(b);
    if (logic) return true;
    if (cmp) {
        if (pa && pb) return true;
        if (pa) return isIntegral(b);
        if (pb) return isIntegral(a);
        return true;
    }
    if (op == "+") {
        if (pa && pb) return false;
        if (pa) { if (!isIntegral(b)) return false; out = pointerResult(a); return true; }
        if (pb) { if (!isIntegral(a)) return false; out = pointerResult(b); return true; }
        out = arithResult(a, b);
        return true;
    }
    if (op == "-") {
        if (pa && pb) {
            if (levels(a) != levels(b)) return false;
            out = makeType("int");
            return true;
        }
        if (pa) { if (!isIntegral(b)) return false; out = pointerResult(a); return true; }
        if (pb) return false;
        out = arithResult(a, b);
        return true;
    }
    if (op == "*" || op == "/") {
        if (pa || pb) return false;
        out = arithResult(a, b);
        return true;
    }
    // % << >> & | ^ need integers
    if (pa || pb) return false;
    if (!isIntegral(a) || !isIntegral(b)) return false;
    out = arithResult(a, b);
    return true;
}
