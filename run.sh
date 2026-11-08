#!/bin/bash
# Runs the compiler over every test case.
#
#   ./run.sh <file.c | folder>               print the IR of that file, or of every .c file in the
#                                            folder and save each as ./<name>.TAC (run 'make' first)
#   ./run.sh --test <executable>             run the suite in tests/, e.g.  ./run.sh --test ./parser
#   ./run.sh --archived <executable>         run the archived front-end suite (archived_tests/25_OCT)
#   ./run.sh --update <exe>                  re-bless the expected IR of tests/ir (only after review)
#   ./run.sh --update --archived <exe>       re-bless the archived expected IR (only after review)
#
# Layout of tests/ (default run):
#   tests/parser/*.c                       syntax tests; a name containing "_invalid" must be rejected
#   tests/semantic/*.c                     must compile (IR is generated but not compared)
#   tests/errors/err_*.c                   must be rejected; the diagnostics must contain the "// EXPECT:" text
#   tests/ir/*.c                           the IR and the layout / symbol tables (--tables) must equal
#                                          tests/ir/expected/<name>.tac
#   tests/codegen/*.c                      compiled to MIPS (-S), run on SPIM (stdin from <name>.in if it exists);
#                                          the output must equal tests/codegen/<name>.expected
#
# Layout of archived_tests/25_OCT (run with --archived):
#   *.c                                    one program per language feature; the generated IR must
#                                          equal expected/<name>.tac
#   semantic_errors/err_*.c                must be rejected; stderr must contain the text after
#                                          "// EXPECT:" on the first line
#   semantic_errors/warn_*.c               must be accepted, with that warning
#   semantic_valid/ok_*.c                  must be accepted with no diagnostics at all
#
# Related make targets: make check, make check-archived, make test-parser, make test-semantic, make ir.
# The generated IR is written to tests/ir_output/ (ignored by git).

UPDATE=0
TEST_MODE=0
ARCHIVED=0
while [ $# -gt 0 ]; do
    case "$1" in
        --update)   UPDATE=1; TEST_MODE=1; shift ;;
        --test)     TEST_MODE=1; shift ;;
        --archived) ARCHIVED=1; TEST_MODE=1; shift ;;
        *) break ;;
    esac
done

