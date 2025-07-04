#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <PlBreaks>
#include <PlClsComp>
#include <PlComp>
#include <PlCompiler>
#include <PlMemory>
#include <PlLexer>
#include <PlLocal>
#include <PlParseRule>
#include <PlParser>

#define PL_MAX_CASES 256

#define PL_COMPOUND_ASSIGNMENT(opCode, getWhat, setWhat, indexAt)             \
    if (indexAt <= UINT8_MAX)                                                 \
    {                                                                         \
        pl_emit(getWhat);                                                     \
        pl_emit(indexAt);                                                     \
        pl_expr();                                                            \
        pl_emit(opCode);                                                      \
        pl_emit(setWhat);                                                     \
        pl_emit(indexAt);                                                     \
    }                                                                         \
    else                                                                      \
    {                                                                         \
        pl_emit(getWhat##_LONG);                                              \
        pl_emit((uint8_t)(indexAt & 0xff));                                   \
        pl_emit((uint8_t)((indexAt >> 8) & 0xff));                            \
        pl_emit((uint8_t)((indexAt >> 16) & 0xff));                           \
        pl_expr();                                                            \
        pl_emit(opCode);                                                      \
        pl_emit(setWhat);                                                     \
        pl_emit((uint8_t)(indexAt & 0xff));                                   \
        pl_emit((uint8_t)((indexAt >> 8) & 0xff));                            \
        pl_emit((uint8_t)((indexAt >> 16) & 0xff));                           \
    }

#define PL_COMPOUND_ASSIGNMENT_PROP(opCode, getWhat, setWhat, indexAt)        \
    pl_emit(PL_DUPLICATE);                                                    \
    if (indexAt <= UINT8_MAX)                                                 \
    {                                                                         \
        pl_emit(getWhat);                                                     \
        pl_emit(indexAt);                                                     \
        pl_expr();                                                            \
        pl_emit(opCode);                                                      \
        pl_emit(setWhat);                                                     \
        pl_emit(indexAt);                                                     \
    }                                                                         \
    else                                                                      \
    {                                                                         \
        pl_emit(getWhat##_LONG);                                              \
        pl_emit((uint8_t)(indexAt & 0xff));                                   \
        pl_emit((uint8_t)((indexAt >> 8) & 0xff));                            \
        pl_emit((uint8_t)((indexAt >> 16) & 0xff));                           \
        pl_expr();                                                            \
        pl_emit(opCode);                                                      \
        pl_emit(setWhat);                                                     \
        pl_emit((uint8_t)(indexAt & 0xff));                                   \
        pl_emit((uint8_t)((indexAt >> 8) & 0xff));                            \
        pl_emit((uint8_t)((indexAt >> 16) & 0xff));                           \
    }

PlParser parser;
PlComp *cur = NULL;
PlClsComp *curCls = NULL;
PlBreaks *curBreak = NULL;

int innermostLoopStart = -1;
int innermostLoopScopeDepth = 0;

static void pl_addLocal(PlToken name);
static int pl_addSurvalue(PlComp *comp, uint8_t index, bool isLocal);
static void pl_advance();
static void pl_and(bool assignable);
static uint8_t pl_argList();
static void pl_beginScope();
static void pl_binary(bool assignable);
static void pl_block(const char *msg);
static void pl_bracket(bool assignable);
static void pl_breakStmt();
static void pl_call(bool assignable);
static bool pl_check(PlTokenType type);
static void pl_classDecl();
static void pl_conditional(bool assignable);
static void pl_continueStmt();
static void pl_consume(PlTokenType type, const char *msg);
static PlSegment *pl_curSeg();
static void pl_decl();
static void pl_declVar();
static void pl_defVar(int global);
static void pl_doStmt();
static void pl_dot(bool assignable);
static void pl_emit(uint8_t byte);
static void pl_emit2(uint8_t first, uint8_t second);
static void pl_emitConstant(PlValue value);
static int pl_emitJump(uint8_t instr);
static void pl_emitLoop(int loopStart);
static void pl_emitReturn();
static PlFunction *pl_endCompiler();
static void pl_endScope();
static void pl_error(const char *msg);
static void pl_errorAt(PlToken *token, const char *msg);
static void pl_errorCurrent(const char *msg);
static void pl_expr();
static void pl_exprStmt();
static void pl_forStmt();
static void pl_funcDecl();
static PlFunction *pl_function(PlFuncType type);
static PlParseRule *pl_getRule(PlTokenType type);
static void pl_globalResolve(bool assignable);
static void pl_group(bool assignable);
static void pl_initComp(PlComp *comp, PlFuncType type);
static int pl_idenConst(PlToken *name);
static bool pl_idenEq(PlToken *first, PlToken *second);
static void pl_ifStmt();
static void pl_increment(bool assignable);
static void pl_literal(bool assignable);
static int pl_makeConstant(PlValue value);
static void pl_map(bool assignable);
static void pl_markInit();
static bool pl_match(PlTokenType type);
static void pl_method(bool op);
static bool pl_namedVar(PlToken name, bool assignable, bool forceGlobal, bool forceAssign);
static void pl_numeral(bool assignable);
static void pl_or(bool assignable);
static void pl_parsePrecedence(PlPrecedence precedence);
static int pl_parseVar(const char *msg);
static void pl_patchJump(int offset);
static void pl_printStmt();
static int pl_resolveLocal(PlComp *comp, PlToken *name);
static int pl_resolveSurvalue(PlComp *comp, PlToken *name);
static void pl_returnStmt();
static void pl_sizeof(bool assignable);
static void pl_stmt();
static void pl_string(bool assignable);
static void pl_subscript(bool assignable);
static void pl_super(bool assignable);
static void pl_switchStmt();
static void pl_sync();
static PlToken pl_synthToken(const char *text);
static void pl_this(bool assignable);
static void pl_unary(bool assignable);
static void pl_var(bool assignable);
static void pl_varDecl();
static void pl_whileStmt();

PlParseRule rules[] = {
    [PL_TT_LEFT_PAREN]            = {pl_group,         pl_call,        PL_PREC_CALL},
    [PL_TT_RIGHT_PAREN]           = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_LEFT_BRACKET]          = {pl_bracket,       pl_subscript,   PL_PREC_CALL},
    [PL_TT_RIGHT_BRACKET]         = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_LEFT_BRACE]            = {pl_map,           NULL,           PL_PREC_NONE},
    [PL_TT_RIGHT_BRACE]           = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_COMMA]                 = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_DOT]                   = {NULL,             pl_dot,         PL_PREC_CALL},
    [PL_TT_MINUS]                 = {pl_unary,         pl_binary,      PL_PREC_TERM},
    [PL_TT_MINUS_MINUS]           = {pl_unary,         pl_increment,   PL_PREC_UNARY},
    [PL_TT_MINUS_EQUAL]           = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_PLUS]                  = {pl_unary,         pl_binary,      PL_PREC_TERM},
    [PL_TT_PLUS_PLUS]             = {pl_unary,         pl_increment,   PL_PREC_UNARY},
    [PL_TT_PLUS_EQUAL]            = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_COLON]                 = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_COLON_COLON]           = {pl_globalResolve, NULL,           PL_PREC_NONE},
    [PL_TT_SEMICOLON]             = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_SLASH]                 = {NULL,             pl_binary,      PL_PREC_FACTOR},
    [PL_TT_SLASH_EQUAL]           = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_STAR]                  = {NULL,             pl_binary,      PL_PREC_FACTOR},
    [PL_TT_STAR_EQUAL]            = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_PERCENTAGE]            = {NULL,             pl_binary,      PL_PREC_FACTOR},
    [PL_TT_PERCENTAGE_EQUAL]      = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_BANG]                  = {pl_unary,         NULL,           PL_PREC_NONE},
    [PL_TT_BANG_EQUAL]            = {NULL,             pl_binary,      PL_PREC_EQUALITY},
    [PL_TT_EQUAL]                 = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_EQUAL_EQUAL]           = {NULL,             pl_binary,      PL_PREC_EQUALITY},
    [PL_TT_GREATER]               = {NULL,             pl_binary,      PL_PREC_COMPARISON},
    [PL_TT_GREATER_EQUAL]         = {NULL,             pl_binary,      PL_PREC_COMPARISON},
    [PL_TT_GREATER_GREATER]       = {NULL,             pl_binary,      PL_PREC_SHIFT},
    [PL_TT_GREATER_GREATER_EQUAL] = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_LESS]                  = {NULL,             pl_binary,      PL_PREC_COMPARISON},
    [PL_TT_LESS_EQUAL]            = {NULL,             pl_binary,      PL_PREC_COMPARISON},
    [PL_TT_LESS_LESS]             = {NULL,             pl_binary,      PL_PREC_SHIFT},
    [PL_TT_LESS_LESS_EQUAL]       = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_TILDE]                 = {pl_unary,         NULL,           PL_PREC_NONE},
    [PL_TT_AMPERSAND]             = {NULL,             pl_binary,      PL_PREC_BITAND},
    [PL_TT_AMPERSAND_EQUAL]       = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_PIPE]                  = {NULL,             pl_binary,      PL_PREC_BITOR},
    [PL_TT_PIPE_EQUAL]            = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_CARET]                 = {NULL,             pl_binary,      PL_PREC_BITXOR},
    [PL_TT_CARET_EQUAL]           = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_QUESTION]              = {NULL,             pl_conditional, PL_PREC_CONDITIONAL},
    [PL_TT_IDENTIFIER]            = {pl_var,           NULL,           PL_PREC_NONE},
    [PL_TT_STRING]                = {pl_string,        NULL,           PL_PREC_NONE},
    [PL_TT_NUMBER]                = {pl_numeral,       NULL,           PL_PREC_NONE},
    [PL_TT_AND]                   = {NULL,             pl_and,         PL_PREC_AND},
    [PL_TT_BREAK]                 = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_CASE]                  = {NULL,             NULL,           PL_PREC_NONE},
    // [PL_TT_CATCH]                 = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_CLASS]                 = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_CONTINUE]              = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_DEFAULT]               = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_DO]                    = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_ELSE]                  = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_FALSE]                 = {pl_literal,       NULL,           PL_PREC_NONE},
    // [PL_TT_FINAL]                 = {NULL,             NULL,           PL_PREC_NONE},
    // [PL_TT_FINALLY]               = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_FOR]                   = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_FUNC]                  = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_IF]                    = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_NOBREAK]               = {NULL,             NULL,           PL_PREC_NONE},
    // [PL_TT_NOEXCEPT]              = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_NULL]                  = {pl_literal,       NULL,           PL_PREC_NONE},
    [PL_TT_OR]                    = {NULL,             pl_or,          PL_PREC_OR},
    [PL_TT_OPERATOR]              = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_PRINT]                 = {NULL,             NULL,           PL_PREC_NONE},
    // [PL_TT_PRIVATE]               = {NULL,             NULL,           PL_PREC_NONE},
    // [PL_TT_PROTECTED]             = {NULL,             NULL,           PL_PREC_NONE},
    // [PL_TT_PUBLIC]                = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_RETURN]                = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_SIZEOF]                = {pl_sizeof,        NULL,           PL_PREC_NONE},
    // [PL_TT_STATIC]                = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_SUPER]                 = {pl_super,         NULL,           PL_PREC_NONE},
    [PL_TT_SWITCH]                = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_THIS]                  = {pl_this,          NULL,           PL_PREC_NONE},
    // [PL_TT_THROW]                 = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_TRUE]                  = {pl_literal,       NULL,           PL_PREC_NONE},
    // [PL_TT_TRY]                   = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_VAR]                   = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_WHILE]                 = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_ERROR]                 = {NULL,             NULL,           PL_PREC_NONE},
    [PL_TT_EOF]                   = {NULL,             NULL,           PL_PREC_NONE},
};

