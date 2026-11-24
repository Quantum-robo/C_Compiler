#pragma once
// Local (basic-block) optimisations on the three-address code, run before MIPS code generation.
//
//   constant folding        t = 2 * 3            ->  t = 6
//   constant propagation    x = 5; t = x + 1     ->  x = 5; t = 5 + 1   (then folded to t = 6)
//   copy propagation        y = x; t = y + 1     ->  y = x; t = x + 1
//   common subexpressions   t1 = a + b; t2 = a + b   ->  t1 = a + b; t2 = t1
//   dead code elimination   instructions whose result is never used, unreachable code after a jump,
//                           jumps to the next instruction, branches on a constant, unused labels
//
// Facts (x holds the constant 5, y is a copy of x, t1 holds a + b) are only kept inside one basic block:
// they are forgotten at every label and jump, so no data-flow analysis across blocks is needed.
//
// Only "simple" variables are tracked: one-word integers and pointers that are locals, parameters or
// temporaries of the function, whose address is never taken and that are not references. Nothing else
// (globals, arrays, structs, char / short / bool, float / double, references) can then change behind the
// optimiser's back: a store through a pointer or a call can only reach variables whose address was taken.
#include "../ir/TAC.hpp"

struct OptStats {
    int folded = 0;          // operations computed at compile time
    int constProp = 0;       // operand uses replaced by a constant
    int copyProp = 0;        // operand uses replaced by the variable they are a copy of
    int cse = 0;             // recomputations replaced by a copy of the earlier result
    int branches = 0;        // conditional jumps on a constant, resolved
    int deadCode = 0;        // instructions removed
    int deadTemps = 0;       // temporaries that no longer need a stack slot
};

// Optimise every function of 'tac' in place (the global initialiser / finaliser code is left as it is).
OptStats optimizeTAC(TACGenerator& tac);
