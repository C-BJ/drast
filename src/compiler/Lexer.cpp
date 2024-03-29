//
// Created by Ashwin Paudel on 2022-03-20.
//

#include "Lexer.h"

void Lexer::lex() {
    for (auto token = getToken(); token.type != TokenType::T_EOF;
         token = getToken()) {
        tokens.push_back(token);
    }

    //    for (auto &token : tokens) {
    //         std::cout << token.toString(this->source) << std::endl;
    //    }
    //
    this->tokens.push_back(returnToken(TokenType::T_EOF));
}

Token Lexer::getToken() {
    this->skipWhitespace();

    this->start = this->index;

    switch (this->current()) {
    case 'a' ... 'z':
    case 'A' ... 'Z':
    case '_':
        return this->identifier();
    case '0' ... '9':
        return this->number();
    case '"':
        return this->string();
    case '\'':
        return this->character();
    case '?':
        return returnToken(TokenType::QUESTION);
    case '<':
        if (this->peek() == '<') {
            this->advance();
            return this->returnToken(TokenType::BITWISE_SHIFT_LEFT,
                                     TokenType::BITWISE_SHIFT_LEFT_EQUAL);
        }

        return this->returnToken(TokenType::LESS_THAN,
                                 TokenType::LESS_THAN_EQUAL);
    case '>':
        if (this->peek() == '>') {
            this->advance();
            return this->returnToken(TokenType::BITWISE_SHIFT_RIGHT,
                                     TokenType::BITWISE_SHIFT_RIGHT_EQUAL);
        }

        return this->returnToken(TokenType::GREATER_THAN,
                                 TokenType::GREATER_THAN_EQUAL);
    case '=':
        return this->returnToken(TokenType::EQUAL, TokenType::EQUAL_EQUAL);
    case '!':
        return this->returnToken(TokenType::NOT, TokenType::NOT_EQUAL);
    case '+':
        return this->returnToken(TokenType::OPERATOR_ADD,
                                 TokenType::OPERATOR_ADD_EQUAL);
    case '-':
        return this->returnToken(TokenType::OPERATOR_SUB,
                                 TokenType::OPERATOR_SUB_EQUAL);
    case '*':
        return this->returnToken(TokenType::OPERATOR_MUL,
                                 TokenType::OPERATOR_MUL_EQUAL);
    case '/':
        if (peek() == '/') {
            this->skipLine();
            return this->getToken();
        } else if (peek() == '*') {
            this->skipBlockComment();
            return this->getToken();
        }
        return this->returnToken(TokenType::OPERATOR_DIV,
                                 TokenType::OPERATOR_DIV_EQUAL);
    case '%':
        return this->returnToken(TokenType::OPERATOR_MOD,
                                 TokenType::OPERATOR_MOD_EQUAL);

    case '&':
        if (peek() == '&') {
            this->advance();
            return this->returnToken(TokenType::BITWISE_AND_AND,
                                     TokenType::BITWISE_AND_AND_EQUAL);
        }
        return this->returnToken(TokenType::BITWISE_AND,
                                 TokenType::BITWISE_AND_EQUAL);
    case '|':
        if (peek() == '|') {
            this->advance();
            return this->returnToken(TokenType::BITWISE_PIPE_PIPE,
                                     TokenType::BITWISE_PIPE_PIPE_EQUAL);
        }
        return this->returnToken(TokenType::BITWISE_PIPE,
                                 TokenType::BITWISE_PIPE_EQUAL);
    case '^':
        return this->returnToken(TokenType::BITWISE_POWER,
                                 TokenType::BITWISE_POWER_EQUAL);
    case '~':
        return this->returnToken(TokenType::BITWISE_NOT);
    case ':':
        if (peek() == ':') {
            this->advance();
            return this->returnToken(TokenType::DOUBLE_COLON);
        }
        return this->returnToken(TokenType::COLON);
    case ';':
        return this->returnToken(TokenType::SEMICOLON);
    case '(':
        return this->returnToken(TokenType::PARENS_OPEN);
    case ')':
        return this->returnToken(TokenType::PARENS_CLOSE);
    case '[':
        return this->returnToken(TokenType::SQUARE_OPEN);
    case ']':
        return this->returnToken(TokenType::SQUARE_CLOSE);
    case '{':
        return this->returnToken(TokenType::BRACE_OPEN);
    case '}':
        return this->returnToken(TokenType::BRACE_CLOSE);
    case ',':
        return this->returnToken(TokenType::COMMA);
    case '.':
        return this->returnToken(TokenType::PERIOD);
    case '$':
        return this->returnToken(TokenType::DOLLAR);
    case '#':
        return this->returnToken(TokenType::HASHTAG);
    case '@':
        return this->returnToken(TokenType::AT);
    case '\\':
        return this->returnToken(TokenType::BACKSLASH);
    case '\0':
        break;
    default:
        this->throw_error("Unexpected Character `" +
                          std::string({this->current()}) + "`");
    }

    return returnToken(TokenType::T_EOF, true);
}

