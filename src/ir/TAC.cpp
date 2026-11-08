#include "TAC.hpp"
#include <cstdio>

TACGenerator tacGen;

void TACGenerator::reset() {
    instructions.clear();
    globalInstructions.clear();
    funcs.clear();
    globalInit = FuncInfo();
    globalInit.name = "_global_init";
    finiInstructions.clear();
    globalFini = FuncInfo();
    globalFini.name = "_global_fini";
    finiMode = false;
    globals.clear();
    temp_count = 1;
    label_count = 1;
    inFunction = false;
    forceGlobal = false;
}

std::string TACGenerator::newTemp(const Type& ty) {
    std::string name = "t" + std::to_string(temp_count++);
    Type t = known(ty) ? ty : makeType("int");
    (finiMode ? globalFini.temps : inGlobalContext() ? globalInit.temps : funcs.back().temps).push_back(std::make_pair(name, t));
    return name;
}

void TACGenerator::setTempType(const std::string& temp, const Type& ty) {
    if (!known(ty)) return;
    std::vector<std::pair<std::string, Type> >& v = finiMode ? globalFini.temps : inGlobalContext() ? globalInit.temps : funcs.back().temps;
    for (size_t i = v.size(); i-- > 0;)
        if (v[i].first == temp) { v[i].second = ty; return; }
}

std::string TACGenerator::newLabel() {
    return "L" + std::to_string(label_count++);
}

void TACGenerator::emit(const std::string& op, const std::string& arg1, const std::string& arg2,
                        const std::string& result, int sz, int off) {
    bool global = forceGlobal || (!inFunction && op != "func_begin");
    if (op == "func_begin") inFunction = true;
    TACInstruction in;
    in.op = op; in.arg1 = arg1; in.arg2 = arg2; in.result = result; in.sz = sz; in.off = off;
    (finiMode ? finiInstructions : global ? globalInstructions : instructions).push_back(in);
    if (op == "func_end") inFunction = false;
}

void TACGenerator::beginFunction(const FuncInfo& f) {
    funcs.push_back(f);
}

void TACGenerator::declareVar(const VarInfo& v) {
    if (v.isGlobal) {
        for (size_t i = 0; i < globals.size(); ++i) {
            if (globals[i].name != v.name) continue;
            if (globals[i].isExtern && !v.isExtern) globals[i] = v;   // the definition replaces an extern
            return;
        }
        globals.push_back(v);
    } else if (!funcs.empty()) {
        funcs.back().locals.push_back(v);
    }
}

// ": N" marks an access whose size is not a word.
static void sizeTag(FILE* out, int sz) {
    if (sz != 0 && sz != 4) fprintf(out, " :%d", sz);
}

static std::string ptrExpr(const std::string& p, int off) {
    if (off == 0) return p;
    return "(" + p + (off < 0 ? " - " : " + ") + std::to_string(off < 0 ? -off : off) + ")";
}

// Print one instruction in the readable three-address form: copies and operators as "x = a op b", memory
// accesses with their size tag (":1" for a byte), jumps, calls with their PopParams, function begin and end.
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
        fprintf(out, "    %s = *%s", result.c_str(), ptrExpr(arg1, off).c_str());
        sizeTag(out, sz);
        fprintf(out, "\n");
    } else if (op == "store") {
        fprintf(out, "    *%s = %s", ptrExpr(result, off).c_str(), arg1.c_str());
        sizeTag(out, sz);
        fprintf(out, "\n");
    } else if (op == "idxload") {
        fprintf(out, "    %s = %s[%s]", result.c_str(), arg1.c_str(), arg2.c_str());
        sizeTag(out, sz);
        fprintf(out, "\n");
    } else if (op == "idxstore") {
        fprintf(out, "    %s[%s] = %s", arg1.c_str(), arg2.c_str(), result.c_str());
        sizeTag(out, sz);
        fprintf(out, "\n");
    } else if (op == "addridx") {
        fprintf(out, "    %s = &%s[%s]", result.c_str(), arg1.c_str(), arg2.c_str());
        sizeTag(out, sz);
        fprintf(out, "\n");
    } else if (op == "zero") {
        fprintf(out, "    Zero %s :%d\n", arg1.c_str(), sz);
    } else if (arg2.empty()) {
        fprintf(out, "    %s = %s%s\n", result.c_str(), op.c_str(), arg1.c_str());
    } else {
        fprintf(out, "    %s = %s %s %s\n", result.c_str(), arg1.c_str(), op.c_str(), arg2.c_str());
    }
}

void TACGenerator::printTAC(FILE* out) const {
    if (!globalInstructions.empty()) {
        fprintf(out, "\n_global_init:\n");
        for (size_t i = 0; i < globalInstructions.size(); ++i) globalInstructions[i].print(out);
    }
    for (size_t i = 0; i < instructions.size(); ++i) instructions[i].print(out);
    if (!finiInstructions.empty()) {
        fprintf(out, "\n_global_fini:\n");
        for (size_t i = 0; i < finiInstructions.size(); ++i) finiInstructions[i].print(out);
    }
}

static std::string varDesc(const VarInfo& v, const LayoutTable& L) {
    Type base = v.ty;
    base.arr = 0;                                    // the dimensions are printed separately
    std::string s = v.name + ":" + typeName(base);
    for (size_t i = 0; i < v.dims.size(); ++i) s += "[" + (v.dims[i] ? std::to_string(v.dims[i]) : std::string()) + "]";
    if (v.isRef) s += "&";
    // a reference or an array parameter is a single address; everything else occupies its object size
    long size = (v.isRef || (v.isParam && !v.dims.empty())) ? 4 : L.objectSize(v.ty, v.dims);
    s += "(" + std::to_string(size) + ")";
    if (v.isStatic) s += " static";
    return s;
}

// Human-readable dump of the layouts and symbol tables the code generator works from.
void TACGenerator::printTables(FILE* out, const LayoutTable& L) const {
    fprintf(out, "\n# ---- tables ----\n");
    L.print(out);
    for (size_t i = 0; i < globals.size(); ++i)
        fprintf(out, "# global   %s\n", varDesc(globals[i], L).c_str());
    for (size_t i = 0; i < funcs.size(); ++i) {
        const FuncInfo& f = funcs[i];
        fprintf(out, "# function %s -> %s%s\n", f.name.c_str(), typeName(f.ret).c_str(), f.returnsRef ? "&" : "");
        for (size_t j = 0; j < f.params.size(); ++j)
            fprintf(out, "#   param  %s\n", varDesc(f.params[j], L).c_str());
        for (size_t j = 0; j < f.locals.size(); ++j)
            fprintf(out, "#   local  %s\n", varDesc(f.locals[j], L).c_str());
        std::string aggrTemps;
        for (size_t j = 0; j < f.temps.size(); ++j) {
            const Type& t = f.temps[j].second;
            if (t.aggr && t.ptr == 0 && t.arr == 0)
                aggrTemps += " " + f.temps[j].first + ":" + typeName(t) + "(" + std::to_string(L.baseSize(t)) + ")";
        }
        if (!aggrTemps.empty()) fprintf(out, "#   struct temps%s\n", aggrTemps.c_str());
    }
}

bool TACGenerator::saveTAC(const std::string& filename, bool withTables, const LayoutTable& layouts) const {
    FILE* file = fopen(filename.c_str(), "w");
    if (!file) {
        perror(("Error opening output file: " + filename).c_str());
        return false;
    }
    printTAC(file);
    if (withTables) printTables(file, layouts);
    fclose(file);
    return true;
}
