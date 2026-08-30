CXX = g++
FLEX = flex
BISON = bison

LEXER_DIR = src/lexer
PARSER_DIR = src/parser

LEXER_SPEC = $(LEXER_DIR)/Lexer.l
LEXER_GEN = $(LEXER_DIR)/lex.yy.c

PARSER_YACC_SPEC = $(PARSER_DIR)/parser.y
PARSER_YACC_GEN_C = $(PARSER_DIR)/parser.tab.c
PARSER_YACC_GEN_H = $(PARSER_DIR)/parser.tab.h

# Lexer standalone uses its own main.cpp
LEXER_SOURCES = $(LEXER_GEN) \
                $(LEXER_DIR)/Token.cpp \
                $(LEXER_DIR)/main.cpp

# Parser sources use the same lex.yy.c generated from Lexer.l
PARSER_SOURCES = $(PARSER_YACC_GEN_C) \
                 $(LEXER_GEN) \
                 $(LEXER_DIR)/Token.cpp

TARGET = lexer
PARSER_TARGET = parser

CXXFLAGS = -std=c++17
CPPFLAGS = -I$(LEXER_DIR)
PARSER_CPPFLAGS = -I$(LEXER_DIR) -I$(PARSER_DIR)

.PHONY: all lexer parser test-lexer test-parser clean

all: lexer parser

lexer: $(PARSER_YACC_GEN_H) $(LEXER_GEN) $(LEXER_DIR)/Token.cpp $(LEXER_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -I$(PARSER_DIR) $(LEXER_SOURCES) -o $(TARGET)

parser: $(PARSER_YACC_GEN_C) $(LEXER_GEN) $(LEXER_DIR)/Token.cpp
	$(CXX) $(CXXFLAGS) $(PARSER_CPPFLAGS) $(PARSER_SOURCES) -o $(PARSER_TARGET)

$(PARSER_YACC_GEN_C) $(PARSER_YACC_GEN_H): $(PARSER_YACC_SPEC)
	$(BISON) -d -o $(PARSER_YACC_GEN_C) $<

$(LEXER_GEN): $(LEXER_SPEC) $(PARSER_YACC_GEN_H)
	$(FLEX) -o $@ $<

test-lexer: lexer
	./scripts/run_lexer.sh

test-parser: parser
	./scripts/run_parser.sh

clean:
	rm -f $(LEXER_GEN)
	rm -f $(TARGET)
	rm -f $(PARSER_YACC_GEN_C) $(PARSER_YACC_GEN_H)
	rm -f $(PARSER_TARGET)