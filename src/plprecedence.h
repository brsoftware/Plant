#ifndef PLPRECEDENCE_H
#define PLPRECEDENCE_H

typedef enum PlPrecedence
{
    PL_PREC_NONE,
    PL_PREC_ASSIGNMENT,  // = += -= *= /= %= &= |= ^= ~= <<= >>=
    PL_PREC_CONDITIONAL, // ?:
    PL_PREC_OR,          // ||
    PL_PREC_AND,         // &&
    PL_PREC_BITOR,       // |
    PL_PREC_BITXOR,      // ^
    PL_PREC_BITAND,      // &
    PL_PREC_EQUALITY,    // == !=
    PL_PREC_COMPARISON,  // < > <= >=
    PL_PREC_SHIFT,       // << >>
    PL_PREC_TERM,        // + -
    PL_PREC_FACTOR,      // * / %
    PL_PREC_UNARY,       // ! - + ~ ++(prefix) --(prefix)
    PL_PREC_CALL,        // . () ++(suffix) --(suffix) :: []
    PL_PREC_PRIMARY
} PlPrecedence;

#endif // PLPRECEDENCE_H
