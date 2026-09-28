#!/bin/bash

# Navigate to the root directory dynamically so the script can be run from anywhere
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR" || exit 1

echo ""
echo " Building Lexical Analyzer (Lexer)..."
echo ""
make clean
make lexer

if [ ! -f "./lexer" ] && [ ! -f "./lexer.exe" ]; then
    echo "Compilation failed! Cannot run tests."
    exit 1
fi

# Determine if we should use ./lexer or ./lexer.exe
LEXER_BIN="./lexer"
if [ ! -f "$LEXER_BIN" ] && [ -f "./lexer.exe" ]; then
    LEXER_BIN="./lexer.exe"
fi

echo ""
echo ""
echo " Running Lexer Test Cases..."
echo ""

# Loop through all test files in the lexer tests directory
for test_file in tests/lexer/*.c; do
    # Skip if no files exist
    if [ ! -f "$test_file" ]; then
        echo "No test files found in tests/lexer/"
        break
    fi

    echo ""
    echo "-----------------------------------------"
    echo " Testing: $test_file"
    echo "-----------------------------------------"
    
    $LEXER_BIN "$test_file"
done

echo ""
echo ""
echo " All lexer tests completed!"
echo "............................................"