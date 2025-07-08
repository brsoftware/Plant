#include <stdlib.h>
#include <string.h>

#include <PlLex>
#include <PlLexer>

PlLex lexer;
bool impliedError;
PlToken impliedToken;

static char pl_advance();
static PlTokenType pl_checkKeyword(int start, int length, const char *rest, PlTokenType type);
static PlToken pl_genError(const char *message);
static PlToken pl_genToken(PlTokenType type);
static int pl_hexValue(char c);
static PlTokenType pl_idenType();
static PlToken pl_identifier();
static bool pl_isAlpha(char c);
static bool pl_isAtEnd();
static bool pl_isDigit(char c);
static bool pl_isHex(char c);
static bool pl_match(char expected);
static PlToken pl_numeral();
static char pl_peek();
static char pl_peekNext();
static char pl_procSeq();
static void pl_skipSpace(bool includeNewlines);
static PlToken pl_string(char delimiter);

void pl_initLexer(const char *source)
{
    impliedError = false;

    lexer.start = source;
    lexer.current = source;
    lexer.line = 1;
}

PlToken pl_scanToken()
{
    impliedError = false;

    pl_skipSpace(true);

    if (impliedError)
        return impliedToken;

    lexer.start = lexer.current;

    if (pl_isAtEnd())
        return pl_genToken(PL_TT_EOF);

    char c = pl_advance();

    if (pl_isAlpha(c))
        return pl_identifier();
    if (pl_isDigit(c))
        return pl_numeral();

    switch (c)
    {
    case '\x04':
        exit(0);
    case '(':
        return pl_genToken(PL_TT_LEFT_PAREN);
    case ')':
        return pl_genToken(PL_TT_RIGHT_PAREN);
    case '[':
        return pl_genToken(PL_TT_LEFT_BRACKET);
    case ']':
        return pl_genToken(PL_TT_RIGHT_BRACKET);
    case '{':
        return pl_genToken(PL_TT_LEFT_BRACE);
    case '}':
        return pl_genToken(PL_TT_RIGHT_BRACE);
    case ';':
        return pl_genToken(PL_TT_SEMICOLON);
    case ',':
        return pl_genToken(PL_TT_COMMA);
    case '?':
        return pl_genToken(PL_TT_QUESTION);
    case '.':
        if (pl_isDigit(pl_peekNext()))
            return pl_numeral();
        return pl_genToken(PL_TT_DOT);
    case '~':
        return pl_genToken(PL_TT_TILDE);
    case ':':
        return pl_genToken(
            pl_match(':') ? PL_TT_COLON_COLON : PL_TT_COLON);
    case '-':
        return pl_genToken(
            pl_match('=') ? PL_TT_MINUS_EQUAL : pl_match('-') ? PL_TT_MINUS_MINUS : PL_TT_MINUS);
    case '+':
        return pl_genToken(
            pl_match('=') ? PL_TT_PLUS_EQUAL : pl_match('+') ? PL_TT_PLUS_PLUS : PL_TT_PLUS);
    case '/':
        return pl_genToken(
            pl_match('=') ? PL_TT_SLASH_EQUAL : PL_TT_SLASH);
    case '*':
        return pl_genToken(
            pl_match('=') ? PL_TT_STAR_EQUAL : PL_TT_STAR);
    case '%':
        return pl_genToken(
            pl_match('=') ? PL_TT_PERCENTAGE_EQUAL : PL_TT_PERCENTAGE);
    case '^':
        return pl_genToken(
            pl_match('=') ? PL_TT_CARET_EQUAL : PL_TT_CARET);
    case '&':
        return pl_genToken(
            pl_match('&') ? PL_TT_AND : pl_match('=') ? PL_TT_AMPERSAND_EQUAL : PL_TT_AMPERSAND);
    case '|':
        return pl_genToken(
            pl_match('|') ? PL_TT_OR : pl_match('=') ? PL_TT_PIPE_EQUAL : PL_TT_PIPE);
    case '!':
        return pl_genToken(
            pl_match('=') ? PL_TT_BANG_EQUAL : PL_TT_BANG);
    case '=':
        return pl_genToken(
            pl_match('=') ? PL_TT_EQUAL_EQUAL : PL_TT_EQUAL);
    case '<':
        return pl_genToken(
            pl_match('=') ? PL_TT_LESS_EQUAL :
                pl_match('<') ? (pl_match('=') ? PL_TT_LESS_LESS_EQUAL : PL_TT_LESS_LESS) : PL_TT_LESS);
    case '>':
        return pl_genToken(
            pl_match('=') ? PL_TT_GREATER_EQUAL :
                pl_match('>') ? (pl_match('=') ? PL_TT_GREATER_GREATER_EQUAL : PL_TT_GREATER_GREATER) : PL_TT_GREATER);
    case '\\':
        pl_skipSpace(false);
        if (pl_match('\n'))
            return pl_scanToken();
        return pl_genError("Only newlines could be followed after the escape character.");
    case '"':
        return pl_string('"');
    case '\'':
        return pl_string('\'');
    case '`':
        return pl_string('`');
    default:
        return pl_genError("Invalid token.");
    }

}