PlFunction *pl_compile(const char *source)
{
    pl_initLexer(source);
    PlComp comp;
    pl_initComp(&comp, PL_FUNCTYPE_SCRIPT);

    parser.error = false;
    parser.panic = false;

    pl_advance();

    while (!pl_match(PL_TT_EOF))
    {
        if (parser.prev.alloc)
        {
            free((void*)parser.prev.start);
        }

        pl_decl();
    }

    PlFunction *function = pl_endCompiler();
    return parser.error ? NULL : function;
}

void pl_markCompilerRoots()
{
    PlComp *comp = cur;

    while (comp != NULL)
    {
        pl_markObject((PlObject*)comp->func);
        comp = comp->parent;
    }
}

static void pl_addLocal(PlToken name)
{
    if (cur->localCount == UINT8_COUNT * 2)
    {
        pl_error("Too many local variables in function.");
        return;
    }

    PlLocal *local = &cur->locals[cur->localCount++];
    local->name = name;
    local->depth = -1;
    local->captured = false;
}

static int pl_addSurvalue(PlComp *comp, uint8_t index, bool isLocal)
{
    int survalueCount = comp->func->survalueCount;

    for (int idx = 0; idx < survalueCount; idx++)
    {
        PlSurval *surval = &comp->survalues[idx];

        if (surval->index == index && surval->isLocal == isLocal)
        {
            return idx;
        }
    }

    if (survalueCount == UINT8_COUNT * 2)
    {
        pl_error("Too many closure variables in function.");
        return 0;
    }

    comp->survalues[survalueCount].isLocal = isLocal;
    comp->survalues[survalueCount].index = index;
    return comp->func->survalueCount++;
}

static void pl_advance()
{
    parser.prev2 = parser.prev;
    parser.prev = parser.cur;

    for (;;)
    {
        parser.cur = pl_scanToken();

        if (parser.cur.type != PL_TT_ERROR)
            break;

        pl_errorCurrent(parser.cur.start);
    }
}

static void pl_and(bool assignable)
{
    int endJump = pl_emitJump(PL_JUMP_IF_FALSE);

    pl_emit(PL_POP);
    pl_parsePrecedence(PL_PREC_AND);

    pl_patchJump(endJump);
}

static uint8_t pl_argList()
{
    uint8_t argCount = 0;

    if (!pl_check(PL_TT_RIGHT_PAREN))
    {
        do
        {
            if (pl_check(PL_TT_RIGHT_PAREN))
                break;

            pl_expr();

            if (argCount == 255)
            {
                pl_error("Can't have more than 255 arguments.");
            }

            argCount++;
        } while (pl_match(PL_TT_COMMA));
    }

    pl_consume(PL_TT_RIGHT_PAREN, "Expect ')' after arguments.");
    return argCount;
}

static void pl_beginScope()
{
    cur->scopeDepth++;
}

static void pl_binary(bool assignable)
{
    PlTokenType operatorType = parser.prev.type;
    PlParseRule *rule = pl_getRule(operatorType);
    pl_parsePrecedence((PlPrecedence)(rule->precedence + 1));

    switch (operatorType)
    {
    case PL_TT_BANG_EQUAL:
        pl_emit2(PL_EQUAL, PL_NOT);
        break;
    case PL_TT_EQUAL_EQUAL:
        pl_emit(PL_EQUAL);
        break;
    case PL_TT_GREATER:
        pl_emit(PL_GREATER);
        break;
    case PL_TT_GREATER_EQUAL:
        pl_emit2(PL_LESS, PL_NOT);
        break;
    case PL_TT_GREATER_GREATER:
        pl_emit(PL_RSHIFT);
        break;
    case PL_TT_LESS:
        pl_emit(PL_LESS);
        break;
    case PL_TT_LESS_EQUAL:
        pl_emit2(PL_GREATER, PL_NOT);
        break;
    case PL_TT_LESS_LESS:
        pl_emit(PL_LSHIFT);
        break;
    case PL_TT_PLUS:
        pl_emit(PL_ADD);
        break;
    case PL_TT_MINUS:
        pl_emit(PL_SUBTRACT);
        break;
    case PL_TT_STAR:
        pl_emit(PL_MULTIPLY);
        break;
    case PL_TT_SLASH:
        pl_emit(PL_DIVIDE);
        break;
    case PL_TT_PERCENTAGE:
        pl_emit(PL_MODULO);
        break;
    case PL_TT_AMPERSAND:
        pl_emit(PL_BITAND);
        break;
    case PL_TT_PIPE:
        pl_emit(PL_BITOR);
        break;
    case PL_TT_CARET:
        pl_emit(PL_BITXOR);
        break;
    default:
        return; // Unreachable.
    }
}

