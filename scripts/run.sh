#!/usr/bin/env bash

set -e

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <executable>"
    exit 1
fi

EXECUTABLE="$1"

if [ ! -x "$EXECUTABLE" ]; then
    echo "Error: executable not found or not executable: $EXECUTABLE"
    exit 1
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEST_DIR="$ROOT_DIR/tests/lexer"

for test_file in "$TEST_DIR"/*.c; do
    echo "========================================"
    echo "Testing: $test_file"
    echo "========================================"

    if "$EXECUTABLE" "$test_file"; then
        status=0
    else
        status=$?
    fi

    if [ "$status" -ne 0 ]; then
        if [ "$(basename "$test_file")" = "errors.c" ]; then
            echo "Expected lexical errors detected."
        else
            echo "Test failed: $test_file"
            exit 1
        fi
    fi

    echo
done

echo "All lexer tests completed successfully."