static char pl_advance()
{
    lexer.current++;
    return lexer.current[-1];
}

static PlTokenType pl_checkKeyword(int start, int length, const char *rest, PlTokenType type)
{
    if (lexer.current - lexer.start == start + length && memcmp(lexer.start + start, rest, length) == 0)
    {
        return type;
    }

    return PL_TT_IDENTIFIER;
}

static PlToken pl_genError(const char *message)
{
    PlToken token;
    token.type = PL_TT_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = lexer.line;
    token.alloc = false;
    return token;
}

static PlToken pl_genToken(PlTokenType type)
{
    PlToken token;
    token.type = type;
    token.start = lexer.start;
    token.length = (int)(lexer.current - lexer.start);
    token.line = lexer.line;
    token.alloc = false;
    return token;
}

static int pl_hexValue(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 0;
}

static PlTokenType pl_idenType()
{
    switch (lexer.start[0])
    {
    case 'a':
        return pl_checkKeyword(1, 2, "nd", PL_TT_AND);
    case 'b':
        return pl_checkKeyword(1, 4, "reak", PL_TT_BREAK);
    case 'c':
        if (lexer.current - lexer.start > 1)
        {
            switch (lexer.start[1])
            {
            case 'a':
                if (lexer.current - lexer.start > 2)
                {
                    switch (lexer.start[2])
                    {
                    case 's':
                        return pl_checkKeyword(3, 1, "e", PL_TT_CASE);
                    case 't':
                        return pl_checkKeyword(3, 2, "ch", PL_TT_CATCH);
                    default:
                        break;
                    }
                }

                break;
            case 'l':
                return pl_checkKeyword(2, 3, "ass", PL_TT_CLASS);
            case 'o':
                if (lexer.current - lexer.start > 3)
                    return pl_checkKeyword(3, 2, "st", PL_TT_CONST);
                return pl_checkKeyword(2, 6, "ntinue", PL_TT_CONTINUE);
            default:
                break;
            }
        }

        break;
    case 'd':
        if (lexer.current - lexer.start > 1)
        {
            switch (lexer.start[1])
            {
            case 'e':
                if (lexer.current - lexer.start > 2 && lexer.start[2] == 'l')
                    return pl_checkKeyword(3, 3, "ete", PL_TT_DELETE);
                return pl_checkKeyword(2, 5, "fault", PL_TT_DEFAULT);
            case 'o':
                return pl_checkKeyword(2, 0, "o", PL_TT_DO);
            default:
                break;
            }
        }

        break;
    case 'e':
        return pl_checkKeyword(1, 3, "lse", PL_TT_ELSE);
    case 'f':
        if (lexer.current - lexer.start > 1)
        {
            switch (lexer.start[1])
            {
            case 'a':
                return pl_checkKeyword(2, 3, "lse", PL_TT_FALSE);
            case 'i':
                if (lexer.current - lexer.start == 7)
                    return pl_checkKeyword(2, 5, "nally", PL_TT_FINALLY);
                if (lexer.current - lexer.start == 5)
                    return pl_checkKeyword(2, 3, "nal", PL_TT_FINAL);
                return PL_TT_IDENTIFIER;
            case 'o':
                return pl_checkKeyword(2, 1, "r", PL_TT_FOR);
            case 'u':
                return pl_checkKeyword(2, 2, "nc", PL_TT_FUNC);
            default:
                break;
            }
        }

        break;
    case 'i':
        if (lexer.current - lexer.start > 1)
        {
            switch (lexer.start[1])
            {
            case 'f':
                return pl_checkKeyword(2, 0, "f", PL_TT_IF);
            case 's':
                if (lexer.current - lexer.start > 2 && lexer.start[2] == 'n')
                    return pl_checkKeyword(3, 1, "t", PL_TT_BANG_EQUAL);
                return pl_checkKeyword(2, 0, "s", PL_TT_EQUAL_EQUAL);
            default:
                break;
            }
        }

        break;
    case 'n':
        if (lexer.current - lexer.start > 1)
        {
            switch (lexer.start[1])
            {
            case 'o':
                if (lexer.current - lexer.start > 2)
                {
                    switch (lexer.start[2])
                    {
                    case 'b':
                        return pl_checkKeyword(3, 4, "reak", PL_TT_NOBREAK);
                    case 'e':
                        return pl_checkKeyword(3, 5, "xcept", PL_TT_NOEXCEPT);
                    case 't':
                        return pl_checkKeyword(3, 0, "t", PL_TT_BANG);
                    default:
                        break;
                    }
                }

                break;
            case 'u':
                return pl_checkKeyword(2, 2, "ll", PL_TT_NULL);
            default:
                break;
            }
        }

        break;
    case 'o':
        if (lexer.current - lexer.start > 1)
        {
            switch (lexer.start[1])
            {
            case 'p':
                return pl_checkKeyword(2, 6, "erator", PL_TT_OPERATOR);
            case 'r':
                return pl_checkKeyword(2, 0, "r", PL_TT_OR);
            default:
                break;
            }
        }

        break;
    case 'p':
        if (lexer.current - lexer.start > 1)
        {
            switch (lexer.start[1])
            {
            case 'r':
                if (lexer.current - lexer.start > 2)
                {
                    switch (lexer.start[2])
                    {
                    case 'i':
                        if (lexer.current - lexer.start > 3)
                        {
                            switch (lexer.start[3])
                            {
                            case 'n':
                                return pl_checkKeyword(4, 1, "t", PL_TT_PRINT);
                            case 'v':
                                return pl_checkKeyword(4, 3, "ate", PL_TT_PRIVATE);
                            default:
                                break;
                            }
                        }

                        break;
                    case 'o':
                        return pl_checkKeyword(3, 6, "tected", PL_TT_PROTECTED);
                    default:
                        break;
                    }
                }

                break;
            case 'u':
                return pl_checkKeyword(2, 4, "blic", PL_TT_PUBLIC);
            default:
                break;
            }
        }

        break;
    case 'r':
        return pl_checkKeyword(1, 5, "eturn", PL_TT_RETURN);
    case 's':
        if (lexer.current - lexer.start > 1)
        {
            switch (lexer.start[1])
            {
            case 'i':
                return pl_checkKeyword(2, 4, "zeof", PL_TT_SIZEOF);
            case 't':
                return pl_checkKeyword(2, 4, "atic", PL_TT_STATIC);
            case 'u':
                return pl_checkKeyword(2, 3, "per", PL_TT_SUPER);
            case 'w':
                return pl_checkKeyword(2, 4, "itch", PL_TT_SWITCH);
            default:
                break;
            }
        }

        break;
    case 't':
        if (lexer.current - lexer.start > 1)
        {
            switch (lexer.start[1])
            {
            case 'h':
                if (lexer.current - lexer.start > 2)
                {
                    switch (lexer.start[2])
                    {
                    case 'i':
                        return pl_checkKeyword(3, 1, "s", PL_TT_THIS);
                    case 'r':
                        return pl_checkKeyword(3, 2, "ow", PL_TT_THROW);
                    default:
                        break;
                    }
                }

                break;
            case 'r':
                if (lexer.current - lexer.start > 2)
                {
                    switch (lexer.start[2])
                    {
                    case 'u':
                        return pl_checkKeyword(3, 1, "e", PL_TT_TRUE);
                    case 'y':
                        return pl_checkKeyword(3, 0, "y", PL_TT_TRY);
                    default:
                        break;
                    }
                }

                break;
            default:
                break;
            }
        }

        break;
    case 'v':
        return pl_checkKeyword(1, 2, "ar", PL_TT_VAR);
    case 'w':
        return pl_checkKeyword(1, 4, "hile", PL_TT_WHILE);
    default:
        break;
    }

    return PL_TT_IDENTIFIER;
}