# ---- IR mode:  ./run.sh <file.c | folder>  prints the IR of that file (or of every .c in the folder) ----
if [ $TEST_MODE -eq 0 ]; then
    if [ $# -ne 1 ]; then
        echo "Usage: $0 <file.c | folder>        print the IR of a file, or of every .c file in a folder"
        echo "       $0 --test <executable>             run the suite in tests/"
        echo "       $0 --archived [--update] <executable>   run the archived suite (archived_tests/)"
        exit 1
    fi
    TARGET="$1"
    BIN="$(dirname "$0")/parser"
    [ -x "$BIN" ] || BIN="$BIN.exe"
    if [ ! -x "$BIN" ]; then
        echo "Error: compiler not built (run 'make' first)."
        exit 1
    fi
    if [ -d "$TARGET" ]; then
        files=$(find "$TARGET" -type f -name '*.c' | sort)
    elif [ -f "$TARGET" ]; then
        files="$TARGET"
    else
        echo "Error: '$TARGET' is not a file or folder."
        exit 1
    fi
    if [ -z "$files" ]; then
        echo "No .c files found in '$TARGET'."
        exit 1
    fi
    tmp=$(mktemp)
    rc=0
    while IFS= read -r f; do
        echo "===== IR: $f ====="
        outfile="$(basename "${f%.c}").TAC"
        if "$BIN" "$f" "$outfile" > /dev/null 2> "$tmp.err" < /dev/null; then
            cat "$outfile"
            echo "-----> IR saved to $outfile"
        else
            rm -f "$outfile"
            cat "$tmp.err"
            rc=1
        fi
        echo ""
    done <<< "$files"
    rm -f "$tmp" "$tmp.err"
    exit $rc
fi

if [ $# -ne 1 ]; then
    echo "Usage: $0 [--test | --archived [--update]] <executable>"
    exit 1
fi

PARSER="$1"

if [ ! -x "$PARSER" ] && [ -x "$PARSER.exe" ]; then
    PARSER="$PARSER.exe"
fi

if [ ! -x "$PARSER" ]; then
    echo "Error: executable '$PARSER' not found or not executable (run 'make' first)."
    exit 1
fi

# The OS (application control / antivirus) can block a freshly built executable for a while.
# Probe it first so that a blocked launch is reported once, not as a failure of every test.
probe=1
for i in 1 2 3 4 5; do
    "$PARSER" /dev/null /dev/null > /dev/null 2>&1; probe=$?
    [ $probe -lt 126 ] && break
    sleep 3
done
if [ $probe -ge 126 ]; then
    echo "Error: '$PARSER' cannot be launched (exit status $probe)."
    echo "The operating system is probably blocking the new executable; allow it and run again."
    exit 2
fi

TESTS="tests"
ARCH="archived_tests/25_OCT"
OUT="$TESTS/ir_output"
EXPECTED="$ARCH/expected"
TMP="$OUT/_tmp"

mkdir -p "$OUT"
[ $ARCHIVED -eq 1 ] && mkdir -p "$EXPECTED"

total=0
passed=0
failed=0

pass() { echo "  PASS  $1"; passed=$((passed + 1)); }
fail() { echo "  FAIL  $1"; failed=$((failed + 1)); }
show() { sed 's/^/          /' "$1" | head -"${2:-12}"; }
group() { echo ""; echo "== $1"; }

# compile <source> <ir-output> <stderr-file> [flags]: exit status of the compiler
compile() {
    local s i
    for i in 1 2 3; do
        "$PARSER" $4 "$1" "$2" > /dev/null 2> "$3"; s=$?
        [ $s -lt 126 ] && break
        sleep 2   # the OS may block the first launch of a fresh executable
    done
    return $s
}

# ---- 1. syntax tests ----
if [ $ARCHIVED -eq 0 ]; then
group "Syntax tests ($TESTS/parser)"
for f in "$TESTS"/parser/*.c; do
    [ -e "$f" ] || continue
    total=$((total + 1))
    name=$(basename "$f")
    mkdir -p "$OUT/parser"
    compile "$f" "$OUT/parser/${name%.c}.tac" "$TMP.err"
    status=$?
    if [[ "$name" == *_invalid* ]]; then
        if [ $status -ne 0 ]; then pass "$name (error correctly detected)"
        else fail "$name (invalid program was accepted)"; fi
    else
        if [ $status -eq 0 ]; then pass "$name"
        else fail "$name (valid program was rejected)"; show "$TMP.err"; fi
    fi
done

fi

if [ $ARCHIVED -eq 1 ]; then
# ---- 2. one program per language feature: the IR must match the reviewed expected IR ----
group "Feature tests: generated IR vs expected IR ($ARCH)"
for f in "$ARCH"/*.c; do
    [ -e "$f" ] || continue
    total=$((total + 1))
    name=$(basename "$f" .c)
    mkdir -p "$OUT/archived"
    out="$OUT/archived/$name.tac"
    exp="$EXPECTED/$name.tac"
    if ! compile "$f" "$out" "$TMP.err"; then
        fail "$name (rejected)"; show "$TMP.err"
    elif [ $UPDATE -eq 1 ]; then
        cp "$out" "$exp"; pass "$name (expected IR updated)"
    elif [ ! -f "$exp" ]; then
        fail "$name (no expected IR; review the output, then run with --update)"
    elif diff --strip-trailing-cr -u "$exp" "$out" > "$TMP.diff"; then
        pass "$name"
    else
        fail "$name (IR differs from expected)"; show "$TMP.diff" 20
    fi
done

# ---- 3. semantic errors and warnings ----
group "Semantic errors and warnings ($ARCH/semantic_errors)"
for f in "$ARCH"/semantic_errors/*.c; do
    [ -e "$f" ] || continue
    total=$((total + 1))
    name=$(basename "$f" .c)
    want=$(head -1 "$f" | tr -d '\015' | sed -n 's#^// EXPECT: *##p')
    compile "$f" "$TMP.tac" "$TMP.err"
    status=$?
    if [ -z "$want" ]; then
        fail "$name (missing '// EXPECT:' header)"
    elif [[ "$name" == warn_* ]]; then
        if [ $status -ne 0 ]; then fail "$name (should only warn, but was rejected)"; show "$TMP.err"
        elif grep -qF -- "$want" "$TMP.err"; then pass "$name"
        else fail "$name (warning not reported: $want)"; show "$TMP.err"; fi
    else
        if [ $status -eq 0 ]; then fail "$name (invalid program was accepted)"
        elif grep -qF -- "$want" "$TMP.err"; then pass "$name"
        else fail "$name (wrong message, wanted: $want)"; show "$TMP.err"; fi
    fi
done

# ---- 4. valid programs must produce no diagnostics ----
group "Valid programs, no diagnostics ($ARCH/semantic_valid)"
for f in "$ARCH"/semantic_valid/*.c; do
    [ -e "$f" ] || continue
    total=$((total + 1))
    name=$(basename "$f" .c)
    if ! compile "$f" "$TMP.tac" "$TMP.err"; then
        fail "$name (valid program was rejected)"; show "$TMP.err"
    elif [ -s "$TMP.err" ]; then
        fail "$name (unexpected diagnostics)"; show "$TMP.err"
    else
        pass "$name"
    fi
done

fi

if [ $ARCHIVED -eq 0 ]; then
# ---- 5. earlier feature programs: must compile ----
group "Compile-only tests ($TESTS/semantic)"
for f in "$TESTS"/semantic/*.c; do
    [ -e "$f" ] || continue
    total=$((total + 1))
    name=$(basename "$f")
    mkdir -p "$OUT/semantic"
    if compile "$f" "$OUT/semantic/${name%.c}.tac" "$TMP.err"; then pass "$name"
    else fail "$name (rejected)"; show "$TMP.err"; fi
done

# ---- 6. IR tests: the generated IR and symbol / layout tables must match the reviewed expected output ----
group "IR tests: generated IR and tables vs expected ($TESTS/ir)"
for f in "$TESTS"/ir/*.c; do
    [ -e "$f" ] || continue
    total=$((total + 1))
    name=$(basename "$f" .c)
    mkdir -p "$OUT/ir" "$TESTS/ir/expected"
    out="$OUT/ir/$name.tac"
    exp="$TESTS/ir/expected/$name.tac"
    if ! compile "$f" "$out" "$TMP.err" --tables; then
        fail "$name (rejected)"; show "$TMP.err"
    elif [ $UPDATE -eq 1 ]; then
        cp "$out" "$exp"; pass "$name (expected IR updated)"
    elif [ ! -f "$exp" ]; then
        fail "$name (no expected IR; review the output, then run with --update)"
    elif diff --strip-trailing-cr -u "$exp" "$out" > "$TMP.diff"; then
        pass "$name"
    else
        fail "$name (IR differs from expected)"; show "$TMP.diff" 20
    fi
done

# ---- 6b. programs that must be rejected: access control and common mistakes ----
# The first line holds "// EXPECT: <text>"; the compiler must fail and its diagnostics must contain the text.
group "Rejected programs ($TESTS/errors)"
for f in "$TESTS"/errors/*.c; do
    [ -e "$f" ] || continue
    total=$((total + 1))
    name=$(basename "$f" .c)
    want=$(head -1 "$f" | tr -d '' | sed -n 's#^// EXPECT: *##p')
    compile "$f" "$TMP.tac" "$TMP.err"
    status=$?
    if [ -z "$want" ]; then fail "$name (missing '// EXPECT:' header)"
    elif [ $status -eq 0 ]; then fail "$name (invalid program was accepted)"
    elif grep -qF -- "$want" "$TMP.err"; then pass "$name"
    else fail "$name (wrong message, wanted: $want)"; show "$TMP.err"; fi
done

# ---- 7. end-to-end tests: compile to MIPS, run on SPIM, compare the output with <name>.expected ----
# A <name>.in file, if present, is fed to the program's standard input.
group "End-to-end tests: MIPS code run on SPIM ($TESTS/codegen)"
if ! command -v spim > /dev/null 2>&1; then
    echo "  SKIP  spim is not installed (sudo apt install spim): the end-to-end tests were not run"
else
    for f in "$TESTS"/codegen/*.c; do
        [ -e "$f" ] || continue
        total=$((total + 1))
        name=$(basename "$f" .c)
        mkdir -p "$OUT/codegen"
        asm="$OUT/codegen/$name.s"
        got="$OUT/codegen/$name.out"
        exp="$TESTS/codegen/$name.expected"
        inp=/dev/null
        [ -f "$TESTS/codegen/$name.in" ] && inp="$TESTS/codegen/$name.in"
        if ! compile "$f" "$OUT/codegen/$name.tac" "$TMP.err" "-S $asm"; then
            fail "$name (compile failed)"; show "$TMP.err"
            continue
        fi
        if command -v timeout > /dev/null 2>&1; then run_spim="timeout 60 spim"; else run_spim="spim"; fi
        $run_spim -quiet -file "$asm" < "$inp" 2>&1             | sed '/^SPIM Version/d;/^Copyright 1990/d;/^All Rights Reserved/d;/^See the file README/d;/^Loaded:/d' > "$got"
        if [ $UPDATE -eq 1 ]; then
            cp "$got" "$exp"; pass "$name (expected output updated)"
        elif [ ! -f "$exp" ]; then
            fail "$name (no $name.expected; review the output, then run with --update)"
        elif diff --strip-trailing-cr -u "$exp" "$got" > "$TMP.diff"; then
            pass "$name"
        else
            fail "$name (output differs from $name.expected)"; show "$TMP.diff" 20
        fi
    done
fi

fi

rm -f "$TMP.err" "$TMP.diff" "$TMP.tac"

echo ""
echo "=============================="
echo "Test Summary"
echo "=============================="
echo "Total:  $total"
echo "Passed: $passed"
echo "Failed: $failed"
echo ""

if [ $failed -eq 0 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "Some tests failed."
    exit 1
fi
