CXX      = g++
FLEX     = flex
BISON    = bison
CXXFLAGS = -std=gnu++17

ifeq ($(OS),Windows_NT)
EXE = .exe
LDFLAGS = -static
endif
LEXER_BIN  = ./lexer$(EXE)
PARSER_BIN = ./parser$(EXE)

# Windows application-control / antivirus may block the first launch of a freshly linked
# executable for a few seconds; launch it once here (retrying) so later targets can run it.
WARMUP = for i in 1 2 3 4 5 6 7 8 9 10 11 12; do $(1) /dev/null >/dev/null 2>&1; [ $$? -lt 126 ] && exit 0; sleep 2; done; exit 0

LEXER_DIR  = src/lexer
PARSER_DIR = src/parser
IR_DIR     = src/ir
SEM_DIR    = src/semantic

LEXER_GEN = $(LEXER_DIR)/lex.yy.c
PARSER_C  = $(PARSER_DIR)/parser.tab.c
PARSER_H  = $(PARSER_DIR)/parser.tab.h

# Headers are prerequisites so that editing one rebuilds the binaries.
HEADERS = $(wildcard $(LEXER_DIR)/*.hpp $(IR_DIR)/*.hpp $(SEM_DIR)/*.hpp)

.PHONY: all test test-lexer test-parser test-semantic ir check clean

all: lexer parser

# On Windows the real files are lexer.exe / parser.exe, so "make lexer" and "make parser" are aliases.
# On Linux the target name is the file name itself and needs no alias.
ifneq ($(EXE),)
.PHONY: lexer parser
lexer: lexer$(EXE)
parser: parser$(EXE)
endif

# Standalone lexer (prints the token table)

lexer$(EXE): $(LEXER_GEN) $(PARSER_H) $(LEXER_DIR)/Token.cpp $(LEXER_DIR)/main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -I$(LEXER_DIR) -I$(PARSER_DIR) \
		$(LEXER_GEN) $(LEXER_DIR)/Token.cpp $(LEXER_DIR)/main.cpp $(LDFLAGS) -o lexer$(EXE)
	@$(call WARMUP,./lexer$(EXE))

# Compiler front end: lexer + parser + semantic checks + IR generation

parser$(EXE): $(PARSER_C) $(LEXER_GEN) $(LEXER_DIR)/Token.cpp $(IR_DIR)/TAC.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -I$(LEXER_DIR) -I$(PARSER_DIR) -I$(IR_DIR) \
		$(PARSER_C) $(LEXER_GEN) $(LEXER_DIR)/Token.cpp $(IR_DIR)/TAC.cpp $(LDFLAGS) -o parser$(EXE)
	@$(call WARMUP,./parser$(EXE))

$(PARSER_C) $(PARSER_H): $(PARSER_DIR)/parser.y
	$(BISON) -d -o $(PARSER_C) $<

$(LEXER_GEN): $(LEXER_DIR)/Lexer.l $(PARSER_H)
	$(FLEX) -o $@ $<

# make test: generate the IR for test.c only (writes ./test.TAC)
test: parser
	@$(PARSER_BIN) test.c test.TAC; s=$$?; \
	if [ $$s -ne 0 ]; then echo "test.c: FAILED (see the error above)"; fi; exit $$s

# make test-lexer: run the standalone lexer over tests/lexer/*.c and show lexical errors
test-lexer: lexer
	@total=0; bad=0; \
	for f in tests/lexer/*.c; do \
		[ -e "$$f" ] || continue; \
		total=$$((total + 1)); \
		echo "== $$f"; \
		$(LEXER_BIN) "$$f" > /dev/null 2> .lexer.err; s=$$?; \
		cat .lexer.err; \
		if [ $$s -ge 126 ]; then bad=$$((bad + 1)); echo "   -> could not run the lexer (blocked by the OS?)"; \
		elif [ $$s -ne 0 ]; then bad=$$((bad + 1)); echo "   -> lexical errors found"; \
		else echo "   -> no lexical errors"; fi; \
	done; rm -f .lexer.err; \
	echo ""; echo "Lexer: $$total file(s), $$bad with lexical errors"

# make test-parser: syntax-check tests/parser/*.c ("_invalid" files must be rejected)
test-parser: parser
	@total=0; bad=0; \
	for f in tests/parser/*.c; do \
		[ -e "$$f" ] || continue; \
		total=$$((total + 1)); \
		echo "== $$f"; \
		$(PARSER_BIN) "$$f" /dev/null > /dev/null 2> .parser.err; s=$$?; \
		cat .parser.err; \
		case "$$f" in \
			*_invalid*) if [ $$s -ne 0 ]; then echo "   -> PASS (error correctly detected)"; \
			            else echo "   -> FAIL (invalid program was accepted)"; bad=$$((bad + 1)); fi ;; \
			*)          if [ $$s -eq 0 ]; then echo "   -> PASS"; \
			            else echo "   -> FAIL (valid program was rejected)"; bad=$$((bad + 1)); fi ;; \
		esac; \
	done; rm -f .parser.err; \
	echo ""; echo "Parser: $$total file(s), $$bad failed"; [ $$bad -eq 0 ]

# make test-semantic: semantic-check tests/semantic/*.c, print every error found
test-semantic: parser
	@total=0; bad=0; \
	for f in tests/semantic/*.c; do \
		[ -e "$$f" ] || continue; \
		total=$$((total + 1)); \
		echo "== $$f"; \
		$(PARSER_BIN) "$$f" /dev/null > /dev/null 2> .sem.err; s=$$?; \
		cat .sem.err; \
		if [ $$s -eq 0 ]; then echo "   -> PASS (no errors)"; \
		else echo "   -> FAIL"; bad=$$((bad + 1)); fi; \
	done; rm -f .sem.err; \
	echo ""; echo "Semantic: $$total file(s), $$bad with errors"; [ $$bad -eq 0 ]

# make ir: IR for every tests/semantic/*.c, written to ./<name>.TAC.
# If a phase fails, the .TAC file holds the diagnostics instead, headed by the failing phase.
ir: parser
	@for f in tests/semantic/*.c; do \
		[ -e "$$f" ] || continue; \
		out=$$(basename "$$f" .c).TAC; \
		$(PARSER_BIN) "$$f" "$$out" > /dev/null 2> .ir.err; s=$$?; \
		if [ $$s -eq 0 ]; then echo "OK      $$f -> $$out"; \
		else \
			phase=$$(grep -o -m1 'LEXICAL\|SYNTAX\|SEMANTIC' .ir.err | head -1); \
			{ echo "FAILED in $${phase:-UNKNOWN} phase: $$f"; cat .ir.err; } > "$$out"; \
			echo "FAILED  $$f -> $$out ($${phase:-UNKNOWN} phase)"; \
		fi; \
	done; rm -f .ir.err

# make check: the full regression suite (every test folder, golden IR comparison)
check: parser
	./run.sh --test $(PARSER_BIN)

clean:
	rm -f $(LEXER_GEN) $(PARSER_C) $(PARSER_H)
	rm -f lexer lexer.exe parser parser.exe
	rm -f *.TAC .lexer.err .parser.err .sem.err .ir.err
	rm -rf tests/ir_output
	rm -f tests/parser/*.tac
