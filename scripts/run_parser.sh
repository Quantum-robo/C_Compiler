#!/bin/bash

# Navigate to the root directory dynamically so the script can be run from anywhere
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR" || exit 1

echo ""
echo " Building Syntax Analyzer (Parser)"
echo ""
make clean
make parser

if [ ! -f "./parser" ] && [ ! -f "./parser.exe" ]; then
    echo "Compilation failed! Cannot run tests."
    exit 1
fi

# Determine if we should use ./parser or ./parser.exe
PARSER_BIN="./parser"
if [ ! -f "$PARSER_BIN" ] && [ -f "./parser.exe" ]; then
    PARSER_BIN="./parser.exe"
fi

echo ""
echo ""
echo " Running Parser Test Cases..."
echo ""

for test_file in tests/parser/*.c; do
    echo ""
    echo "-----------------------------------------"
    echo " Testing: $test_file"
    echo "-----------------------------------------"
    
    $PARSER_BIN "$test_file"
done

echo ""
echo ""
echo " All parser tests completed!"
echo "....................................."
