#pragma once
// MIPS32 code generator: turns the typed three-address code (see docs/IR.md) into SPIM assembly.
#include <cstdio>
#include <string>
#include <vector>
#include "../ir/TAC.hpp"
#include "../semantic/Layout.hpp"

// Writes the complete program (data segment, code, runtime library) to 'out'.
// Returns false, with one message per problem in 'errors', when the program uses something the
// generator cannot translate (for example an unknown library function).
bool generateMips(const TACGenerator& tac, const LayoutTable& layouts, FILE* out,
                  std::vector<std::string>& errors);

// Assembly source of the runtime library (printf, malloc, strcpy, ...), see Runtime.cpp.
extern const char* const kRuntimeAsm;
