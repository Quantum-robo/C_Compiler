#!/usr/bin/env bash

set -e

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <executable>"
    exit 1
fi

EXECUTABLE="$1"

# Convert executable path to an absolute path
if [[ "$EXECUTABLE" != /* ]]; then
    EXECUTABLE="$(cd "$(dirname "$EXECUTABLE")" && pwd)/$(basename "$EXECUTABLE")"
fi

if [ ! -x "$EXECUTABLE" ]; then
    echo "Error: executable not found or not executable: $EXECUTABLE"
    exit 1
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEST_DIR="$ROOT_DIR/tests/lexer"

if [ ! -d "$TEST_DIR" ]; then
    echo "Error: test directory not found: $TEST_DIR"
    exit 1
fi

for test_file in "$TEST_DIR"/test*.c; do
    echo "========================================"
    echo "Testing: $test_file"
    echo "========================================"

    "$EXECUTABLE" "$test_file"

    echo
done

echo "========================================"
echo "All lexer tests completed successfully."
echo "========================================"