Token Lexer::identifier() {
    return this->lexWhile(TokenType::IDENTIFIER, [this]() {
        return isalnum(this->current()) || this->current() == '_';
    });
}

Token Lexer::hexadecimal() {
    // 0xFFF -> this function will come from the `number` function, so the start
    // will be 'x' instead of 0
    advance();

    return lexWhile(TokenType::V_HEX,
                    [this]() { return ishexnumber(this->current()); });
}

Token Lexer::octal() {
    // 0o305
    advance();

    return lexWhile(TokenType::V_OCTAL,
                    [this]() { return is_octal(this->current()); });
}

Token Lexer::binary() {
    // 0b0103
    advance();

    return lexWhile(TokenType::V_BINARY,
                    [this]() { return is_binary(this->current()); });
}

Token Lexer::number() {
    this->start = this->index;
    bool is_float = false;

    for (;;) {
        if (this->current() == '.' && !is_float) {
            is_float = true;
            advance();
            if (!isnumber(this->current())) {
                this->throw_error("Expected Literal after decimal");
            }
        }
        if (this->current() == '\0' || !isnumber(this->current())) {
            break;
        }
        advance();
        if (this->current() == 'x' || this->current() == 'X') {
            return hexadecimal();
        }

        if (this->current() == 'o' || this->current() == 'O') {
            return octal();
        }

        if (this->current() == 'b' || this->current() == 'B') {
            return binary();
        }
    }

    return this->returnToken(is_float ? TokenType::V_FLOAT : TokenType::V_INT,
                             true);
}

Token Lexer::string() {
    return this->lexWhile(
        TokenType::V_STRING, [this]() { return (this->current() != '"'); },
        true);
}

Token Lexer::character() {
    return this->lexWhile(
        TokenType::V_CHAR, [this]() { return (this->current() != '\''); },
        true);
}

Token Lexer::returnToken(TokenType type, bool without_advance) {
    auto return_token =
        Token(type, this->start, this->index - this->start, this->location);
    if (!without_advance) {
        this->advance();
        return_token.location.column += 1;
        return_token.length += 1;
    }
    return return_token;
}

Token Lexer::returnToken(TokenType first_type, TokenType second_type) {
    if (this->peek() == '=') {
        this->advance();
        return this->returnToken(second_type);
    } else {
        return this->returnToken(first_type);
    }
}

void Lexer::skipWhitespace() {
    while (isspace(this->current())) {
        if (this->current() == '\n') {
            this->tokens.push_back(returnToken(TokenType::NEW_LINE, true));
            this->location.line += 1;
            this->location.column = 0;
        }
        if (this->current() == '\0') {
            break;
        }
        this->advance();
    }
}

void Lexer::skipLine() {
    while (this->current() != '\n' && this->current() != '\0') {
        this->advance();
    }
}

void Lexer::skipBlockComment() {
    this->advance();
    this->advance();

while_loop:
    while (this->current() != '*') {
        if (this->current() == '\n') {
            this->location.line += 1;
            this->location.column = 0;
        }
        this->advance();

        if (this->current() == '\0') {
            this->throw_error("Unterminated block comment");
        }
    }

    this->advance();
    if (this->current() == '/') {
        this->advance();
        return;
    } else {
        goto while_loop;
    }
}

void Lexer::advance() {
    this->location.column += 1;
    this->index += 1;
    this->current() = this->source[this->index];
}

char Lexer::peek(size_t offset) { return this->source[this->index + offset]; }

template <typename predicate>
Token Lexer::lexWhile(TokenType type, predicate &&pred, bool is_string) {
    this->start = this->index;

    if (is_string) {
        this->advance();
    }

    while (pred()) {
        if (this->current() == '\0') {
            break;
        }
        if (this->current() == '\n') {
            this->location.line += 1;
            this->location.column = 0;
        }
        if (is_string && this->current() == '\\') {
            this->advance();
        }

        this->advance();
    }

    if (is_string) {
        this->advance();
    }

    if (type == TokenType::IDENTIFIER) {
        uint32_t length = this->index - this->start;

        std::string_view value(this->source.data() + this->start, length);

        TokenType type1 = Token::is_keyword(value);
        return this->returnToken(type1, true);
    }

    return this->returnToken(type, true);
}

int Lexer::throw_error(const std::string &message) {
    printer.error(message, this->location);
    return -1;
}