static PlToken pl_identifier()
{
    while (pl_isAlpha(pl_peek()) || pl_isDigit(pl_peek()))
        pl_advance();

    return pl_genToken(pl_idenType());
}

static bool pl_isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool pl_isAtEnd()
{
    return *lexer.current == '\0';
}

static bool pl_isDigit(char c)
{
    return c >= '0' && c <= '9';
}

static bool pl_isHex(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static bool pl_match(char expected)
{
    if (pl_isAtEnd())
        return false;
    if (*lexer.current != expected)
        return false;

    lexer.current++;
    return true;
}

static PlToken pl_numeral()
{
    char *buffer = malloc(256);
    if (!buffer)
        return pl_genError("Memory allocation failed.");

    int capac = 256;
    int len = 0;

    if (lexer.start[0] == '.')
    {
        buffer[len++] = '.';
    }

    else
    {
        buffer[len++] = lexer.start[0];
    }

    while (!pl_isAtEnd())
    {
        char c = pl_peek();

        if (c == '\'')
        {
            pl_advance();
            continue;
        }

        if (pl_isHex(c) || c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-' ||
            c == 'o' || c == 'O' || c == 'x' || c == 'X')
        {
            if (len >= capac - 1)
            {
                capac *= 2;
                char *newBuffer = realloc(buffer, capac);

                if (!newBuffer)
                {
                    free(buffer);
                    return pl_genError("Memory allocation failed.");
                }

                buffer = newBuffer;
            }

            buffer[len++] = c;
            pl_advance();
        }

        else
        {
            break;
        }
    }

    buffer[len] = '\0';

    PlToken token;
    token.type = PL_TT_NUMBER;
    token.start = buffer;
    token.length = len;
    token.line = lexer.line;
    token.alloc = true;
    return token;
}

static char pl_peek()
{
    return *lexer.current;
}

static char pl_peekNext()
{
    if (pl_isAtEnd())
        return '\0';
    return lexer.current[1];
}

static char pl_procSeq()
{
    if (pl_isAtEnd())
        return '\\';

    char c = pl_advance();
    switch (c)
    {
    case 'a':
        return '\a';
    case 'b':
        return '\b';
    case 'f':
        return '\f';
    case 'n':
        return '\n';
    case 'r':
        return '\r';
    case 't':
        return '\t';
    case 'v':
        return '\v';
    case '\\':
        return '\\';
    case '\'':
        return '\'';
    case '"':
        return '"';
    case ' ':
        return ' ';

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7': {
        int octal = c - '0';

        for (int index = 0; index < 2; index++)
        {
            if (pl_peek() >= '0' && pl_peek() <= '7')
            {
                octal = (octal << 3) + (pl_advance() - '0');
            }

            else
            {
                break;
            }
        }

        if (octal > 255)
        {
            impliedError = true;
            impliedToken = pl_genError("Out of range for octal escape sequence: cannot larger than \\377.");
            return 0;
        }

        return (char)octal;
    }

    case 'x': {
        if (!pl_isHex(pl_peek()))
        {
            impliedError = true;
            impliedToken = pl_genError("Invalid hexadecimal digit.");
        }

        int hex = 0;

        for (int index = 0; index < 2; index++)
        {
            if (pl_isHex(pl_peek()))
            {
                char digit = pl_advance();
                hex = (hex << 4) + pl_hexValue(digit);
            }

            else
            {
                impliedError = true;
                impliedToken = pl_genError("Incomplete hexadecimal escape sequence.");
            }
        }

        return (char)hex;
    }

    case 'u':
    case 'U': {
        int length = (c == 'u') ? 4 : 8;
        uint32_t code = 0;

        for (int index = 0; index < length; index++)
        {
            if (!pl_isHex(pl_peek()))
            {
                impliedError = true;
                impliedToken = pl_genError("Incomplete Unicode escape sequence.");
                return 0;
            }

            char digit = pl_advance();
            code = (code << 4) + pl_hexValue(digit);
        }

        if (code <= 0x7f)
        {
            return (char)code;
        }

        impliedError = true;
        impliedToken = pl_genError("Unicode escape sequence is not fully implemented.");
        return 0;
    }

    case '\n':
        lexer.line++;
        return 0;
    default:
        impliedError = true;
        impliedToken = pl_genError("Invalid escape sequence.");
        return 0;
    }
}

