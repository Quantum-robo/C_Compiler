#!/bin/bash
# Runs the compiler over every test case in tests/.
#
#   ./run.sh <file.c | folder>        print the IR of that file, or of every .c file in the folder
#                                     and saves each as ./<name>.TAC (run 'make' first)
#   ./run.sh --test <executable>      run the regression suite, e.g.  ./run.sh --test ./parser
#   ./run.sh --update <executable>    re-bless the expected IR (only after reviewing it)
#
# Test layout:
#   tests/parser/*.c                       syntax tests; a name containing "_invalid" must be rejected
#   tests/25_OCT/*.c                       one program per language feature; the generated IR must
#                                          equal tests/25_OCT/expected/<name>.tac
#   tests/25_OCT/semantic_errors/err_*.c   must be rejected; stderr must contain the text after
#                                          "// EXPECT:" on the first line
#   tests/25_OCT/semantic_errors/warn_*.c  must be accepted, with that warning
#   tests/25_OCT/semantic_valid/ok_*.c     must be accepted with no diagnostics at all
#   tests/semantic/*.c                     must compile (IR is generated but not compared)
#
# Related make targets: make check (this script), make test-parser, make test-semantic, make ir.
# The generated IR is written to tests/ir_output/ (ignored by git).

UPDATE=0
TEST_MODE=0
case "$1" in
    --update) UPDATE=1; TEST_MODE=1; shift ;;
    --test)   TEST_MODE=1; shift ;;
esac

# ---- IR mode:  ./run.sh <file.c | folder>  prints the IR of that file (or of every .c in the folder) ----
if [ $TEST_MODE -eq 0 ]; then
    if [ $# -ne 1 ]; then
        echo "Usage: $0 <file.c | folder>        print the IR of a file, or of every .c file in a folder"
        echo "       $0 --test|--update <executable>   run the regression suite"
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
    echo "Usage: $0 [--test|--update] <executable>"
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
OUT="$TESTS/ir_output"
EXPECTED="$TESTS/25_OCT/expected"
TMP="$OUT/_tmp"

mkdir -p "$OUT" "$EXPECTED"

total=0
passed=0
failed=0

pass() { echo "  PASS  $1"; passed=$((passed + 1)); }
fail() { echo "  FAIL  $1"; failed=$((failed + 1)); }
show() { sed 's/^/          /' "$1" | head -"${2:-12}"; }
group() { echo ""; echo "== $1"; }

# compile <source> <ir-output> <stderr-file>: exit status of the compiler
compile() {
    local s i
    for i in 1 2 3; do
        "$PARSER" "$1" "$2" > /dev/null 2> "$3"; s=$?
        [ $s -lt 126 ] && break
        sleep 2   # the OS may block the first launch of a fresh executable
    done
    return $s
}

# ---- 1. syntax tests ----
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

# ---- 2. one program per language feature: the IR must match the reviewed expected IR ----
group "Feature tests: generated IR vs expected IR ($TESTS/25_OCT)"
for f in "$TESTS"/25_OCT/*.c; do
    [ -e "$f" ] || continue
    total=$((total + 1))
    name=$(basename "$f" .c)
    mkdir -p "$OUT/25_OCT"
    out="$OUT/25_OCT/$name.tac"
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
group "Semantic errors and warnings ($TESTS/25_OCT/semantic_errors)"
for f in "$TESTS"/25_OCT/semantic_errors/*.c; do
    [ -e "$f" ] || continue
    total=$((total + 1))
    name=$(basename "$f" .c)
    want=$(head -1 "$f" | sed -n 's#^// EXPECT: *##p')
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
group "Valid programs, no diagnostics ($TESTS/25_OCT/semantic_valid)"
for f in "$TESTS"/25_OCT/semantic_valid/*.c; do
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
