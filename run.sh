#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <parser-executable>"
    exit 1
fi

PARSER="$1"

if [ ! -x "$PARSER" ]; then
    echo "Error: parser executable '$PARSER' not found or not executable."
    exit 1
fi

TEST_DIR="tests/parser"

if [ ! -d "$TEST_DIR" ]; then
    echo "Error: test directory '$TEST_DIR' not found."
    exit 1
fi

echo ""
echo "Running Syntax Analyzer Tests"
echo "=============================="

passed=0
failed=0
total=0

for test_file in "$TEST_DIR"/*.c; do
    [ -e "$test_file" ] || continue

    total=$((total + 1))

    echo ""
    echo "-----------------------------------------"
    echo "Testing: $test_file"
    echo "-----------------------------------------"

    "$PARSER" "$test_file"
    status=$?

    # Files containing "_invalid" are expected to be rejected.
    if [[ "$test_file" == *"_invalid"* ]]; then
        if [ $status -ne 0 ]; then
            echo "Result: PASS (error correctly detected)"
            passed=$((passed + 1))
        else
            echo "Result: FAIL (invalid program was accepted)"
            failed=$((failed + 1))
        fi
    else
        if [ $status -eq 0 ]; then
            echo "Result: PASS"
            passed=$((passed + 1))
        else
            echo "Result: FAIL (valid program was rejected)"
            failed=$((failed + 1))
        fi
    fi
done

echo ""
echo "=============================="
echo "Test Summary"
echo "=============================="
echo "Total:  $total"
echo "Passed: $passed"
echo "Failed: $failed"

if [ $failed -eq 0 ]; then
    echo ""
    echo "All parser tests passed!"
    exit 0
else
    echo ""
    echo "Some parser tests failed."
    exit 1
fi
