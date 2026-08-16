CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./src/lexer

LEXER_DIR = src/lexer

LEXER_SRC = $(LEXER_DIR)/Lexer.l
LEXER_GEN = $(LEXER_DIR)/lex.yy.cc

LEXER_OBJ = \
	$(LEXER_DIR)/lex.yy.o \
	$(LEXER_DIR)/Token.o \
	$(LEXER_DIR)/LexerMain.o

LEXER_BIN = $(LEXER_DIR)/lexer

.PHONY: all lexer clean test

all: lexer

lexer: $(LEXER_BIN)

$(LEXER_GEN): $(LEXER_SRC)
	flex -o $@ $<

$(LEXER_DIR)/lex.yy.o: $(LEXER_GEN)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LEXER_DIR)/Token.o: $(LEXER_DIR)/Token.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LEXER_DIR)/LexerMain.o: $(LEXER_DIR)/LexerMain.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LEXER_BIN): $(LEXER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

test: lexer
	@echo "Running lexer tests..."
	@for file in tests/lexer/*.c; do \
		echo "========================================"; \
		echo "Testing $$file"; \
		$(LEXER_BIN) "$$file"; \
		status=$$?; \
		if [ "$$file" = "tests/lexer/errors.c" ]; then \
			if [ $$status -ne 1 ]; then \
				echo "FAIL: expected exit code 1"; \
				exit 1; \
			fi; \
		else \
			if [ $$status -ne 0 ]; then \
				echo "FAIL: expected exit code 0"; \
				exit 1; \
			fi; \
		fi; \
	done
	@echo "========================================"
	@echo "All lexer tests passed."

clean:
	rm -f $(LEXER_DIR)/lex.yy.cc
	rm -f $(LEXER_DIR)/*.o
	rm -f $(LEXER_BIN)
	rm -f $(LEXER_DIR)/lexer_test