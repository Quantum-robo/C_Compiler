CXX = g++
FLEX = flex

LEXER_DIR = src/lexer

LEXER_SPEC = $(LEXER_DIR)/Lexer.l
LEXER_GEN = $(LEXER_DIR)/lex.yy.c

LEXER_SOURCES = $(LEXER_GEN) \
                $(LEXER_DIR)/Token.cpp

TARGET = lexer

CXXFLAGS = -std=c++17
CPPFLAGS = -I$(LEXER_DIR)

.PHONY: all lexer test clean

all: lexer

lexer: $(LEXER_GEN) $(LEXER_DIR)/Token.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LEXER_SOURCES) -o $(TARGET)

$(LEXER_GEN): $(LEXER_SPEC)
	$(FLEX) -o $@ $<

test: lexer
	./scripts/run.sh ./$(TARGET)

clean:
	rm -f $(LEXER_GEN)
	rm -f $(TARGET)