#!/usr/bin/env python3
"""Lists the functions longer than 30 lines that have no comment directly above them.

The assignment asks for a comment on every function definition of more than 30 lines.  A function is found as
a line that starts in column 0, contains '(' and ends with '{', up to the closing '}' in column 0.  It counts as
commented when the nearest non-blank line above it is a comment (// ..., or the end of a /* ... */ block).

    python3 scripts/long_functions.py            # checks src/ (exit status 1 if something is missing)
    python3 scripts/long_functions.py -v         # also lists the long functions that are commented
"""
import os
import re
import sys

LIMIT = 30
FILES = (".cpp", ".hpp", ".y", ".l")
SIGNATURE = re.compile(r"^[A-Za-z_~][\w:<>,\*&\s~]*\(.*\)\s*(const)?\s*(\{|[^;{}]*\{)\s*$")
KEYWORDS = ("if", "for", "while", "switch", "else", "do", "return", "struct", "class", "enum", "namespace")


def is_comment_line(line):
    t = line.strip()
    return t.startswith("//") or t.endswith("*/") or t.startswith("*") or t.startswith("/*")


def functions(path):
    with open(path, encoding="utf-8", errors="replace") as f:
        lines = f.read().split("\n")
    i = 0
    while i < len(lines):
        line = lines[i]
        first = line.split("(")[0].split()
        if (SIGNATURE.match(line) and line[0] not in " \t#}" and first and first[0] not in KEYWORDS
                and not line.startswith("%")):
            j = i
            while j < len(lines) and lines[j] != "}":
                j += 1
            if j < len(lines):
                k = i - 1
                while k >= 0 and not lines[k].strip():
                    k -= 1
                yield i + 1, j - i + 1, line.strip(), k >= 0 and is_comment_line(lines[k])
                i = j
        i += 1


def main():
    verbose = "-v" in sys.argv
    missing = 0
    for root, _, names in os.walk("src"):
        for name in sorted(names):
            if not name.endswith(FILES) or name.endswith((".tab.c", "lex.yy.c")):
                continue
            path = os.path.join(root, name)
            for line_no, length, sig, commented in functions(path):
                if length <= LIMIT:
                    continue
                if not commented:
                    missing += 1
                    print("%s:%d: %d lines, no comment: %s" % (path, line_no, length, sig[:90]))
                elif verbose:
                    print("%s:%d: %d lines, commented: %s" % (path, line_no, length, sig[:90]))
    print("%d long function(s) without a comment" % missing)
    return 1 if missing else 0


if __name__ == "__main__":
    sys.exit(main())
