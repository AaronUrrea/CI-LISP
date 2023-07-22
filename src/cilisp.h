#ifndef __cilisp_h_
#define __cilisp_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "parser.h"

#define NAN_RET_VAL (RET_VAL){DOUBLE_TYPE, NAN}
#define ZERO_RET_VAL (RET_VAL){INT_TYPE, 0}

#define BISON_FLEX_LOG_PATH "../src/bison-flex-output/bison_flex_log"
FILE* read_target;
FILE* flex_bison_log_file;

int yyparse(void);
int yylex(void);
void yyerror(char *, ...);
void warning(char*, ...);

typedef enum func_type {

    // TODO complete the enum - Complete

    NEG_FUNC,
    ABS_FUNC,
    ADD_FUNC,
    SUB_FUNC,
    MULT_FUNC,
    DIV_FUNC,
    REM_FUNC,
    EXP_FUNC,
    EXP2_FUNC,
    POW_FUNC,
    LOG_FUNC,
    SQRT_FUNC,
    CBRT_FUNC,
    HYPOT_FUNC,
    MAX_FUNC,
    MIN_FUNC,
    RAND_FUNC,
    READ_FUNC,
    EQUAL_FUNC,
    LESS_FUNC,
    GREATER_FUNC,
    PRINT_FUNC,
    CUSTOM_FUNC
} FUNC_TYPE;

FUNC_TYPE resolveFunc(char *);

typedef enum num_type {
    INT_TYPE,
    DOUBLE_TYPE,
    NO_TYPE
} NUM_TYPE;

NUM_TYPE resolveNumType(char *);

typedef struct {
    NUM_TYPE type;
    double value;
} AST_NUMBER;

typedef AST_NUMBER RET_VAL;

typedef struct ast_function {
    FUNC_TYPE func;
    struct ast_node *opList;
} AST_FUNCTION;

typedef enum ast_node_type {
    NUM_NODE_TYPE,
    FUNC_NODE_TYPE,
    SYM_NODE_TYPE,   //SYMBOL NODE TYPE
    SCOPE_NODE_TYPE,  //SCOPE NODE TYPE
    COND_NODE_TYPE    //CONDITIONAL NODE TYPE
} AST_NODE_TYPE;

typedef struct {
    char* id; // THIS IS THE VARIABLE, e.g "x"
} AST_SYMBOL;

typedef struct {                //THE SCOPE IS BASICALLY THE HIERARCHY
    struct ast_node *child;     // THE CHILD IS NEXT UP
} AST_SCOPE;

typedef struct  {                   //THIS WORKS AS A TERNARY CONDITION ( a ? x : y )
    struct ast_node *condition;    // Condition
    struct ast_node *trueValue;    // Return if true
    struct ast_node *falseValue;   // Return if false
} AST_CONDITIONAL;

typedef struct ast_node {
    AST_NODE_TYPE type;

    struct ast_node *parent;                // PARENT NODE
    struct symbol_table_node *symbolTable;  // SYMBOL TABLE

    union {
        AST_NUMBER number;
        AST_FUNCTION function;

        AST_SYMBOL symbol;      //SYMBOL VALUE
        AST_SCOPE scope;        //SCOPE CONTAINER

        AST_CONDITIONAL conditional;
    } data;
    struct ast_node *next;
} AST_NODE;

typedef struct symbol_table_node {  //A LET_ELEM DEFINES A SYMBOL
    char *id;                       // ID OF VARIABLE
    AST_NODE *value;                // VALUE OF VARIABLE. THIS IS HOW WE ALLOW FUNCS TO CONTINUE
    struct symbol_table_node *next; // NEXT SYMBOL
    NUM_TYPE type;
} SYMBOL_TABLE_NODE;

AST_NODE *createNumberNode(double value, NUM_TYPE type);
AST_NODE *createFunctionNode(FUNC_TYPE func, AST_NODE *opList);
AST_NODE *createSymbolNode(char* ident);
AST_NODE *createScopeNode(SYMBOL_TABLE_NODE *symbol, AST_NODE *child);
AST_NODE *createCondNode(AST_NODE *condition, AST_NODE *trueVal, AST_NODE *falseVal);
SYMBOL_TABLE_NODE *createSymbolTable(NUM_TYPE type, char* ident, AST_NODE *value);

AST_NODE *addExpressionToList(AST_NODE *newExpr, AST_NODE *exprList);
SYMBOL_TABLE_NODE *addSymbolToTable(SYMBOL_TABLE_NODE *newTable, SYMBOL_TABLE_NODE *tableList);

RET_VAL eval(AST_NODE *node);

void printRetVal(RET_VAL val);

void freeNode(AST_NODE *node);
void freeSymbolTable(SYMBOL_TABLE_NODE *table);


#endif
