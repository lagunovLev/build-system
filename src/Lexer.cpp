#include "Lexer.h"
#include <cctype>
#include <iostream>

char Lexer::current() {
    if (cur_i < code_length)
        return code[cur_i];
    return '\0';
}

char Lexer::next() {
    if (next_i < code_length)
        return code[next_i];
    return '\0';
}

char Lexer::move() {
    cur_i++;
    next_i++;
    return current();
}

std::list<Token> Lexer::tokenize(std::string code)
{
    this->code = code;
    code_length = code.size();
    cur_i = 0;
    next_i = 1;

    std::list<Token> tokens;
    Token token;
    do
    {
        token = getNextToken();
        tokens.push_back(token);
        if (token.type == TokenType::Error)
        {
            std::cout << token.string_val.value() << "\n";
            break;
        }
    } while (token.type != TokenType::EOI);
    return std::move(tokens);
}

Lexer::Lexer() {
    word_to_token["if"] = Token(TokenType::If);
    word_to_token["else"] = Token(TokenType::Else);
    word_to_token["while"] = Token(TokenType::While);
    word_to_token["for"] = Token(TokenType::For);
    word_to_token["foreach"] = Token(TokenType::Foreach);
    word_to_token["do"] = Token(TokenType::Do);
    word_to_token["in"] = Token(TokenType::In);
    word_to_token["var"] = Token(TokenType::Var);
    word_to_token["const"] = Token(TokenType::Const);
    word_to_token["fn"] = Token(TokenType::Fn);
    word_to_token["true"] = Token::BoolLiteral(true);
    word_to_token["false"] = Token::BoolLiteral(false);
    word_to_token["int"] = Token(TokenType::IntType);
    word_to_token["float"] = Token(TokenType::FloatType);
    word_to_token["bool"] = Token(TokenType::BoolType);
    word_to_token["string"] = Token(TokenType::StringType);

    op_to_token[">"] = Token(TokenType::Greater);
    op_to_token["<"] = Token(TokenType::Less);
    op_to_token["="] = Token(TokenType::Assign);
    op_to_token["=="] = Token(TokenType::Equal);
    op_to_token[";"] = Token(TokenType::Semicolon);
    op_to_token[":"] = Token(TokenType::Colon);
    op_to_token["&&"] = Token(TokenType::And);
    op_to_token["||"] = Token(TokenType::Or);
    op_to_token["!"] = Token(TokenType::Not);
    op_to_token["!="] = Token(TokenType::NotEqual);
    op_to_token[","] = Token(TokenType::Comma);
    op_to_token["."] = Token(TokenType::Dot);
    op_to_token["+"] = Token(TokenType::Plus);
    op_to_token["-"] = Token(TokenType::Minus);
    op_to_token["*"] = Token(TokenType::Asterisk);
    op_to_token["/"] = Token(TokenType::Slash);
    op_to_token["\\"] = Token(TokenType::BackSlash);
    op_to_token["%"] = Token(TokenType::Percent);
    op_to_token["("] = Token(TokenType::LParent);
    op_to_token[")"] = Token(TokenType::RParent);
    op_to_token["["] = Token(TokenType::LBracket);
    op_to_token["]"] = Token(TokenType::RBracket);
    op_to_token["{"] = Token(TokenType::LBrace);
    op_to_token["}"] = Token(TokenType::RBrace);
    op_to_token["*="] = Token(TokenType::AsteriskEqual);
    op_to_token["/="] = Token(TokenType::SlashEqual);
    op_to_token["+="] = Token(TokenType::PlusEqual);
    op_to_token["-"] = Token(TokenType::MinusEqual);
}

Token Lexer::getNextToken() {
    if (skip_spaces())
        return std::move(Token(TokenType::NewLine));
    if (current() == '\0')
        return std::move(Token(TokenType::EOI));
    if (isdigit(current()))
        return number();
    if (isalpha(current()) || current() == '_')
        return word();
    if (current() == '"')
        return str();
    if (ispunct(current()))
        return op();
    return error_symbol_not_allowed(current());
}

bool Lexer::skip_spaces() {
    bool newline = false;
    while (current() == ' ' || current() == '\t' || current() == '\n' || current() == '#' || current() == '\r')
    {
        if (current() == '#')
        {
            move();
            while (current() != '\n')
                move();
            continue;
        }
        if (current() == '\n')
        {
            newline = true;
            move();
            continue;
        }
        move();
    }
    return newline;
}

Token Lexer::number()
{
    int v = 0; 
    do
    {
        while (current() == '_')
            move();
        if (isdigit(current()))
        {
            int n = current() - '0';
            v = 10 * v + n;
            move();
        }
    } while (isdigit(current()) || current() == '_');
    if (current() == 'f')
    {
        move();
        return Token::FloatLiteral(v);
    }
    if (current() != '.') {
        return Token::IntegerLiteral(v);
    }

    float x = v;
    float d = 10;
    while (true)
    {
        move();
        if (current() == '_') continue;
        if (!isdigit(current())) break;
        int n = current() - '0';
        x = x + n / d;
        d = d * 10;
    }
    if (current() == 'f')
        move();
    return Token::FloatLiteral(x);
}

Token Lexer::word()
{
    std::string word;
    while (isalpha(current()) || isdigit(current()) || current() == '_')
    {
        word += current();
        move();
    }
    if (word_to_token.contains(word))
        return word_to_token[word];
    return Token::Identifier(word);
}

Token Lexer::op()
{
    if (auto op = std::string() + current() + next(); op_to_token.contains(op))
    {
        move(); move();
        return op_to_token[op];
    }
    if (auto op = std::string() + current(); op_to_token.contains(op))
    {
        move();
        return op_to_token[op];
    }
    return error_symbol_not_allowed(current());
}

Token Lexer::str()
{
    move();
    std::string str;
    while (true)
    {
        if (current() == '"')
        {
            move();
            return Token::StringLiteral(str);
        }
        if (current() == '\0' || current() == '\n')
        {
            move();
            return error_expected_end_of_string();
        }
        if (current() == '\\')
        {
            move();
            if (current() == '\\')
                str += '\\';
            else if (current() == 'n')
                str += '\n';
            else if (current() == 't')
                str += '\t';
            else
            {
                move();
                return error_wrong_escape_character();
            }
            move();
            continue;
        }
        str += current();
        move();
    }
}

Token Lexer::error_symbol_not_allowed(char ch)
{
    return Token::Error(std::string("This symbol ") + ch + " is not allowed");
}

Token Lexer::error_expected_end_of_string()
{
    return Token::Error("Expected end of string");
}

Token Lexer::error_wrong_escape_character()
{
    return Token::Error("Wrong escape character");
}