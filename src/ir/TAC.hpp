#pragma once
#include <string>
#include <vector>
#include <cstdio>

// Instruction kinds (op field) understood by TACInstruction::print:
//   "="          result = arg1
//   "store"      *result = arg1
//   "deref"      result = *arg1
//   "addr"       result = &arg1
//   "idxload"    result = arg1[arg2]
//   "idxstore"   arg1[arg2] = result
//   "addridx"    result = &arg1[arg2]
//   "new"        result = new arg1
//   "newarr"     result = new arg1[arg2]
//   "delete"     delete arg1
//   "deletearr"  delete[] arg1
//   "sizeof"     result = sizeof arg1
//   "param"      PushParam arg1
//   "call"       result = LCall arg1   (arg2 = number of pushed params)
//   anything else is a unary (arg2 empty) or binary operator.
struct TACInstruction {
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;

    void print(FILE* out = stdout) const;
};

class TACGenerator {
private:
    std::vector<TACInstruction> instructions;
    std::vector<TACInstruction> globalInstructions;  // global / static initialisers
    int temp_count = 1;   // Start at t1
    int label_count = 1;  // Start at L1
    bool inFunction = false;
    bool forceGlobal = false;

public:
    std::string newTemp();
    std::string newLabel();

    // Instructions emitted outside a function body (or while forceGlobal is
    // set, e.g. for static locals) go to the _global_init section.
    void emit(const std::string& op, const std::string& arg1,
              const std::string& arg2, const std::string& result);

    void setForceGlobal(bool v) { forceGlobal = v; }

    void printTAC(FILE* out = stdout) const;
    bool saveTAC(const std::string& filename) const;
};

extern TACGenerator tacGen;
