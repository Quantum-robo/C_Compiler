#include "TAC.hpp"
#include <cstdio>

TACGenerator tacGen;

std::string TACGenerator::newTemp() {
    return "t" + std::to_string(temp_count++);
}

std::string TACGenerator::newLabel() {
    return "L" + std::to_string(label_count++);
}

void TACGenerator::emit(const std::string& op, const std::string& arg1,
                        const std::string& arg2, const std::string& result) {
    bool global = forceGlobal || (!inFunction && op != "func_begin");
    if (op == "func_begin") inFunction = true;
    (global ? globalInstructions : instructions).push_back({op, arg1, arg2, result});
    if (op == "func_end") inFunction = false;
}

void TACInstruction::print(FILE* out) const {
    if (op == "=") {
        fprintf(out, "    %s = %s\n", result.c_str(), arg1.c_str());
    } else if (op == "label") {
        fprintf(out, "%s:\n", result.c_str());
    } else if (op == "goto") {
        fprintf(out, "    Goto %s\n", result.c_str());
    } else if (op == "ifFalse") {
        fprintf(out, "    IfFalse %s Goto %s\n", arg1.c_str(), result.c_str());
    } else if (op == "if") {
        fprintf(out, "    If %s Goto %s\n", arg1.c_str(), result.c_str());
    } else if (op == "return") {
        if (arg1.empty()) {
            fprintf(out, "    Return\n");
        } else {
            fprintf(out, "    Return %s\n", arg1.c_str());
        }
    } else if (op == "func_begin") {
        fprintf(out, "\n%s:\n", result.c_str());
        fprintf(out, "    BeginFunc\n");
    } else if (op == "func_end") {
        fprintf(out, "    EndFunc\n");
    } else if (op == "param") {
        fprintf(out, "    PushParam %s\n", arg1.c_str());
    } else if (op == "call") {
        if (result.empty()) {
            fprintf(out, "    LCall %s\n", arg1.c_str());
        } else {
            fprintf(out, "    %s = LCall %s\n", result.c_str(), arg1.c_str());
        }
        if (!arg2.empty() && arg2 != "0") {
            fprintf(out, "    PopParams %s\n", arg2.c_str());
        }
    } else if (op == "uminus") {
        fprintf(out, "    %s = -%s\n", result.c_str(), arg1.c_str());
    } else if (op == "addr") {
        fprintf(out, "    %s = &%s\n", result.c_str(), arg1.c_str());
    } else if (op == "deref") {
        fprintf(out, "    %s = *%s\n", result.c_str(), arg1.c_str());
    } else if (op == "store") {
        fprintf(out, "    *%s = %s\n", result.c_str(), arg1.c_str());
    } else if (op == "idxload") {
        fprintf(out, "    %s = %s[%s]\n", result.c_str(), arg1.c_str(), arg2.c_str());
    } else if (op == "idxstore") {
        fprintf(out, "    %s[%s] = %s\n", arg1.c_str(), arg2.c_str(), result.c_str());
    } else if (op == "addridx") {
        fprintf(out, "    %s = &%s[%s]\n", result.c_str(), arg1.c_str(), arg2.c_str());
    } else if (op == "new") {
        fprintf(out, "    %s = new %s\n", result.c_str(), arg1.c_str());
    } else if (op == "newarr") {
        fprintf(out, "    %s = new %s[%s]\n", result.c_str(), arg1.c_str(), arg2.c_str());
    } else if (op == "delete") {
        fprintf(out, "    delete %s\n", arg1.c_str());
    } else if (op == "deletearr") {
        fprintf(out, "    delete[] %s\n", arg1.c_str());
    } else if (op == "sizeof") {
        fprintf(out, "    %s = sizeof %s\n", result.c_str(), arg1.c_str());
    } else if (arg2.empty()) {
        fprintf(out, "    %s = %s%s\n", result.c_str(), op.c_str(), arg1.c_str());
    } else {
        fprintf(out, "    %s = %s %s %s\n", result.c_str(), arg1.c_str(), op.c_str(), arg2.c_str());
    }
}

void TACGenerator::printTAC(FILE* out) const {
    if (!globalInstructions.empty()) {
        fprintf(out, "\n_global_init:\n");
        for (const auto& inst : globalInstructions) {
            inst.print(out);
        }
    }
    for (const auto& inst : instructions) {
        inst.print(out);
    }
}

bool TACGenerator::saveTAC(const std::string& filename) const {
    FILE* file = fopen(filename.c_str(), "w");
    if (!file) {
        perror(("Error opening output file: " + filename).c_str());
        return false;
    }
    printTAC(file);
    fclose(file);
    return true;
}
