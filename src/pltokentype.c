#include <PlTokenType>

const char *pl_tokToStr(PlTokenType type)
{
    switch (type)
    {
    case PL_TT_LEFT_PAREN:
        return "(";
    case PL_TT_RIGHT_PAREN:
        return ")";
    case PL_TT_LEFT_BRACKET:
        return "[";
    case PL_TT_RIGHT_BRACKET:
        return "]";
    case PL_TT_LEFT_BRACE:
        return "{";
    case PL_TT_RIGHT_BRACE:
        return "}";
    case PL_TT_COMMA:
        return ",";
    case PL_TT_DOT:
        return ".";
    case PL_TT_MINUS:
        return "-";
    case PL_TT_MINUS_MINUS:
        return "--";
    case PL_TT_MINUS_EQUAL:
        return "-=";
    case PL_TT_PLUS:
        return "+";
    case PL_TT_PLUS_PLUS:
        return "++";
    case PL_TT_PLUS_EQUAL:
        return "+=";
    case PL_TT_COLON:
        return ":";
    case PL_TT_COLON_COLON:
        return "::";
    case PL_TT_SEMICOLON:
        return ";";
    case PL_TT_SLASH:
        return "/";
    case PL_TT_SLASH_EQUAL:
        return "/=";
    case PL_TT_STAR:
        return "*";
    case PL_TT_STAR_EQUAL:
        return "*=";
    case PL_TT_PERCENTAGE:
        return "%";
    case PL_TT_PERCENTAGE_EQUAL:
        return "%=";
    case PL_TT_BANG:
        return "!";
    case PL_TT_BANG_EQUAL:
        return "!=";
    case PL_TT_EQUAL:
        return "=";
    case PL_TT_EQUAL_EQUAL:
        return "==";
    case PL_TT_GREATER:
        return ">";
    case PL_TT_GREATER_EQUAL:
        return ">=";
    case PL_TT_GREATER_GREATER:
        return ">>";
    case PL_TT_GREATER_GREATER_EQUAL:
        return ">>=";
    case PL_TT_LESS:
        return "<";
    case PL_TT_LESS_EQUAL:
        return "<=";
    case PL_TT_LESS_LESS:
        return "<<";
    case PL_TT_LESS_LESS_EQUAL:
        return "<<=";
    case PL_TT_TILDE:
        return "~";
    case PL_TT_AMPERSAND:
        return "&";
    case PL_TT_AMPERSAND_EQUAL:
        return "&=";
    case PL_TT_PIPE:
        return "|";
    case PL_TT_PIPE_EQUAL:
        return "|=";
    case PL_TT_CARET:
        return "^";
    case PL_TT_CARET_EQUAL:
        return "^=";
    case PL_TT_QUESTION:
        return "?";
    case PL_TT_IDENTIFIER:
        return "iden";
    case PL_TT_STRING:
        return "str";
    case PL_TT_NUMBER:
        return "num";
    case PL_TT_AND:
        return "and";
    case PL_TT_BREAK:
        return "break";
    case PL_TT_CASE:
        return "case";
    case PL_TT_CATCH:
        return "catch";
    case PL_TT_CLASS:
        return "class";
    case PL_TT_CONST:
        return "const";
    case PL_TT_CONTINUE:
        return "continue";
    case PL_TT_DECLTYPE:
        return "decltype";
    case PL_TT_DEFAULT:
        return "default";
    case PL_TT_DELETE:
        return "delete";
    case PL_TT_DO:
        return "do";
    case PL_TT_ELSE:
        return "else";
    case PL_TT_FALSE:
        return "false";
    case PL_TT_FINAL:
        return "final";
    case PL_TT_FINALLY:
        return "finally";
    case PL_TT_FOR:
        return "for";
    case PL_TT_FUNC:
        return "func";
    case PL_TT_IF:
        return "if";
    case PL_TT_NOBREAK:
        return "nobreak";
    case PL_TT_NOEXCEPT:
        return "noexcept";
    case PL_TT_NULL:
        return "null";
    case PL_TT_OPERATOR:
        return "operator";
    case PL_TT_OR:
        return "or";
    case PL_TT_PRINT:
        return "print";
    case PL_TT_PRIVATE:
        return "private";
    case PL_TT_PROTECTED:
        return "protected";
    case PL_TT_PUBLIC:
        return "public";
    case PL_TT_RETURN:
        return "return";
    case PL_TT_SIZEOF:
        return "sizeof";
    case PL_TT_STATIC:
        return "static";
    case PL_TT_STD:
        return "std";
    case PL_TT_SUPER:
        return "super";
    case PL_TT_SWITCH:
        return "switch";
    case PL_TT_THIS:
        return "this";
    case PL_TT_THROW:
        return "throw";
    case PL_TT_TRUE:
        return "true";
    case PL_TT_TRY:
        return "try";
    case PL_TT_VAR:
        return "var";
    case PL_TT_WHILE:
        return "while";
    case PL_TT_ERROR:
        return "ERROR";
    case PL_TT_EOF:
        return "EOF";
    default:
        return "unknown";
    }
}
