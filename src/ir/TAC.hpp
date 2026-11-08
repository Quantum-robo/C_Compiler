#pragma once
#include <string>
#include <vector>
#include <map>
#include <cstdio>
#include "../semantic/Types.hpp"
#include "../semantic/Layout.hpp"

// Three-address code. Operands are plain strings, always one of:
//   an integer literal (decimal, hex or 'c' character literal), a "string literal",
//   a temporary (t1, t2, ...) or the IR name of a variable (unique inside its function).
// Everything else (field access, indexing, pointer scaling, sizeof, new/delete) is lowered to the
// instructions below, so every operand has a known size (see FuncInfo / VarInfo).
//
// Instruction kinds (op field) understood by TACInstruction::print:
//   "="          result = arg1                       (scalar or whole-object copy, sized by the variable)
//   "store"      *(result + off) = arg1              (sz bytes)
//   "deref"      result = *(arg1 + off)              (sz bytes)
//   "addr"       result = &arg1                      (address of a variable; an array name is its own address)
//   "idxload"    result = arg1[arg2]                 (arg2 = element index, sz = element size in bytes)
//   "idxstore"   arg1[arg2] = result                 (same)
//   "addridx"    result = &arg1[arg2]                (same)
//   "zero"       zero arg1                           (clear the whole variable; sz = its size)
//   "param"      PushParam arg1
//   "call"       result = LCall arg1   (arg2 = number of pushed params)
//   "label", "goto", "if", "ifFalse", "return", "func_begin", "func_end"
//   anything else is a unary (arg2 empty) or binary operator on word values ("+" is a plain integer add:
//   pointer arithmetic has already been scaled by the element size).
// The label and goto of a source-level label carry "user" in arg1, so the code generator can keep their
// names apart from the compiler's own L1, L2, ... labels.
struct TACInstruction {
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;
    int sz = 0;     // bytes accessed / stepped (memory instructions), 0 = not applicable
    int off = 0;    // constant byte offset added to the pointer (deref / store)

    void print(FILE* out = stdout) const;
};

// A variable known to the code generator.
struct VarInfo {
    std::string name;            // IR name, unique inside its function (statics: func.name)
    Type ty;                     // declared type (arr = number of array dimensions)
    std::vector<long> dims;      // array dimensions, 0 = unsized (parameters)
    bool isRef = false;          // reference: holds an address, every use is implicitly dereferenced
    bool isParam = false;
    bool isStatic = false;
    bool isGlobal = false;       // lives in the data segment (globals and static locals)
    bool isExtern = false;
};

struct FuncInfo {
    std::string name;                                 // IR label: "f" or "Class::method"
    std::vector<VarInfo> params;                      // in declaration order (methods: 'this' first)
    std::vector<VarInfo> locals;
    std::vector<std::pair<std::string, Type> > temps; // every temporary, in creation order
    Type ret;
    bool returnsRef = false;
    bool isMethod = false;
};

class TACGenerator {
private:
    std::vector<TACInstruction> instructions;
    std::vector<TACInstruction> globalInstructions;  // global / static initialisers
    std::vector<FuncInfo> funcs;
    FuncInfo globalInit;                             // temporaries used by the global initialisers
    std::vector<TACInstruction> finiInstructions;    // destructors of global objects, run after main
    FuncInfo globalFini;
    bool finiMode = false;
    std::vector<VarInfo> globals;
    int temp_count = 1;   // Start at t1
    int label_count = 1;  // Start at L1
    bool inFunction = false;
    bool forceGlobal = false;

public:
    void reset();

    // Every temporary has a type; an unknown type is recorded as int.
    std::string newTemp(const Type& ty);
    void setTempType(const std::string& temp, const Type& ty);
    std::string newLabel();

    // Instructions emitted outside a function body (or while forceGlobal is
    // set, e.g. for static locals) go to the _global_init section.
    void emit(const std::string& op, const std::string& arg1,
              const std::string& arg2, const std::string& result, int sz = 0, int off = 0);

    void setForceGlobal(bool v) { forceGlobal = v; }
    bool inGlobalContext() const { return forceGlobal || finiMode || !inFunction; }
    // While set, instructions and temporaries go to the _global_fini section.
    void setFiniMode(bool v) { finiMode = v; }

    void beginFunction(const FuncInfo& f);
    void declareVar(const VarInfo& v);     // local of the current function, or a global / static

    const std::vector<FuncInfo>& functions() const { return funcs; }
    const FuncInfo& initInfo() const { return globalInit; }
    const FuncInfo& finiInfo() const { return globalFini; }
    const std::vector<TACInstruction>& finiCode() const { return finiInstructions; }
    const std::vector<VarInfo>& globalVars() const { return globals; }
    const std::vector<TACInstruction>& code() const { return instructions; }
    const std::vector<TACInstruction>& initCode() const { return globalInstructions; }

    void printTAC(FILE* out = stdout) const;
    void printTables(FILE* out, const LayoutTable& layouts) const;
    bool saveTAC(const std::string& filename, bool withTables, const LayoutTable& layouts) const;
};

extern TACGenerator tacGen;