static void pl_block(const char *msg)
{
    while (!pl_check(PL_TT_RIGHT_BRACE) && !pl_check(PL_TT_EOF))
    {
        pl_decl();
    }

    pl_consume(PL_TT_RIGHT_BRACE, msg);
}

static void pl_bracket(bool assignable)
{
    if (pl_check(PL_TT_EQUAL))
    {
        pl_advance();
        pl_consume(PL_TT_RIGHT_BRACKET, "Expect ']' after lambda declaration.");
        pl_function(PL_FUNCTYPE_LAMBDA);
        return;
    }

    int initSize = 0;

    if (!pl_check(PL_TT_RIGHT_BRACKET))
    {
        do
        {
            if (pl_check(PL_TT_RIGHT_BRACKET))
                break;

            initSize++;

            if (initSize > 255)
            {
                pl_errorCurrent("Can't have more than 255 items.");
            }

            pl_expr();
        } while (pl_match(PL_TT_COMMA));
    }

    pl_consume(PL_TT_RIGHT_BRACKET, "Expect ']' after vector.");
    pl_emit2(PL_VECTOR, initSize);
}

static void pl_breakStmt()
{
    if (curBreak == NULL)
    {
        pl_error("Cannot use 'break' outside of a loop.");
        return;
    }

    pl_consume(PL_TT_SEMICOLON, "Expect ';' after 'break'.");

    int jump = pl_emitJump(PL_JUMP);

    if (curBreak->count >= curBreak->capacity)
    {
        int newCapacity = curBreak->capacity == 0 ? 4 : curBreak->capacity * 2;
        int *newBreaks = realloc(curBreak->breaks, newCapacity * sizeof(int));

        if (!newBreaks)
        {
            pl_error("Memory allocation failed.");
            return;
        }

        curBreak->breaks = newBreaks;
        curBreak->capacity = newCapacity;
    }

    curBreak->breaks[curBreak->count++] = jump;
}

static void pl_call(bool assignable)
{
    uint8_t argCount = pl_argList();
    pl_emit2(PL_CALL, argCount);
}

static bool pl_check(PlTokenType type)
{
  return parser.cur.type == type;
}

static void pl_classDecl()
{
    pl_consume(PL_TT_IDENTIFIER, "Expect class name.");
    PlToken className = parser.prev;
    int nameConstant = pl_idenConst(&parser.prev);
    pl_declVar();

    if (nameConstant <= UINT8_MAX)
    {
        pl_emit2(PL_CLASS, nameConstant);
    }

    else
    {
        pl_emit(PL_CLASS_LONG);
        pl_emit((uint8_t)(nameConstant & 0xff));
        pl_emit((uint8_t)((nameConstant >> 8) & 0xff));
        pl_emit((uint8_t)((nameConstant >> 16) & 0xff));
    }

    pl_defVar(nameConstant);

    PlClsComp classCompiler;
    classCompiler.hasSuper = false;
    classCompiler.parent = curCls;
    curCls = &classCompiler;

    if (pl_match(PL_TT_COLON))
    {
        pl_consume(PL_TT_IDENTIFIER, "Expect superclass name.");
        pl_var(false);

        if (pl_idenEq(&className, &parser.prev))
        {
            pl_error("A class can't inherit from itself.");
        }

        pl_beginScope();
        pl_addLocal(pl_synthToken("super"));
        pl_defVar(0);

        pl_namedVar(className, false, false, false);
        pl_emit(PL_INHERIT);
        classCompiler.hasSuper = true;
    }

    pl_namedVar(className, false, false, false);

    bool hasSemi = false;

    if (pl_match(PL_TT_SEMICOLON))
        hasSemi = true;

    if (!hasSemi)
        pl_consume(PL_TT_LEFT_BRACE, "Expect '{' before class body.");

    while (!pl_check(PL_TT_RIGHT_BRACE) && !pl_check(PL_TT_EOF) && !parser.panic && !hasSemi)
    {
        if (pl_match(PL_TT_FUNC))
        {
            pl_method(false);
        }

        else if (pl_match(PL_TT_OPERATOR))
        {
            pl_method(true);
        }

        else
        {
            pl_errorCurrent("Expect method.");
        }
    }

    if (!hasSemi)
        pl_consume(PL_TT_RIGHT_BRACE, "Expect '}' after class body.");

    pl_emit(PL_POP);

    if (classCompiler.hasSuper)
    {
        pl_endScope();
    }

    curCls = curCls->parent;
}

static void pl_conditional(bool assignable)
{
    int thenBranch = pl_emitJump(PL_JUMP_IF_FALSE);
    pl_emit(PL_POP);
    pl_parsePrecedence(PL_PREC_CONDITIONAL);
    pl_consume(PL_TT_COLON, "Expect ':'.");
    int thenEndJump = pl_emitJump(PL_JUMP);
    pl_patchJump(thenBranch);
    pl_emit(PL_POP);
    pl_parsePrecedence(PL_PREC_ASSIGNMENT);
    pl_patchJump(thenEndJump);
}

static void pl_continueStmt()
{
    if (innermostLoopStart == -1)
    {
        pl_error("'continue' statement may only be used within a loop.");
        return;
    }

    pl_consume(PL_TT_SEMICOLON, "Expect ';' after 'continue'.");

    for (int index = cur->localCount - 1; index >= 0 && cur->locals[index].depth > innermostLoopScopeDepth; index--)
    {
        pl_emit(PL_POP);
    }

    pl_emitLoop(innermostLoopStart);
}

static void pl_consume(PlTokenType type, const char *msg)
{
    if (parser.cur.type == type)
    {
        pl_advance();
        return;
    }

    pl_errorCurrent(msg);
}

static PlSegment *pl_curSeg()
{
    return &cur->func->segment;
}

static void pl_decl()
{
    if (pl_match(PL_TT_CLASS))
    {
        pl_classDecl();
    }

    else if (pl_match(PL_TT_FUNC))
    {
        pl_funcDecl();
    }

    else if (pl_match(PL_TT_VAR))
    {
        pl_varDecl();
    }

    else
    {
        pl_stmt();
    }

    if (parser.panic)
        pl_sync();
}

static void pl_declVar()
{
    if (cur->scopeDepth == 0)
        return;

    PlToken *name = &parser.prev;

    for (int index = cur->localCount - 1; index >= 0; index--)
    {
        PlLocal *local = &cur->locals[index];

        if (local->depth != -1 && local->depth < cur->scopeDepth)
        {
            break;
        }

        if (pl_idenEq(name, &local->name))
        {
            pl_error("Already a variable with this name in this scope.");
        }
    }

    pl_addLocal(*name);
}

static void pl_defVar(int global)
{
    if (cur->scopeDepth > 0)
    {
        pl_markInit();
        return;
    }

    if (global <= UINT8_MAX)
    {
        pl_emit2(PL_DEFINE_GLOBAL, global);
    }

    else
    {
        pl_emit(PL_DEFINE_GLOBAL_LONG);
        pl_emit((uint8_t)(global & 0xff));
        pl_emit((uint8_t)((global >> 8) & 0xff));
        pl_emit((uint8_t)((global >> 16) & 0xff));
    }
}

static void pl_doStmt()
{
    PlBreaks *surroundingBreak = curBreak;

    PlBreaks newBreak;
    newBreak.parent = surroundingBreak;
    newBreak.breaks = NULL;
    newBreak.count = 0;
    newBreak.capacity = 0;
    curBreak = &newBreak;

    int surroundingLoopStart = innermostLoopStart;
    int surroundingLoopScopeDepth = innermostLoopScopeDepth;
    innermostLoopStart = pl_curSeg()->count;
    innermostLoopScopeDepth = cur->scopeDepth;

    if (pl_match(PL_TT_LEFT_BRACE))
    {
        pl_block("Expect '}' after 'do' statements.");
    }

    else
    {
        pl_stmt();
    }

    pl_consume(PL_TT_WHILE, "Expect 'while' after '}'.");
    pl_consume(PL_TT_LEFT_PAREN, "Expect '(' after 'while'.");
    pl_expr();
    pl_consume(PL_TT_RIGHT_PAREN, "Expect ')' after condition.");

    int exitJump = pl_emitJump(PL_JUMP_IF_FALSE);
    pl_emit(PL_POP);
    pl_emitLoop(innermostLoopStart);

    pl_patchJump(exitJump);
    pl_emit(PL_POP);

    if (pl_match(PL_TT_NOBREAK))
    {
        if (pl_match(PL_TT_LEFT_BRACE))
        {
            pl_block("Expect '}' after 'nobreak' statements of 'do'.");
        }

        else
        {
            pl_stmt();
        }
    }

    else
    {
        pl_consume(PL_TT_SEMICOLON, "Expect ';' after condition.");
    }

    for (int index = 0; index < curBreak->count; index++)
    {
        pl_patchJump(curBreak->breaks[index]);
    }

    if (curBreak->breaks)
    {
        free(curBreak->breaks);
    }

    curBreak = surroundingBreak;

    innermostLoopStart = surroundingLoopStart;
    innermostLoopScopeDepth = surroundingLoopScopeDepth;
}

