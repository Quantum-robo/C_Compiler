#!/usr/bin/env bash

set -e

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <source-file>"
    exit 1
fi

cd "$ROOT_DIR"

make

./src/lexer/lexer "$1"