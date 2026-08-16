#include <fstream>
#include <iostream>

#include "Lexer.hpp"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <source-file>\n";
        return 1;
    }

    std::ifstream input(argv[1]);

    if (!input)
    {
        std::cerr << "Error opening file: " << argv[1] << '\n';
        return 1;
    }

    Lexer lexer(&input);

    std::cout << "Lexeme | Token\n";

    while (true)
    {
        Token token = lexer.nextToken();

        if (token.type != TokenType::EndOfFile)
        {
            std::cout << token.lexeme
                      << " | "
                      << tokenTypeToString(token.type)
                      << '\n';
        }

        if (token.type == TokenType::EndOfFile)
            break;
    }

    if (!lexer.errors().empty())
    {
        std::cerr << "\nLexical Errors:\n";

        for (const auto& error : lexer.errors())
        {
            std::cerr << error.message
                      << " at line " << error.line
                      << ", column " << error.column
                      << '\n';
        }
    }

    return lexer.errors().empty() ? 0 : 1;
}