static void pl_dot(bool assignable)
{
    pl_consume(PL_TT_IDENTIFIER, "Expect property name after '.'.");
    int name = pl_idenConst(&parser.prev);

    if (assignable && pl_match(PL_TT_EQUAL))
    {
        pl_expr();

        if (name <= UINT8_MAX)
        {
            pl_emit2(PL_SET_PROPERTY, name);
        }

        else
        {
            pl_emit(PL_SET_PROPERTY_LONG);
            pl_emit((uint8_t)(name & 0xff));
            pl_emit((uint8_t)((name >> 8) & 0xff));
            pl_emit((uint8_t)((name >> 16) & 0xff));
        }
    }

    else if (assignable && pl_match(PL_TT_MINUS_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT_PROP(PL_SUBTRACT, PL_GET_PROPERTY, PL_SET_PROPERTY, name);
    }

    else if (assignable && pl_match(PL_TT_PLUS_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT_PROP(PL_ADD, PL_GET_PROPERTY, PL_SET_PROPERTY, name);
    }

    else if (assignable && pl_match(PL_TT_SLASH_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT_PROP(PL_DIVIDE, PL_GET_PROPERTY, PL_SET_PROPERTY, name);
    }

    else if (assignable && pl_match(PL_TT_STAR_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT_PROP(PL_MULTIPLY, PL_GET_PROPERTY, PL_SET_PROPERTY, name);
    }

    else if (assignable && pl_match(PL_TT_PERCENTAGE_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT_PROP(PL_MODULO, PL_GET_PROPERTY, PL_SET_PROPERTY, name);
    }

    else if (assignable && pl_match(PL_TT_GREATER_GREATER_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT_PROP(PL_RSHIFT, PL_GET_PROPERTY, PL_SET_PROPERTY, name);
    }

    else if (assignable && pl_match(PL_TT_LESS_LESS_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT_PROP(PL_LSHIFT, PL_GET_PROPERTY, PL_SET_PROPERTY, name);
    }

    else if (assignable && pl_match(PL_TT_CARET_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT_PROP(PL_BITXOR, PL_GET_PROPERTY, PL_SET_PROPERTY, name);
    }

    else if (assignable && pl_match(PL_TT_AMPERSAND_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT_PROP(PL_BITAND, PL_GET_PROPERTY, PL_SET_PROPERTY, name);
    }

    else if (assignable && pl_match(PL_TT_PIPE_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT_PROP(PL_BITOR, PL_GET_PROPERTY, PL_SET_PROPERTY, name);
    }

    else if (pl_match(PL_TT_LEFT_PAREN))
    {
        uint8_t argCount = pl_argList();

        if (name <= UINT8_MAX)
        {
            pl_emit2(PL_INVOKE, name);
        }

        else
        {
            pl_emit(PL_INVOKE_LONG);
            pl_emit((uint8_t)(name & 0xff));
            pl_emit((uint8_t)((name >> 8) & 0xff));
            pl_emit((uint8_t)((name >> 16) & 0xff));
        }

        pl_emit(argCount);
    }

    else
    {
        if (name <= UINT8_MAX)
        {
            pl_emit2(PL_GET_PROPERTY, name);
        }

        else
        {
            pl_emit(PL_GET_PROPERTY_LONG);
            pl_emit((uint8_t)(name & 0xff));
            pl_emit((uint8_t)((name >> 8) & 0xff));
            pl_emit((uint8_t)((name >> 16) & 0xff));
        }
    }
}

static void pl_emit(uint8_t byte)
{
    pl_writeSegment(pl_curSeg(), byte, parser.prev.line);
}

static void pl_emit2(uint8_t first, uint8_t second)
{
    pl_emit(first);
    pl_emit(second);
}

static void pl_emitConstant(PlValue value)
{
    int constant = pl_makeConstant(value);

    if (constant <= UINT8_MAX)
    {
        pl_emit2(PL_CONSTANT, constant);
    }

    else
    {
        pl_emit(PL_CONSTANT_LONG);
        pl_emit((uint8_t)(constant & 0xff));
        pl_emit((uint8_t)((constant >> 8) & 0xff));
        pl_emit((uint8_t)((constant >> 16) & 0xff));
    }
}

static int pl_emitJump(uint8_t instr)
{
    pl_emit(instr);
    pl_emit(0xff);
    pl_emit(0xff);
    return pl_curSeg()->count - 2;
}

static void pl_emitLoop(int loopStart)
{
    pl_emit(PL_LOOP);

    int offset = pl_curSeg()->count - loopStart + 2;
    if (offset > UINT16_MAX)
        pl_error("Loop body too large.");

    pl_emit((offset >> 8) & 0xff);
    pl_emit(offset & 0xff);
}

static void pl_emitReturn()
{
    if (cur->type == PL_FUNCTYPE_INIT)
    {
        pl_emit2(PL_GET_LOCAL, 0);
    }

    else
    {
        pl_emit(PL_NULL);
    }

    pl_emit(PL_RETURN);
}

static PlFunction *pl_endCompiler()
{
    pl_emitReturn();
    PlFunction *function = cur->func;
    cur = cur->parent;
    return function;
}

static void pl_endScope()
{
    cur->scopeDepth--;

    while (cur->localCount > 0 && cur->locals[cur->localCount - 1].depth > cur->scopeDepth)
    {
        if (cur->locals[cur->localCount - 1].captured)
        {
            pl_emit(PL_CLOSE_SURVALUE);
        }

        else
        {
            pl_emit(PL_POP);
        }

        cur->localCount--;
    }
}

static void pl_error(const char *msg)
{
    pl_errorAt(&parser.prev, msg);
}

static void pl_errorAt(PlToken *token, const char *msg) {
    if (parser.panic)
      return;

    parser.panic = true;
    fprintf(stderr, "  Line %d\nSyntax Error", token->line);

    if (token->type == PL_TT_EOF)
    {
        fprintf(stderr, " at end");
    }

    else if (token->type == PL_TT_ERROR)
    {
        ;
    }

    else
    {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", msg);
    parser.error = true;
}

static void pl_errorCurrent(const char *msg)
{
    pl_errorAt(&parser.cur, msg);
}

static void pl_expr()
{
    pl_parsePrecedence(PL_PREC_ASSIGNMENT);
}

static void pl_exprStmt()
{
    pl_expr();
    pl_consume(PL_TT_SEMICOLON, "Expect ';' after expression.");
    pl_emit(PL_POP);
}

static void pl_forStmt()
{
    PlBreaks *surroundingBreak = curBreak;

    PlBreaks newBreak;
    newBreak.parent = surroundingBreak;
    newBreak.breaks = NULL;
    newBreak.count = 0;
    newBreak.capacity = 0;

    curBreak = &newBreak;

    pl_beginScope();
    pl_consume(PL_TT_LEFT_PAREN, "Expect '(' after 'for'.");

    if (pl_match(PL_TT_SEMICOLON))
    {
        ; // No initializer.
    }

    else if (pl_match(PL_TT_VAR))
    {
        pl_varDecl();
    }

    else
    {
        pl_exprStmt();
    }

    int surroundingLoopStart = innermostLoopStart;
    int surroundingLoopScopeDepth = innermostLoopScopeDepth;
    innermostLoopStart = pl_curSeg()->count;
    innermostLoopScopeDepth = cur->scopeDepth;

    int exitJump = -1;

    if (!pl_match(PL_TT_SEMICOLON))
    {
        pl_expr();
        pl_consume(PL_TT_SEMICOLON, "Expect ';' after loop condition.");

        exitJump = pl_emitJump(PL_JUMP_IF_FALSE);
        pl_emit(PL_POP);
    }

    if (!pl_match(PL_TT_RIGHT_PAREN))
    {
        int bodyJump = pl_emitJump(PL_JUMP);
        int incrementStart = pl_curSeg()->count;
        pl_expr();
        pl_emit(PL_POP);
        pl_consume(PL_TT_RIGHT_PAREN, "Expect ')' after 'for' clauses.");

        pl_emitLoop(innermostLoopStart);
        innermostLoopStart = incrementStart;
        pl_patchJump(bodyJump);
    }

    pl_stmt();
    pl_emitLoop(innermostLoopStart);

    if (exitJump != -1)
    {
        pl_patchJump(exitJump);
        pl_emit(PL_POP);
    }

    if (pl_match(PL_TT_NOBREAK))
    {
        if (pl_match(PL_TT_LEFT_BRACE))
        {
            pl_block("Expect '}' after 'nobreak' statements of 'for'.");
        }

        else
        {
            pl_stmt();
        }
    }

    for (int index = 0; index < curBreak->count; index++)
    {
        pl_patchJump(curBreak->breaks[index]);
    }

    if (curBreak->breaks)
    {
        free(curBreak->breaks);
    }

    curBreak = surroundingBreak;

    innermostLoopStart = surroundingLoopStart;
    innermostLoopScopeDepth = surroundingLoopScopeDepth;

    pl_endScope();
}

static void pl_funcDecl()
{
    uint8_t global = pl_parseVar("Expect function name.");
    pl_markInit();
    pl_function(PL_FUNCTYPE_FUNC);
    pl_defVar(global);
}

static PlFunction *pl_function(PlFuncType type)
{
    PlComp comp;
    pl_initComp(&comp, type);
    pl_beginScope();

    pl_consume(PL_TT_LEFT_PAREN, "Expect '(' after function name.");

    if (!pl_check(PL_TT_RIGHT_PAREN))
    {
        do
        {
            if (pl_check(PL_TT_RIGHT_PAREN))
                break;

            cur->func->arity++;

            if (cur->func->arity > 255)
            {
                pl_errorCurrent("Can't have more than 255 parameters.");
            }

            int constant = pl_parseVar("Expect parameter name.");
            pl_defVar(constant);
        } while (pl_match(PL_TT_COMMA));
    }

    pl_consume(PL_TT_RIGHT_PAREN, "Expect ')' after parameters.");

    if (pl_match(PL_TT_LEFT_BRACE))
        // pl_consume(PL_TT_LEFT_BRACE, "Expect '{' before function body.");
        pl_block("Expect '}' after function body.");
    else if (pl_match(PL_TT_SEMICOLON))
        ; // ...
    else
        pl_errorCurrent("Expect ';' or '{' after function body.");

    PlFunction *fn = pl_endCompiler();
    int funcIndex = pl_makeConstant(PL_OBJECT_VALUE(fn));

    if (funcIndex <= UINT8_MAX)
    {
        pl_emit2(PL_CLOSURE, funcIndex);
    }

    else
    {
        pl_emit(PL_CLOSURE_LONG);
        pl_emit((uint8_t)(funcIndex & 0xff));
        pl_emit((uint8_t)((funcIndex >> 8) & 0xff));
        pl_emit((uint8_t)((funcIndex >> 16) & 0xff));
    }

    for (int index = 0; index < fn->survalueCount; index++)
    {
        pl_emit(comp.survalues[index].isLocal ? 1 : 0);
        pl_emit(comp.survalues[index].index);
    }

    return fn;
}

static PlParseRule *pl_getRule(PlTokenType type)
{
    return &rules[type];
}

static void pl_globalResolve(bool assignable)
{
    pl_consume(PL_TT_IDENTIFIER, "Expect global variable name.");
    pl_namedVar(parser.prev, assignable, true, false);
}

static void pl_group(bool assignable)
{
    pl_expr();
    pl_consume(PL_TT_RIGHT_PAREN, "Expect ')' after expression.");
}

static void pl_initComp(PlComp *comp, PlFuncType type)
{
    comp->parent = cur;
    comp->func = NULL;
    comp->type = type;
    comp->localCount = 0;
    comp->scopeDepth = 0;
    comp->func = pl_newFunction();
    cur = comp;

    if (type != PL_FUNCTYPE_SCRIPT)
    {
        cur->func->name = pl_copyString(parser.prev.start, parser.prev.length);
    }

    PlLocal *local = &cur->locals[cur->localCount++];
    local->depth = 0;
    local->captured = false;

    if (type != PL_FUNCTYPE_FUNC && type != PL_FUNCTYPE_LAMBDA)
    {
        local->name.start = "this";
        local->name.length = 4;
    }

    else
    {
        local->name.start = "";
        local->name.length = 0;
    }
}

static int pl_idenConst(PlToken *name)
{
    return pl_makeConstant(PL_OBJECT_VALUE(pl_copyString(name->start, name->length)));
}

static bool pl_idenEq(PlToken *first, PlToken *second)
{
    if (first->length != second->length)
        return false;

    return memcmp(first->start, second->start, second->length) == 0;
}

static void pl_ifStmt()
{
    pl_consume(PL_TT_LEFT_PAREN, "Expect '(' after 'if'.");
    pl_expr();
    pl_consume(PL_TT_RIGHT_PAREN, "Expect ')' after condition."); // [paren]

    int thenJump = pl_emitJump(PL_JUMP_IF_FALSE);
    pl_emit(PL_POP);

    if (pl_match(PL_TT_LEFT_BRACE))
    {
        pl_block("Expect '}' after 'if' statements.");
    }

    else
    {
        pl_stmt();
    }

    int elseJump = pl_emitJump(PL_JUMP);

    pl_patchJump(thenJump);
    pl_emit(PL_POP);

    if (pl_match(PL_TT_ELSE))
    {
        if (pl_match(PL_TT_LEFT_BRACE))
        {
            pl_block("Expect '}' after 'else' statements.");
        }

        else
        {
            pl_stmt();
        }
    }

    pl_patchJump(elseJump);
}

static void pl_increment(bool assignable)
{
    switch (parser.prev.type)
    {
    case PL_TT_MINUS_MINUS:
        pl_emit2(PL_DUPLICATE, PL_DEC_1);
        pl_namedVar(parser.prev2, true, false, true);
        pl_emit(PL_POP);
        break;
    case PL_TT_PLUS_PLUS:
        pl_emit2(PL_DUPLICATE, PL_ADD_1);
        pl_namedVar(parser.prev2, true, false, true);
        pl_emit(PL_POP);
        break;
    default:
        return; // Unreachable.
    }
}

static void pl_literal(bool assignable)
{
    switch (parser.prev.type)
    {
    case PL_TT_FALSE:
        pl_emit(PL_FALSE);
        break;
    case PL_TT_NULL:
        pl_emit(PL_NULL);
        break;
    case PL_TT_TRUE:
        pl_emit(PL_TRUE);
        break;
    default:
        return; // Unreachable.
    }
}

static int pl_makeConstant(PlValue value)
{
    int constant = pl_addConstant(pl_curSeg(), value);

    if (constant > UINT32_MAX)
    {
        pl_error("Too many constants in one chunk.");
        return 0;
    }

    return constant;
}

static void pl_map(bool assignable)
{
    int initSize = 0;

    if (!pl_check(PL_TT_RIGHT_BRACE))
    {
        do
        {
            if (pl_check(PL_TT_RIGHT_BRACE))
                break;

            initSize++;

            if (initSize > 255)
            {
                pl_errorCurrent("Can't have more than 255 pairs.");
            }

            pl_expr();
            pl_consume(PL_TT_COLON, "Expect ':' after key.");
            pl_expr();
        } while (pl_match(PL_TT_COMMA));
    }

    pl_consume(PL_TT_RIGHT_BRACE, "Expect '}' after map.");
    pl_emit2(PL_MAP, initSize);
}

static void pl_markInit()
{
    if (cur->scopeDepth == 0)
        return;
    cur->locals[cur->localCount - 1].depth = cur->scopeDepth;
}

static bool pl_match(PlTokenType type)
{
    if (!pl_check(type))
        return false;

    pl_advance();
    return true;
}

static void pl_method(bool op)
{
    if (!op)
        pl_consume(PL_TT_IDENTIFIER, "Expect method name.");

    const char *opSymb = NULL;

    if (op)
    {
        if (pl_match(PL_TT_PLUS))
        {
            opSymb = "+";
        }

        else if (pl_match(PL_TT_MINUS))
        {
            opSymb = "-";
        }

        else if (pl_match(PL_TT_STAR))
        {
            opSymb = "*";
        }

        else if (pl_match(PL_TT_SLASH))
        {
            opSymb = "/";
        }

        else if (pl_match(PL_TT_PERCENTAGE))
        {
            opSymb = "%";
        }

        else if (pl_match(PL_TT_AMPERSAND))
        {
            opSymb = "&";
        }

        else if (pl_match(PL_TT_PIPE))
        {
            opSymb = "|";
        }

        else if (pl_match(PL_TT_CARET))
        {
            opSymb = "^";
        }

        else if (pl_match(PL_TT_TILDE))
        {
            opSymb = "~";
        }

        else if (pl_match(PL_TT_LESS_LESS))
        {
            opSymb = "<<";
        }

        else if (pl_match(PL_TT_GREATER_GREATER))
        {
            opSymb = ">>";
        }

        else if (pl_match(PL_TT_MINUS_MINUS))
        {
            opSymb = "--";
        }

        else if (pl_match(PL_TT_PLUS_PLUS))
        {
            opSymb = "++";
        }

        else if (pl_match(PL_TT_GREATER))
        {
            opSymb = ">";
        }

        else if (pl_match(PL_TT_LESS))
        {
            opSymb = "<";
        }

        else if (pl_match(PL_TT_EQUAL_EQUAL))
        {
            opSymb = "==";
        }

        else
        {
            if (pl_match(PL_TT_IDENTIFIER))
            {
                if (parser.prev.length == 4 && memcmp(parser.prev.start, "bool", 4) == 0)
                {
                    opSymb = "bool";
                }
            }

            pl_errorCurrent("Invalid operator overloading symbol.");
        }
    }

    int constant = pl_idenConst(&parser.prev);
    PlFuncType type = PL_FUNCTYPE_METHOD;

    if (parser.prev.length == 4 && memcmp(parser.prev.start, "init", 4) == 0)
    {
        type = PL_FUNCTYPE_INIT;
    }

    PlFunction *func = pl_function(type);

    if (opSymb != NULL)
    {
        if (func->name)
        {
            PL_FREE(PlString, cur->func->name);
        }

        func->name = pl_copyString(opSymb, (int)strlen(opSymb));

        int expectedArgs;

        if (strcmp(opSymb, "~") == 0 || strcmp(opSymb, "--") == 0 || strcmp(opSymb, "++") == 0)
            expectedArgs = 0;
        else
            expectedArgs = 1;

        if (func->arity != expectedArgs)
        {
            pl_error("Invalid number of parameter for operating overloading method.");
        }
    }

    if (constant <= UINT8_MAX)
    {
        pl_emit2(PL_METHOD, constant);
    }

    else
    {
        pl_emit(PL_METHOD_LONG);
        pl_emit((uint8_t)(constant & 0xff));
        pl_emit((uint8_t)((constant >> 8) & 0xff));
        pl_emit((uint8_t)((constant >> 16) & 0xff));
    }
}

static bool pl_namedVar(PlToken name, bool assignable, bool forceGlobal, bool forceAssign)
{
    uint8_t getOp, setOp, getOp_LONG, setOp_LONG;
    int arg = pl_resolveLocal(cur, &name);
    bool limit512 = false;
    bool setVar = false;

    if (arg != -1 && !forceGlobal)
    {
        getOp = PL_GET_LOCAL;
        setOp = PL_SET_LOCAL;
        getOp_LONG = PL_GET_LOCAL_LONG;
        setOp_LONG = PL_SET_LOCAL_LONG;
        limit512 = true;
    }

    else if ((arg = pl_resolveSurvalue(cur, &name)) != -1 && !forceGlobal)
    {
        getOp = PL_GET_SURVALUE;
        setOp = PL_SET_SURVALUE;
        getOp_LONG = PL_GET_SURVALUE_LONG;
        setOp_LONG = PL_SET_SURVALUE_LONG;
        limit512 = true;
    }

    else
    {
        arg = pl_idenConst(&name);
        getOp = PL_GET_GLOBAL;
        setOp = PL_SET_GLOBAL;
        getOp_LONG = PL_GET_GLOBAL_LONG;
        setOp_LONG = PL_SET_GLOBAL_LONG;
    }

    if (assignable && pl_match(PL_TT_EQUAL) || forceAssign)
    {
        if (!forceAssign)
            pl_expr();

        if (arg <= UINT8_MAX)
        {
            pl_emit2(setOp, arg);
        }

        else
        {
            pl_emit(setOp_LONG);
            pl_emit((uint8_t)(arg & 0xff));
            pl_emit((uint8_t)((arg >> 8) & 0xff));
            pl_emit((uint8_t)((arg >> 16) & 0xff));
        }

        setVar = true;
    }

    else if (assignable && pl_match(PL_TT_MINUS_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT(PL_SUBTRACT, getOp, setOp, arg);
        setVar = true;
    }

    else if (assignable && pl_match(PL_TT_PLUS_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT(PL_ADD, getOp, setOp, arg);
        setVar = true;
    }

    else if (assignable && pl_match(PL_TT_SLASH_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT(PL_DIVIDE, getOp, setOp, arg);
        setVar = true;
    }

    else if (assignable && pl_match(PL_TT_STAR_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT(PL_MULTIPLY, getOp, setOp, arg);
        setVar = true;
    }

    else if (assignable && pl_match(PL_TT_PERCENTAGE_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT(PL_MODULO, getOp, setOp, arg);
        setVar = true;
    }

    else if (assignable && pl_match(PL_TT_GREATER_GREATER_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT(PL_RSHIFT, getOp, setOp, arg);
        setVar = true;
    }

    else if (assignable && pl_match(PL_TT_LESS_LESS_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT(PL_LSHIFT, getOp, setOp, arg);
        setVar = true;
    }

    else if (assignable && pl_match(PL_TT_CARET_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT(PL_BITXOR, getOp, setOp, arg);
        setVar = true;
    }

    else if (assignable && pl_match(PL_TT_AMPERSAND_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT(PL_BITAND, getOp, setOp, arg);
        setVar = true;
    }

    else if (assignable && pl_match(PL_TT_PIPE_EQUAL))
    {
        PL_COMPOUND_ASSIGNMENT(PL_BITOR, getOp, setOp, arg);
        setVar = true;
    }

    else
    {
        if (arg <= UINT8_MAX)
        {
            pl_emit2(getOp, arg);
        }

        else
        {
            pl_emit(getOp_LONG);
            pl_emit((uint8_t)(arg & 0xff));
            pl_emit((uint8_t)((arg >> 8) & 0xff));
            pl_emit((uint8_t)((arg >> 16) & 0xff));
        }
    }

    if (limit512 && arg > 512)
    {
        pl_error("Too many local variables, maximum 512 allowed.");
    }

    return setVar;
}

static void pl_numeral(bool assignable)
{
    char *numStr = pl_strndup(parser.prev.start, parser.prev.length);

    if (!numStr)
    {
        pl_error("Memory allocation failed.");
        return;
    }

    double value;
    char *endPtr = numStr;

    if (numStr[0] == '0' && (numStr[1] == 'x' || numStr[1] == 'X'))
        value = (double)strtoull(numStr + 2, &endPtr, 16);
    else if (numStr[0] == '0' && (numStr[1] == 'b' || numStr[1] == 'B'))
        value = (double)strtoull(numStr + 2, &endPtr, 2);
    else if (numStr[0] == '0' && (numStr[1] == 'o' || numStr[1] == 'O'))
        value = (double) strtoull(numStr + 2, &endPtr, 8);
    else
        value = strtod(numStr, &endPtr);

    if (endPtr != numStr + parser.prev.length)
    {
        pl_error("Invalid number format.");
        free(numStr);
        return;
    }

    free(numStr);

    if (parser.prev.alloc)
    {
        free((void*)parser.prev.start);
    }

    pl_emitConstant(PL_NUMERAL_VALUE(value));
}

static void pl_or(bool assignable)
{
    int elseJump = pl_emitJump(PL_JUMP_IF_FALSE);
    int endJump = pl_emitJump(PL_JUMP);

    pl_patchJump(elseJump);
    pl_emit(PL_POP);

    pl_parsePrecedence(PL_PREC_OR);
    pl_patchJump(endJump);
}

static void pl_parsePrecedence(PlPrecedence precedence)
{
    pl_advance();
    PlParseFunc prefix = pl_getRule(parser.prev.type)->prefix;

    if (prefix == NULL)
    {
        pl_error("Expect expression.");
        return;
    }

    bool canAssign = precedence <= PL_PREC_ASSIGNMENT;
    prefix(canAssign);

    while (precedence <= pl_getRule(parser.cur.type)->precedence)
    {
        pl_advance();
        PlParseFunc infix = pl_getRule(parser.prev.type)->infix;
        infix(canAssign);
    }

    if (canAssign && (pl_match(PL_TT_EQUAL) || pl_match(PL_TT_MINUS_EQUAL) || pl_match(PL_TT_PLUS_EQUAL)
        || pl_match(PL_TT_SLASH_EQUAL) || pl_match(PL_TT_STAR_EQUAL) || pl_match(PL_TT_PERCENTAGE_EQUAL)
        || pl_match(PL_TT_AMPERSAND_EQUAL) || pl_match(PL_TT_PIPE_EQUAL) || pl_match(PL_TT_CARET_EQUAL)
        || pl_match(PL_TT_GREATER_GREATER_EQUAL) || pl_match(PL_TT_LESS_LESS_EQUAL)))
    {
        pl_error("Cannot assign to rvalue.");
    }
}

static int pl_parseVar(const char *msg)
{
    pl_consume(PL_TT_IDENTIFIER, msg);
    pl_declVar();

    if (cur->scopeDepth > 0)
        return 0;

    return pl_idenConst(&parser.prev);
}

static void pl_patchJump(int offset)
{
    int jump = pl_curSeg()->count - offset - 2;

    if (jump > UINT16_MAX)
    {
        pl_error("Too much code to jump over.");
    }

    pl_curSeg()->code[offset] = (jump >> 8) & 0xff;
    pl_curSeg()->code[offset + 1] = jump & 0xff;
}

static void pl_printStmt()
{
    if (pl_match(PL_TT_SEMICOLON))
    {
        pl_emit(PL_PRINT_EMPTY);
        return;
    }

    pl_expr();
    pl_consume(PL_TT_SEMICOLON, "Expect ';' after value.");
    pl_emit(PL_PRINT);
}

static int pl_resolveLocal(PlComp *comp, PlToken *name)
{
    for (int index = comp->localCount - 1; index >= 0; index--)
    {
        PlLocal *local = &comp->locals[index];
        if (pl_idenEq(name, &local->name))
        {
            if (local->depth == -1)
            {
                pl_error("Can't read local variable in its own initializer.");
            }

            return index;
        }
    }

    return -1;
}

static int pl_resolveSurvalue(PlComp *comp, PlToken *name)
{
    if (comp->parent == NULL)
        return -1;

    int local = pl_resolveLocal(comp->parent, name);

    if (local != -1)
    {
        comp->parent->locals[local].captured = true;
        return pl_addSurvalue(comp, (uint8_t)local, true);
    }

    int survalue = pl_resolveSurvalue(comp->parent, name);

    if (survalue != -1)
    {
        return pl_addSurvalue(comp, (uint8_t)survalue, false);
    }

    return -1;
}

static void pl_returnStmt()
{
    if (cur->type == PL_FUNCTYPE_SCRIPT)
    {
        pl_error("Can't return from top-level code.");
    }

    if (pl_match(PL_TT_SEMICOLON))
    {
        pl_emitReturn();
    }

    else
    {
        if (cur->type == PL_FUNCTYPE_INIT)
        {
            pl_error("Can't return a value from an initializer.");
        }

        pl_expr();
        pl_consume(PL_TT_SEMICOLON, "Expect ';' after return value.");
        pl_emit(PL_RETURN);
    }
}

static void pl_sizeof(bool assignable)
{
    pl_consume(PL_TT_LEFT_PAREN, "Expect '(' after 'sizeof'.");
    pl_expr();
    pl_consume(PL_TT_RIGHT_PAREN, "Expect ')' after 'sizeof'.");
    pl_emit(PL_SIZEOF);
}

static void pl_stmt()
{
    if (pl_match(PL_TT_SEMICOLON))
    {
        return;
    }

    if (pl_match(PL_TT_PRINT))
    {
        pl_printStmt();
    }

    else if (pl_match(PL_TT_BREAK))
    {
        pl_breakStmt();
    }

    else if (pl_match(PL_TT_CONTINUE))
    {
        pl_continueStmt();
    }

    else if (pl_match(PL_TT_DO))
    {
        pl_doStmt();
    }

    else if (pl_match(PL_TT_FOR))
    {
        pl_forStmt();
    }

    else if (pl_match(PL_TT_IF))
    {
        pl_ifStmt();
    }

    else if (pl_match(PL_TT_RETURN))
    {
        pl_returnStmt();
    }

    else if (pl_match(PL_TT_SWITCH))
    {
        pl_switchStmt();
    }

    else if (pl_match(PL_TT_WHILE))
    {
        pl_whileStmt();
    }

    else if (pl_match(PL_TT_LEFT_BRACE))
    {
        pl_beginScope();
        pl_block("Expect '}' after a block.");
        pl_endScope();
    }

    else
    {
        pl_exprStmt();
    }
}

static void pl_string(bool assignable)
{
    char *comb = NULL;
    size_t combLen = 0;

    do
    {
        char *strContent = pl_strndup(parser.prev.start, parser.prev.length);

        if (!strContent)
        {
            pl_error("Memory allocation failed.");
            return;
        }

        char *newComb = realloc(comb, combLen + parser.prev.length + 1);

        if (!newComb)
        {
            free(strContent);
            free(comb);
            pl_error("Memory allocation failed.");
            return;
        }

        comb = newComb;
        memcpy(comb + combLen, strContent, parser.prev.length);
        combLen += parser.prev.length;
        free(strContent);

        if (parser.prev.alloc)
        {
            free((void*)parser.prev.start);
        }
    } while (pl_match(PL_TT_STRING));

    PlString *string = pl_copyString(comb, (int)combLen);
    free(comb);

    pl_emitConstant(PL_OBJECT_VALUE(string));
}

static void pl_subscript(bool assignable)
{
#define INDEX_COMP(opCode)                                                    \
    pl_emit2(PL_DUPLICATE_PREV, PL_DUPLICATE_PREV);                           \
    pl_emit(PL_GET_INDEX);                                                    \
    pl_expr();                                                                \
    pl_emit(opCode);                                                          \
    pl_emit(PL_SET_INDEX);                                                    \
    return;                                                                   \

    if (pl_match(PL_TT_RIGHT_BRACKET))
    {
        pl_emit(PL_NULL);
        return;
    }

    pl_expr();
    pl_consume(PL_TT_RIGHT_BRACKET, "Expect ']' after subscript.");

    if (pl_match(PL_TT_EQUAL))
    {
        pl_expr();
        pl_emit(PL_SET_INDEX);
        return;
    }

    if (pl_match(PL_TT_MINUS_EQUAL))
    {
        INDEX_COMP(PL_SUBTRACT);
    }

    if (pl_match(PL_TT_PLUS_EQUAL))
    {
        INDEX_COMP(PL_ADD);
    }

    if (pl_match(PL_TT_SLASH_EQUAL))
    {
        INDEX_COMP(PL_DIVIDE);
    }

    if (pl_match(PL_TT_STAR_EQUAL))
    {
        INDEX_COMP(PL_MULTIPLY);
    }

    if (pl_match(PL_TT_PERCENTAGE_EQUAL))
    {
        INDEX_COMP(PL_MODULO);
    }

    if (pl_match(PL_TT_GREATER_GREATER_EQUAL))
    {
        INDEX_COMP(PL_RSHIFT);
    }

    if (pl_match(PL_TT_LESS_LESS_EQUAL))
    {
        INDEX_COMP(PL_LSHIFT);
    }

    if (pl_match(PL_TT_CARET_EQUAL))
    {
        INDEX_COMP(PL_BITXOR);
    }

    if (pl_match(PL_TT_AMPERSAND_EQUAL))
    {
        INDEX_COMP(PL_BITAND);
    }

    if (pl_match(PL_TT_PIPE_EQUAL))
    {
        INDEX_COMP(PL_BITOR);
    }

    pl_emit(PL_GET_INDEX);

#undef INDEX_COMP
}

static void pl_super(bool assignable)
{
    if (curCls == NULL)
    {
        pl_error("Can't use 'super' outside of a class.");
    }

    else if (!curCls->hasSuper)
    {
        pl_error("Can't use 'super' in a class with no superclass.");
    }

    pl_consume(PL_TT_DOT, "Expect '.' after 'super'.");
    pl_consume(PL_TT_IDENTIFIER, "Expect superclass method name.");
    int name = pl_idenConst(&parser.prev);

    pl_namedVar(pl_synthToken("this"), false, false, false);

    if (pl_match(PL_TT_LEFT_PAREN))
    {
        uint8_t argCount = pl_argList();
        pl_namedVar(pl_synthToken("super"), false, false, false);

        if (name <= UINT8_MAX)
        {
            pl_emit2(PL_SUPER_INVOKE, name);
        }

        else
        {
            pl_emit(PL_SUPER_INVOKE_LONG);
            pl_emit((uint8_t)(name & 0xff));
            pl_emit((uint8_t)((name >> 8) & 0xff));
            pl_emit((uint8_t)((name >> 16) & 0xff));
        }

        pl_emit(argCount);
    }

    else
    {
        pl_namedVar(pl_synthToken("super"), false, false, false);
        pl_emit2(PL_GET_SUPER, name);
    }
}

static void pl_switchStmt()
{
    pl_consume(PL_TT_LEFT_PAREN, "Expect '(' after 'switch'.");
    pl_expr();
    pl_consume(PL_TT_RIGHT_PAREN, "Expect ')' after value.");
    pl_consume(PL_TT_LEFT_BRACE, "Expect '{' before switch cases.");

    typedef enum PlSwitchState
    {
        BEFORE_ALL = 0,
        BEFORE_DEFAULT,
        AFTER_DEFAULT
    } PlSwitchState;

    PlSwitchState state = BEFORE_ALL;
    int caseEnds[PL_MAX_CASES];
    int caseCount = 0;
    int previousCaseSkip = -1;

    while (!pl_match(PL_TT_RIGHT_BRACE))
    {
        if (pl_check(PL_TT_EOF))
        {
            pl_errorCurrent("Expect '}' after 'switch'.");
            return;
        }

        if (pl_match(PL_TT_CASE) || pl_match(PL_TT_DEFAULT))
        {
            PlTokenType caseType = parser.prev.type;

            if (state == AFTER_DEFAULT)
            {
                pl_error("'default' makes remaining patterns unreachable.");
            }

            if (state == BEFORE_DEFAULT)
            {
                caseEnds[caseCount++] = pl_emitJump(PL_JUMP);
                pl_patchJump(previousCaseSkip);
                pl_emit(PL_POP);
            }

            if (caseType == PL_TT_CASE)
            {
                state = BEFORE_DEFAULT;

                pl_emit(PL_DUPLICATE);
                pl_expr();

                pl_consume(PL_TT_COLON, "Expect ':' after case value.");

                pl_emit(PL_TT_EQUAL);
                previousCaseSkip = pl_emitJump(PL_JUMP_IF_FALSE);

                pl_emit(PL_POP);
            }

            else
            {
                state = AFTER_DEFAULT;
                pl_consume(PL_TT_COLON, "Expect ':' after default.");
                previousCaseSkip = -1;
            }
        }

        else
        {
            if (state == BEFORE_ALL)
            {
                pl_error("Can't have statements before any case.");
            }

            pl_stmt();
        }
    }

    if (state == BEFORE_DEFAULT)
    {
        pl_patchJump(previousCaseSkip);
        pl_emit(PL_POP);
    }

    for (int index = 0; index < caseCount; index++)
    {
        pl_patchJump(caseEnds[index]);
    }

    pl_emit(PL_POP);
}

static void pl_sync()
{
    parser.panic = false;

    while (parser.cur.type != PL_TT_EOF) {
        if (parser.prev.type == PL_TT_SEMICOLON)
            return;

        switch (parser.cur.type)
        {
        case PL_TT_CLASS:
        case PL_TT_FUNC:
        case PL_TT_VAR:
        case PL_TT_FOR:
        case PL_TT_IF:
        case PL_TT_WHILE:
        case PL_TT_PRINT:
        case PL_TT_RETURN:
            return;
        default:
            ; // Do nothing.
        }

        pl_advance();
    }
}

static PlToken pl_synthToken(const char *text)
{
    PlToken token;
    token.type = PL_TT_IDENTIFIER;
    token.start = text;
    token.length = (int)strlen(text);
    token.line = 0;
    token.alloc = false;
    return token;
}

static void pl_this(bool assignable)
{
    if (curCls == NULL)
    {
        pl_error("Can't use 'this' outside of a class.");
        return;
    }

    pl_var(false);
}

static void pl_unary(bool assignable)
{
    PlTokenType operatorType = parser.prev.type;
    pl_parsePrecedence(PL_PREC_UNARY);

    switch (operatorType)
    {
    case PL_TT_BANG:
        pl_emit(PL_NOT);
        break;
    case PL_TT_MINUS:
        pl_emit(PL_NEGATE);
        break;
    case PL_TT_MINUS_MINUS:
        pl_emit2(PL_DEC_1, PL_DUPLICATE);
        pl_namedVar(parser.prev, assignable, false, true);
        pl_emit(PL_POP);
        break;
    case PL_TT_PLUS:
        pl_emit(PL_AFFIRM);
        break;
    case PL_TT_PLUS_PLUS:
        pl_emit2(PL_ADD_1, PL_DUPLICATE);
        pl_namedVar(parser.prev, assignable, false, true);
        pl_emit(PL_POP);
        break;
    case PL_TT_TILDE:
        pl_emit(PL_BITNOT);
        break;
    default:
        return; // Unreachable.
    }
}

static void pl_var(bool assignable)
{
    pl_namedVar(parser.prev, assignable, false, false);
}

static void pl_varDecl()
{
    uint8_t global = pl_parseVar("Expect variable name.");

    if (pl_match(PL_TT_EQUAL))
    {
        pl_expr();
    }

    else
    {
        pl_emit(PL_NULL);
    }

    pl_consume(PL_TT_SEMICOLON, "Expect ';' after variable declaration.");
    pl_defVar(global);
}

static void pl_whileStmt()
{
    PlBreaks *surroundingBreak = curBreak;

    PlBreaks newBreak;
    newBreak.parent = surroundingBreak;
    newBreak.breaks = NULL;
    newBreak.count = 0;
    newBreak.capacity = 0;
    curBreak = &newBreak;

    int surroundingLoopStart = innermostLoopStart;
    int surroundingLoopScopeDepth = innermostLoopScopeDepth;
    innermostLoopStart = pl_curSeg()->count;
    innermostLoopScopeDepth = cur->scopeDepth;

    pl_consume(PL_TT_LEFT_PAREN, "Expect '(' after 'while'.");
    pl_expr();
    pl_consume(PL_TT_RIGHT_PAREN, "Expect ')' after condition.");

    int exitJump = pl_emitJump(PL_JUMP_IF_FALSE);
    pl_emit(PL_POP);

    if (pl_match(PL_TT_LEFT_BRACE))
    {
        pl_block("Expect '}' after 'while' statements.");
    }

    else
    {
        pl_stmt();
    }

    pl_emitLoop(innermostLoopStart);

    pl_patchJump(exitJump);
    pl_emit(PL_POP);

    if (pl_match(PL_TT_NOBREAK))
    {
        if (pl_match(PL_TT_LEFT_BRACE))
        {
            pl_block("Expect '}' after 'nobreak' statements of 'while'.");
        }

        else
        {
            pl_stmt();
        }
    }

    for (int index = 0; index < curBreak->count; index++)
    {
        pl_patchJump(curBreak->breaks[index]);
    }

    if (curBreak->breaks)
    {
        free(curBreak->breaks);
    }

    curBreak = surroundingBreak;

    innermostLoopStart = surroundingLoopStart;
    innermostLoopScopeDepth = surroundingLoopScopeDepth;
}
