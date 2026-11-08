#pragma once
// MIPS32 code generator: turns the typed three-address code (see docs/IR.md) into SPIM assembly.
#include <cstdio>
#include <string>
#include <vector>
#include "../ir/TAC.hpp"
#include "../semantic/Layout.hpp"

// Writes the complete program to 'out': the data segment, the program's code, the runtime functions it uses
// (compiled from C source, see Runtime.cpp; 'rt' is the result of compiling kRuntimeSource) and the
// hand-written syscall stubs. Returns false, with one message per problem in 'errors', when something cannot
// be translated (for example a call to a function that neither the program nor the runtime defines).
bool generateMips(const TACGenerator& prog, const LayoutTable& progLayouts, const TACGenerator& rt,
                  const LayoutTable& rtLayouts, FILE* out, std::vector<std::string>& errors);

// C source of the runtime library (printf, scanf, malloc, strlen, ...), compiled with the program.
extern const char* const kRuntimeSource;

// Assembly of the syscall stubs the runtime's C code calls (__sys_putchar, __sys_readline, ...).
extern const char* const kSysStubsAsm;