static void pl_skipSpace(bool includeNewlines)
{
    for (;;)
    {
        char c = pl_peek();

        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            pl_advance();
            break;
        case '\n':
            if (includeNewlines)
            {
                lexer.line++;
                pl_advance();
                break;
            }
            return;

        case '/': {
            if (pl_peekNext() == '/')
            {
                while (pl_peek() != '\n' && !pl_isAtEnd())
                {
                    if (pl_peek() == '\\')
                    {
                        pl_advance();

                        if (pl_peek() == '\n')
                            pl_advance();
                    }

                    pl_advance();
                }
            }

            else if (pl_peekNext() == '*')
            {
                pl_advance();
                pl_advance();

                bool commentClosed = false;

                while (!pl_isAtEnd())
                {
                    if (pl_peek() == '*' && pl_peekNext() == '/')
                    {
                        pl_advance();
                        pl_advance();
                        commentClosed = true;
                        break;
                    }

                    pl_advance();
                }

                while (pl_peek() <= ' ' && !pl_isAtEnd())
                    pl_advance();

                if (!commentClosed)
                {
                    impliedError = true;
                    impliedToken = pl_genError("Unterminated comment.");
                }

                return;
            }

            else
            {
                if (pl_peek() <= ' ')
                    break;
                return;
            }

            break;
        }

        default:
            return;
        }
    }
}

static PlToken pl_string(char delimiter)
{
    char *buffer = malloc(256);
    if (!buffer)
        return pl_genError("Memory allocation failed.");

    int capac = 256;
    int len = 0;

    while (pl_peek() != delimiter && !pl_isAtEnd())
    {
        if (len >= capac - 1)
        {
            capac *= 2;
            char *newBuffer = realloc(buffer, capac);

            if (!newBuffer)
            {
                free(buffer);
                return pl_genError("Memory allocation failed.");
            }

            buffer = newBuffer;
        }

        char c = pl_advance();

        if (c == '\\' && delimiter != '`')
        {
            char escape = pl_procSeq();

            if (impliedError)
            {
                free(buffer);
                return impliedToken;
            }

            if (escape == 0 && pl_peek() == '\n')
            {
                lexer.line++;
                pl_advance();
                continue;
            }

            if (escape != 0)
            {
                buffer[len++] = escape;
            }
        }

        else
        {
            if (c == '\n')
                lexer.line++;
            buffer[len++] = c;
        }
    }

    if (pl_isAtEnd())
    {
        free(buffer);
        return pl_genError("Unterminated string.");
    }

    pl_advance();
    buffer[len] = '\0';

    PlToken token = pl_genToken(PL_TT_STRING);
    token.start = buffer;
    token.length = len;
    token.alloc = true;
    return token;
}
