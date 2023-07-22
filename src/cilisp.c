/*

Checklist:
    Task 1: (50 Pts)
        Completed:  ✓
        Debugged:   ✓
        Valgrind:   ✓
        Documented: ✓
    Task 2: (15 Pts)
        Completed:  ✓
        Debugged:   ✓
        Valgrind:   ✓
        Documented: ✓
    Task 3: (5 Pts)
        Completed:  ✓
        Debugged:   ✓
        Valgrind:   ✓
        Documented: ✓
    Task 4: (15 Pts)
        Completed:  ✓
        Debugged:   ✓
        Valgrind:   ✓
        Documented: ✓
    Task 5: (15 Pts)
        Completed:  X
        Debugged:   X
        Valgrind:   X
        Documented: X

Reasoning:
        I was unable to complete Task 5 due to time issues, and a hectic final week in regards to
    both personal issues as well as academic issues. The broken, unfinished Task 5 sits uncommitted, as
    I did not make much progress on it to begin with and I would much rather have a finished, documented and
    bug free (as far as I am aware) Task 4 and accept the 85%, if the work is up to that level of course.
    I would absolutely love to hear any criticism and/or advice for this project, as this is probably my
    first HIGH level programming project, and I am always ready to grow as both a student, and a person!

        I thank you both, Professor Bieszczad and Professor McIntyre, for this great learning experience. I have
    learned so much from this class and I believe I have a much better grasp computer science as a whole than I
    did before. I feel I am a better programmer, and I have nobody else to thank but you two. I hope to have a future
    class with you both again!

        Have a great summer!
            Aaron Urrea

*/

/******************************************************************************************************************/
/*                                               PROGRAM BEGIN:                                                   */
/******************************************************************************************************************/

#include "cilisp.h"

#define RED             "\033[31m"
#define RESET_COLOR     "\033[0m"
#define BUFFER 32

// These are all of the evaluation functions defined before they are declared.
// I defined them here in order to reduce clutter in cilisp.h
RET_VAL evalNeg(AST_NODE *operands);
RET_VAL evalAdd(AST_NODE *operands);
RET_VAL evalAbs(AST_NODE *operands);
RET_VAL evalSub(AST_NODE *operands);
RET_VAL evalMult(AST_NODE *operands);
RET_VAL evalDiv(AST_NODE *operands);
RET_VAL evalRemainder(AST_NODE *operands);
RET_VAL evalExp(AST_NODE *operands);
RET_VAL evalExp2(AST_NODE *operands);
RET_VAL evalPow(AST_NODE *operands);
RET_VAL evalLog(AST_NODE *operands);
RET_VAL evalSqrt(AST_NODE *operands);
RET_VAL evalCbrt(AST_NODE *operands);
RET_VAL evalHypot(AST_NODE *operands);
RET_VAL evalMax(AST_NODE *operands);
RET_VAL evalMin(AST_NODE *operands);

// These functions were specifically added for conditional nodes
RET_VAL evalRand();
RET_VAL evalRead();
RET_VAL evalEqual(AST_NODE *operands);
RET_VAL evalLess(AST_NODE *operands);
RET_VAL evalGreater(AST_NODE *operands);
RET_VAL evalPrint(AST_NODE *operands);

// Further mode, other evaluation methods of nodes this time
//  defined here to reduce clutter in cilisp.h
RET_VAL evalSymNode(AST_NODE *node);
RET_VAL evalScopeNode(AST_NODE *node);
RET_VAL evalCondNode(AST_NODE *node);

// yyerror:
// Something went so wrong that the whole program should crash.
// You should basically never call this unless an allocation fails.
// (see the "yyerror("Memory allocation failed!")" calls and do the same.
// This is basically printf, but red, with "\nERROR: " prepended, "\n" appended,
// and an "exit(1);" at the end to crash the program.
// It's called "yyerror" instead of "error" so the parser will use it for errors too.
void yyerror(char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start (args, format);
    vsnprintf (buffer, 255, format, args);

    printf(RED "\nERROR: %s\nExiting...\n" RESET_COLOR, buffer);
    fflush(stdout);

    va_end (args);
    exit(1);
}

// warning:
// Something went mildly wrong (on the user-input level, probably)
// Let the user know what happened and what you're doing about it.
// Then, move on. No big deal, they can enter more inputs. ¯\_(ツ)_/¯
// You should use this pretty often:
//      too many arguments, let them know and ignore the extra
//      too few arguments, let them know and return NAN
//      invalid arguments, let them know and return NAN
//      many more uses to be added as we progress...
// This is basically printf, but red, and with "\nWARNING: " prepended and "\n" appended.
void warning(char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start (args, format);
    vsnprintf (buffer, 255, format, args);

    printf(RED "WARNING: %s\n" RESET_COLOR, buffer);
    fflush(stdout);

    va_end (args);
}

// Reads from input and if the input matches a function
// Returns the int value that corresponds with that func char
//  e.g Input = "add", return FUNC_TYPE = 2
FUNC_TYPE resolveFunc(char *funcName)
{
    // Array of string values for function names.
    // Must be in sync with members of the FUNC_TYPE enum in order for resolveFunc to work.
    // For example, funcNames[NEG_FUNC] should be "neg"
    char *funcNames[] = {
            "neg",
            "abs",
            "add",
            "sub",
            "mult",
            "div",
            "remainder",
            "exp",
            "exp2",
            "pow",
            "log",
            "sqrt",
            "cbrt",
            "hypot",
            "max",
            "min",
            // TODO complete the array - Complete
            // the empty string below must remain the last element
            "rand",
            "read",
            "equal",
            "less",
            "greater",
            "print",
            ""
    };
    int i = 0;
    while (funcNames[i][0] != '\0')
    {
        if (strcmp(funcNames[i], funcName) == 0)
        {
            return i;
        }
        i++;
    }
    return CUSTOM_FUNC;
}

// Reads from input and if the input matches a digit type
// Returns the int value that corresponds with that digit char
//  e.g Input = "int", return NUM_TYPE = 0;
NUM_TYPE resolveNumType(char *typeName){
    char *typeNames[] = {
            "int",
            "double",
            ""
    };
    int i = 0;
    while (typeNames[i][0] != '\0')
    {
        if (strcmp(typeNames[i], typeName) == 0)
        {
            return i;
        }
        i++;
    }
    return NO_TYPE;
}

// Reads from input, then sends double and NUM_TYPE value
// Create a Number node type, fills in appropriate values (data->number.value and data->number.type)
// Returns new node
AST_NODE *createNumberNode(double value, NUM_TYPE type)
{
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
    {
        yyerror("Memory allocation failed!");
        exit(1);
    }

    // TODO complete the function - Complete
    // Populate "node", the AST_NODE * created above with the argument data.
    // node is a generic AST_NODE, don't forget to specify it is of type NUMBER_NODE

    node->type = NUM_NODE_TYPE;

    switch(type){
        case INT_TYPE:
            node->data.number.type = INT_TYPE;
            node->data.number.value = value;
            break;

        case DOUBLE_TYPE:
            node->data.number.type = DOUBLE_TYPE;
            node->data.number.value = value;
            break;

        default:
            yyerror("\nERROR, invalid type in createNumberNode()...");
    }

    return node;
}

// Reads from input, then sends FUNC_TYPE and AST_NODE opList
// opList is basically the list of operations to perform
//  In hierarchy terms, it is the children operations to perform
// Create a func node type, fills in appropriate values (data->function.func and data->function->opList)
// Returns new node
AST_NODE *createFunctionNode(FUNC_TYPE func, AST_NODE *opList)
{
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
    {
        yyerror("Memory allocation failed!");
        exit(1);
    }

    node->type = FUNC_NODE_TYPE;
    node->data.function.func = func;
    node->data.function.opList = opList;

    while(opList != NULL) {
        opList->parent = node; //TASK 2 ADDITION
        opList = opList->next;
    }
    // TODO complete the function - Complete
    // Populate the allocated AST_NODE *node's data

    return node;
}

// Reads from input, then sends SYMBOL_TABLE_NODE and AST_NODE orphan
// Scope nodes only have children, they're special because they ONLY have children
//  and nothing else. Sets parent as node and child as orphan
// Fills in appropriate values, (node->data.scope.child and next)
// Returns new node
AST_NODE *createScopeNode(SYMBOL_TABLE_NODE *symbol, AST_NODE *orphan){
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
    {
        yyerror("Memory allocation failed!");
        exit(1);
    }

    node->type = SCOPE_NODE_TYPE;

    node->data.scope.child = orphan;          //Node child = next

    orphan->parent = node;                    //Next parent = node
    orphan->symbolTable = symbol;             //SymbolTable assigned to child

    SYMBOL_TABLE_NODE *helper = orphan->symbolTable;
    while(helper != NULL) {   //Every parent in symbol table assigned to scope
        helper->value->parent = orphan;
        helper = helper->next;
    }

    return node;
}

// Reads from input, then sends char ident
// Symbol nodes are just nodes with an identifier, with subsequent value to be added later
//  Thus, only thing manipulated is data->symbol.id
// Returns new node
AST_NODE *createSymbolNode(char *ident){
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
    {
        yyerror("Memory allocation failed!");
        exit(1);
    }

    node->type = SYM_NODE_TYPE;
    node->data.symbol.id = ident;

    return node;
}

// Reads from input, then sends AST_NODE condition, trueVal, and falseVal
// It will assign the corresponding AST_NODES to the data location
//  data->conditional.condition = condition;
//  data->conditional.true = trueVal;
//  data->conditional.false = falseVal;
// Returns new node
AST_NODE *createCondNode(AST_NODE *condition, AST_NODE *trueVal, AST_NODE *falseVal){
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
    {
        yyerror("Memory allocation failed!");
        exit(1);
    }

    node->type = COND_NODE_TYPE;

    condition->parent = node;
    trueVal->parent = node;
    falseVal->parent = node;

    node->data.conditional.condition = condition;
    node->data.conditional.trueValue = trueVal;
    node->data.conditional.falseValue = falseVal;

    return node;
}

// Creates essentially a fancy linkedList
//  Each table has a value, (AST_NODE) an identifier, (char) and a next value
//  The next value corresponds with additional symbols, like a true linkedList
// Fills in the appropriate values, then returns new symbol table
SYMBOL_TABLE_NODE *createSymbolTable(NUM_TYPE type, char* ident, AST_NODE *value){

    SYMBOL_TABLE_NODE *symbol;
    size_t tableSize = sizeof(SYMBOL_TABLE_NODE);

    if ((symbol = calloc(tableSize, 1)) == NULL)
    {
        yyerror("Memory allocation failed!");
        exit(1);
    }

    symbol->id = ident;
    symbol->type = type;
    symbol->value = value;

    return symbol;
}

// This adds a new symbol to a pre-existing symbol table
//  This is a fancy comment for just doing tableList->next = newTable
// We also go down the tableList and see if the identifier is in there
//  If it is, give the user a snarky comment
// Wow.
SYMBOL_TABLE_NODE *addSymbolToTable(SYMBOL_TABLE_NODE *newTable, SYMBOL_TABLE_NODE *tableList){
    newTable->next = tableList;

    SYMBOL_TABLE_NODE *toCheck = tableList;
    while(toCheck != NULL){
        if(strcmp(newTable->id, toCheck->id) == 0) {
            warning("Duplicate assignment to symbol \"%s\" detected in the same scope!", newTable->id);
            printf(RED "Only the first assignment is kept!\n" RESET_COLOR);
        }

        toCheck = toCheck->next;
    }

    return newTable;
}

// This sets the next value of a node to another node
//  It also sets the parent of the next node to the first node
// Returns the new node with next applied.
AST_NODE *addExpressionToList(AST_NODE *newExpr, AST_NODE *exprList)
{
    // TODO complete the function - Complete
    // at newExpr to the exprList as the head. return the resulting list's head.

    newExpr->next = exprList;
    exprList->parent = newExpr;

    return newExpr; // stub
}

// Evaluates the func type and returns the value from the corresponding method
//  If func = add, then it will send opList and return a value from evalAdd
//  Every single function has a corresponding method attached, it is a fancy switch case
// Once eval is called, return said return value.
RET_VAL evalFuncNode(AST_NODE *node)
{
    if (!node)
    {
        yyerror("NULL ast node passed into evalFuncNode!");
        return NAN_RET_VAL; // unreachable but kills a clang-tidy warning
    }

    // TODO complete the function & replace calls with ZERO_RET_VAL - Complete
    // HINT:
    // the helper functions that it calls will need to be defined above it
    // because they are not declared in the .h file (and should not be)

    RET_VAL toReturn = NAN_RET_VAL;


    switch(node->data.function.func){
        case NEG_FUNC:
            toReturn = evalNeg(node->data.function.opList);
            break;
        case ABS_FUNC:
            toReturn = evalAbs(node->data.function.opList);
            break;
        case ADD_FUNC:
            toReturn = evalAdd(node->data.function.opList);
            break;
        case SUB_FUNC:
            toReturn = evalSub(node->data.function.opList);
            break;
        case MULT_FUNC:
            toReturn = evalMult(node->data.function.opList);
            break;
        case DIV_FUNC:
            toReturn = evalDiv(node->data.function.opList);
            break;
        case REM_FUNC:
            toReturn = evalRemainder(node->data.function.opList);
            break;
        case EXP_FUNC:
            toReturn = evalExp(node->data.function.opList);
            break;
        case EXP2_FUNC:
            toReturn = evalExp2(node->data.function.opList);
            break;
        case POW_FUNC:
            toReturn = evalPow(node->data.function.opList);
            break;
        case LOG_FUNC:
            toReturn = evalLog(node->data.function.opList);
            break;
        case SQRT_FUNC:
            toReturn = evalSqrt(node->data.function.opList);
            break;
        case CBRT_FUNC:
            toReturn = evalCbrt(node->data.function.opList);
            break;
        case HYPOT_FUNC:
            toReturn = evalHypot(node->data.function.opList);
            break;
        case MAX_FUNC:
            toReturn = evalMax(node->data.function.opList);
            break;
        case MIN_FUNC:
            toReturn = evalMin(node->data.function.opList);
            break;
        case RAND_FUNC:
            toReturn = evalRand();
            break;
        case READ_FUNC:
            toReturn = evalRead();
            break;
        case EQUAL_FUNC:
            toReturn = evalEqual(node->data.function.opList);
            break;
        case LESS_FUNC:
            toReturn = evalLess(node->data.function.opList);
            break;
        case GREATER_FUNC:
            toReturn = evalGreater(node->data.function.opList);
            break;
        case PRINT_FUNC:
            toReturn = evalPrint(node->data.function.opList);
            break;
        case CUSTOM_FUNC:
            printf("\n\e[0;35mChallenge Complete!\n\e[0;0mHow did we get here?\n\n");
            break;
    }

    // DEBUG
    //printf("\nCalculation tree: %f\n", toReturn.value);

    return toReturn;
}

// These are all of the evaluation methods, that return a RET_VAL after completion
//  Their names are all self explanatory. I don't want to write twenty something comment blocks
//  If you are still confused, just read the documentation :)
//   ...Who reads documentation am I right?
//      ...Does this count as documentation?
//         ...The ramblings of an undergrad
RET_VAL evalNeg(AST_NODE *operands) {

    if(!operands) {
        warning("neg called with no operands! nan returned!");
        return NAN_RET_VAL;
    }

    RET_VAL result = eval(operands);
    result.value = -result.value;

    if(operands->next)
        warning("neg call with extra (ignored) operands!");

    return result;
}

RET_VAL evalAbs(AST_NODE *operands) {
    if(!operands) {
        warning("abs called with no operands! nan returned!");
        return NAN_RET_VAL;
    }

    RET_VAL result = eval(operands);
    result.value = fabs(result.value);

    if(operands->next)
        warning("abs call with extra (ignored) operands!");

    return result;

}

RET_VAL evalAdd(AST_NODE *operands) {
    if (!operands) {
        warning("add called with no operands! 0 returned!");
        return ZERO_RET_VAL;
    }

    else if(!operands->next)
        return eval(operands);

    RET_VAL result = eval(operands);
    RET_VAL current;

    operands = operands->next;
    while(operands) {
        current = eval(operands);
        result.value += current.value;
        if(result.type != DOUBLE_TYPE && current.type == DOUBLE_TYPE)
            result.type = DOUBLE_TYPE;

        operands = operands->next;
    }

    return result;

}

RET_VAL evalSub(AST_NODE *operands) {
    if (!operands) {
        warning("sub called with no operands!!");
        return NAN_RET_VAL;
    }
    else if (!operands->next) {
        warning("sub called with only one arg!");
        return NAN_RET_VAL;
    }

    RET_VAL firstOperand = eval(operands);
    RET_VAL secondOperand = eval(operands->next);

    //printf("Subtracting! %f %f\n", firstOperand.value, secondOperand.value);
    // DEBUG

    RET_VAL result = {
            .value = firstOperand.value - secondOperand.value,
            .type = firstOperand.type || secondOperand.type};

    if(operands->next->next)
        warning("sub called with extra (ignored) operands!");

    return result;
}

RET_VAL evalMult(AST_NODE *operands) {
    if (!operands) {
        warning("mult called with no operands! 1 returned!");
        return (RET_VAL) { .value = 1, .type = INT_TYPE};
    }
    else if (!operands->next)
        return operands->data.number;

    RET_VAL result = eval(operands);
    RET_VAL current;

    operands = operands->next;
    while(operands) {
        current = eval(operands);
        result.value *= current.value;

        if(result.type != DOUBLE_TYPE && current.type == DOUBLE_TYPE)
            result.type = DOUBLE_TYPE;

        operands = operands->next;
    }

    return result;
}

RET_VAL evalDiv(AST_NODE *operands) {
    if (!operands) {
        warning("div called with no operands! nan returned!");
        return NAN_RET_VAL;
    }
    else if (!operands->next) {
        warning("div called with only one arg! nan returned");
        return NAN_RET_VAL;
    }

    RET_VAL firstOperand = eval(operands);
    RET_VAL secondOperand = eval(operands->next);

    RET_VAL result = {
            .value = firstOperand.value/secondOperand.value,
            .type = firstOperand.type || secondOperand.type};

    if(operands->next->next)
        warning("div called with extra (ignored) operands!");

    if(result.type == INT_TYPE )
        result.value = (long) result.value; //trunc() would work as well as we want
                                            // to round down to the nearest positive integer

    return result;
}

RET_VAL evalRemainder(AST_NODE *operands) {
    if (!operands) {
        warning("remainder called with no operands! nan returned!");
        return NAN_RET_VAL;
    }
    else if (!operands->next) {
        warning("remainder called with only one arg! nan returned");
        return NAN_RET_VAL;
    }

    RET_VAL firstOperand = eval(operands);
    RET_VAL secondOperand = eval(operands->next);

    RET_VAL result = {
            .value = fmod(firstOperand.value, secondOperand.value),
            .type = firstOperand.type || secondOperand.type};

    if(result.value < 0)
        result.value += fabs(secondOperand.value);

    if(operands->next->next)
        warning("remainder called with extra (ignored) operands!");

    return result;
}

RET_VAL evalExp(AST_NODE *operands) {
    if (!operands) {
        warning("exp called with no operands! nan returned!");
        return NAN_RET_VAL;
    }

    RET_VAL onlyOperand = eval(operands);

    RET_VAL result = {
            .value = exp(onlyOperand.value),
            .type = DOUBLE_TYPE };

    if(operands->next)
        warning("exp called with extra (ignored) operands!");

    return result;
}

RET_VAL evalExp2(AST_NODE *operands) {
    if (!operands) {
        warning("exp2 called with no operands! nan returned!");
        return NAN_RET_VAL;
    }

    RET_VAL onlyOperand = eval(operands);

    RET_VAL result = {
            .value = pow(2.0, onlyOperand.value),
            .type = (eval(operands).value < 0) ?
                DOUBLE_TYPE : onlyOperand.type};

    if(operands->next)
        warning("exp2 called with extra (ignored) operands!");

    return result;
}

RET_VAL evalPow(AST_NODE *operands) {
    if (!operands) {
        warning("pow called with no operands! nan returned!");
        return NAN_RET_VAL;
    }
    else if (!operands->next) {
        warning("pow called with only 1 operand! nan returned!");
        return NAN_RET_VAL;
    }

    RET_VAL firstOperand = eval(operands);
    RET_VAL secondOperand = eval(operands->next);


    RET_VAL result = {
            .value = pow(firstOperand.value, secondOperand.value),
            .type = firstOperand.type || secondOperand.type};

    if(isnan(result.value))
        result.value = fabs(result.value);

    if(operands->next->next)
        warning("pow called with extra (ignored) operands!");

    return result;
}

RET_VAL evalLog(AST_NODE *operands) {
    if (!operands) {
        warning("log called with no operands! nan returned!");
        return NAN_RET_VAL;
    }

    RET_VAL onlyOperand = eval(operands);

    RET_VAL result = {
            .value = log(onlyOperand.value),
            .type = DOUBLE_TYPE};

    if(isnan(result.value))
        result.value = fabs(result.value);

    if(operands->next)
        warning("log called with extra (ignored) operands!");

    return result;
}

RET_VAL evalSqrt(AST_NODE *operands) {
    if (!operands) {
        warning("sqrt called with no operands! nan returned!");
        return NAN_RET_VAL;
    }

    RET_VAL onlyOperand = eval(operands);

    RET_VAL result = {
            .value = sqrt(onlyOperand.value),
            .type = DOUBLE_TYPE };

    if(isnan(result.value))
        result.value = fabs(result.value);

    if(operands->next)
        warning("sqrt called with extra (ignored) operands!");

    return result;
}

RET_VAL evalCbrt(AST_NODE *operands) {
    if (!operands) {
        warning("cbrt called with no operands! nan returned!");
        return NAN_RET_VAL;
    }

    RET_VAL onlyOperand = eval(operands);

    RET_VAL result = {
            .value = cbrt(onlyOperand.value),
            .type = DOUBLE_TYPE };

    if(isnan(result.value))
        result.value = fabs(result.value);

    if(operands->next)
        warning("cbrt called with extra (ignored) operands!");

    return result;
}

RET_VAL evalHypot(AST_NODE *operands) {
    if (!operands) {
        warning("hypot called with no operands! 0 returned!");
        return ZERO_RET_VAL;
    }

    RET_VAL result = {
            .value = pow(eval(operands).value, 2),
            .type = DOUBLE_TYPE};
    RET_VAL current;
    operands = operands->next;
    while(operands) {
        current = eval(operands);
        result.value += pow(current.value, 2);
        operands = operands->next;
    }

    result.value = sqrt(result.value);

    return result;
}

RET_VAL evalMax(AST_NODE *operands){
    if (!operands) {
        warning("max called with no operands! nan returned!");
        return NAN_RET_VAL;
    }

    RET_VAL result = eval(operands);
    RET_VAL current;

    operands = operands->next;
    while(operands) {
        current = eval(operands);
        if(current.value > result.value)
            result = current;

        operands = operands->next;
    }

    return result;
}

RET_VAL evalMin(AST_NODE *operands){
    if (!operands) {
        warning("min called with no operands! nan returned!");
        return NAN_RET_VAL;
    }

    RET_VAL result = eval(operands);
    RET_VAL current;

    operands = operands->next;
    while(operands) {
        current = eval(operands);

        if(current.value < result.value)
            result = current;

        operands = operands->next;
    }

    return result;
}

RET_VAL evalRand(){
    double random  = (double) rand() / RAND_MAX;
    RET_VAL toReturn = {
            .type = DOUBLE_TYPE,
            .value =  random
    };

    if(toReturn.value == 0 || toReturn.value == 1)
        toReturn.type = INT_TYPE;

    return toReturn;
}

RET_VAL evalRead(){

    char str [BUFFER];

    fscanf(read_target, "%s", str);

    bool isNumber = true;
    bool isDouble = false;

    printf("read :: %s\n", str);

    int i = 0;
    while(isNumber && i < strlen(str)){
        switch(str[i]){
            case '0' ... '9':
                i++;
                break;

            case '.':
                if(i == 0 || isDouble) // This checks if there is a decimal after the number is a double
                    isNumber = false;  // e.g we can't have 3.1415.92
                else
                    isDouble = true;
                i++;
                break;

            case '+':
            case '-':
                if(i != 0)
                    isNumber = false;  // We can't have any symbols in the first index
                i++;
                break;

            default:
                isNumber = false;
                break;
        }
    }

    if(!isNumber) {
        warning("Invalid read entry! NAN returned!");
        return NAN_RET_VAL;
    }

    RET_VAL toReturn = {
            .value = strtod(str, NULL),
            .type = (isDouble ? DOUBLE_TYPE : INT_TYPE)
    };

    return toReturn;

}

RET_VAL evalEqual(AST_NODE *operands){
    if (!operands) {
        warning("equal called with no operands! nan returned!");
        return NAN_RET_VAL;
    }
    else if (!operands->next) {
        warning("equal called without 2 operands! nan returned!");
        return NAN_RET_VAL;
    }

    RET_VAL first = eval(operands);
    RET_VAL second = eval(operands->next);

    RET_VAL toReturn = {
            .type = INT_TYPE,
            .value = (first.value == second.value ? 1 : 0)
    };

    return toReturn;

}

RET_VAL evalLess(AST_NODE *operands) {
    if (!operands) {
        warning("less called with no operands! nan returned!");
        return NAN_RET_VAL;
    }
    else if (!operands->next) {
        warning("less called without 2 operands! nan returned!");
        return NAN_RET_VAL;
    }

    RET_VAL first = eval(operands);
    RET_VAL second = eval(operands->next);

    RET_VAL toReturn = {
            .type = INT_TYPE,
            .value = (first.value < second.value ? 1 : 0)
    };

    return toReturn;
}

RET_VAL evalGreater(AST_NODE *operands) {
    if (!operands) {
        warning("less called with no operands! nan returned!");
        return NAN_RET_VAL;
    }
    else if (!operands->next) {
        warning("less called without 2 operands! nan returned!");
        return NAN_RET_VAL;
    }

    RET_VAL first = eval(operands);
    RET_VAL second = eval(operands->next);

    RET_VAL toReturn = {
            .type = INT_TYPE,
            .value = (first.value > second.value ? 1 : 0)
    };

    return toReturn;
}

RET_VAL evalPrint(AST_NODE *operands){
    if (!operands) {
        warning("print called with no operands! nan returned!");
        return NAN_RET_VAL;
    }
    else if(operands->next)
        warning("print called with extra (ignored) operands");

    RET_VAL toReturn = eval(operands);
    printRetVal(toReturn);

    return toReturn;
}

// FINALLY SOMETHING NEW
//  ...
// This function returns the data->number of a node
//  If said value does not exist, call an error
RET_VAL evalNumNode(AST_NODE *node)
{
    if (!node)
    {
        yyerror("NULL ast node passed into evalNumNode!");
        return NAN_RET_VAL;
    }

    // TODO complete the function - Complete

    return node->data.number;
}

// Where do I even start with this one...
// We essentially search through EVERY symbol table of EVERY node
//  in search of the value of this nodes identifier
//  and if we find the identifier, return the value of the identifier
// Else, we will check the parent node, and ad infinitum
//  until we find a match or we run out of node.
// And...
// ...
// ...Just check the documentation located inside
RET_VAL evalSymNode(AST_NODE *node){

    // You can figure this one out
    //  I believe in you!
    if (!node)
    {
        yyerror("NULL ast node passed into evalNumNode!");
        return NAN_RET_VAL;
    }

    AST_NODE *currentNode = node;
    SYMBOL_TABLE_NODE *currentSymbolTable;

    RET_VAL toReturn = NAN_RET_VAL;
    bool match = false;

    // While this current node isn't null and we haven't found an ident match
    while(currentNode != NULL && !match) {
        currentSymbolTable = currentNode->symbolTable;

        // While currentSymbolTable isn't null and we haven't found an ident match
        while (currentSymbolTable != NULL && !match) {

            //If the ident of our node matches the currentSymbolTable ident
            if (strcmp(node->data.symbol.id, currentSymbolTable->id) == 0) {

                // toReturn will the evaluation value of this currentSymbolTable node
                toReturn = eval(currentSymbolTable->value);

                // If the currentSymbolTable type is not a numberNode, then we'll just create a
                //  new one with the evaluation value in toReturn
                // That way, we can free up the memory from the subsequent nodes
                if(currentSymbolTable->value->type != NUM_NODE_TYPE) {
                    freeNode(currentSymbolTable->value);
                    currentSymbolTable->value = createNumberNode(toReturn.value, toReturn.type);
                }
                // If it is a numberNode, then there is no need to free anything else, as it's at
                //  the bottom of the tree already
                else {
                    currentSymbolTable->value->type = NUM_NODE_TYPE; //Change the node type so we don't keep eval()
                    currentSymbolTable->value->data.number = toReturn;
                }

                // This will set match to true as to break the loop
                //  Match as in "Match found", the symbol was found successfully
                match = true;

                // This will check the NUM_TYPE of the currentSymbolTable node
                //  If it is INT, it will check if there is precision loss,
                //   Specifically from going to Double to Int
                //  Else, it will just assign values to toReturn
                switch(currentSymbolTable->type){
                    case INT_TYPE:
                        if(toReturn.type == DOUBLE_TYPE)
                            warning("Precision loss on int cast from %f to %f",
                                    toReturn.value, floor(toReturn.value));
                        toReturn.type = currentSymbolTable->type;
                        break;

                    case DOUBLE_TYPE:
                        toReturn.type = currentSymbolTable->type;
                        break;

                    default:
                        break;
                }
            }
            // Will check the next in the symbol table list
            currentSymbolTable = currentSymbolTable->next;
        }
        // Will check the parent node, as we're travelling UP, not DOWN
        currentNode = currentNode->parent;
    }

    // If we didn't find a match, then send a polite but FIRM warning
    //  letting the user know that they are definitely not above
    //  the median intelligence level.
    if(!match)
        warning("Undefined Symbol \"%s\" evaluated! NAN returned!", node->data.symbol.id);

    // Return toReturn.
    // ...You get the point.
    return toReturn;
}

// Since scope nodes are unique in that they only have children
//  It will return the evaluation of the child of the node
//  Pretty straight forward
RET_VAL evalScopeNode(AST_NODE *node){
    if (!node)
    {
        yyerror("NULL ast node passed into evalScopeNode!");
        return NAN_RET_VAL;
    }

    return eval(node->data.scope.child);
}

// Evaluates the condition of a node
//  If the return value of that is not zero, then return the evaluation of the trueValue
//  Else, then return the evaluation of the falseValue
// A glorified (x ? y : z) that made me pull hair out
//  Lord have mercy on next year's students
RET_VAL evalCondNode(AST_NODE *node){
    // TASK 2 ADDITION
    if (!node)
    {
        yyerror("NULL ast node passed into evalCondNode!");
        return NAN_RET_VAL;
    }

    RET_VAL condition = eval(node->data.conditional.condition);

    if(condition.value != 0)
        return eval(node->data.conditional.trueValue);

    return eval(node->data.conditional.falseValue);
}

// Examines the contents of a node
//  Depending on the type of node, it will return the evaluation of the corresponding method
//   e.g if the node is a symbol node, return evalSymNode(node)
RET_VAL eval(AST_NODE *node)
{
    if (!node)
    {
        yyerror("NULL ast node passed into eval!");
        return NAN_RET_VAL;
    }


    switch(node->type){
        case NUM_NODE_TYPE:
            return evalNumNode(node);
        case FUNC_NODE_TYPE:
            return evalFuncNode(node);
        case SYM_NODE_TYPE:
            return evalSymNode(node);
        case SCOPE_NODE_TYPE:
            return evalScopeNode(node);
        case COND_NODE_TYPE:
            return evalCondNode(node);
        default:
            printf("Invalid node type in eval(). NULL?\n");
            break;
    }


    // TODO complete the function - Complete

    return NAN_RET_VAL;
}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val)
{
    switch (val.type)
    {
        case INT_TYPE:
            printf("Integer : %.lf\n", val.value);
            break;
        case DOUBLE_TYPE:
            printf("Double : %lf\n", val.value);
            break;
        default:
            printf("No Type : %lf\n", val.value);
            break;
    }
}

// Documentation is located within the method
//  This one was fun
//   ...I <3 Valgrind
void freeNode(AST_NODE *node)
{
    if (!node)
    {
        return;
    }

    // TODO complete the function - Complete

    // look through the AST_NODE struct, decide what
    // referenced data should have freeNode called on it
    // (hint: it might be part of an s_expr_list, with more
    // nodes following it in the list)

    // if this node is FUNC_TYPE, it might have some operands
    // to free as well (but this should probably be done in
    // a call to another function, named something like
    // freeFunctionNode)

    // GO DOWN TREE, NOT UP. e.g node->parent

    // Every node is capable of having a symbol table
    //  Therefore, check every node for a symbol table
    //   If true, then call freeSymbolTable
    if(node->symbolTable)
        freeSymbolTable(node->symbolTable);

    // Checks the data type of the node
    //  Depending on the type, it will clear the corresponding data.
    //  In the case where the data is a node, recursively call freeNode
    switch(node->type){
        case FUNC_NODE_TYPE:
            freeNode(node->data.function.opList);
            break;

        case SYM_NODE_TYPE:
            free(node->data.symbol.id);
            break;

        case COND_NODE_TYPE:
            freeNode(node->data.conditional.condition);
            freeNode(node->data.conditional.trueValue);
            freeNode(node->data.conditional.falseValue);
            break;

        case SCOPE_NODE_TYPE:
            freeNode(node->data.scope.child);
            break;
        default:
            break;
    }

    // Since we're going bottom up, always call the next node before the current
    freeNode(node->next);

    // and, finally,
    free(node);
}

// Documentation is located within the method as well
void freeSymbolTable(SYMBOL_TABLE_NODE *table){

    // If the table is empty, (e.g there is no next value) return
    if(!table)
        return;

    // Free the table identifier and value
    free(table->id);
    freeNode(table->value);

    // Recursively free the next table node (BOTTOM-UP)
    freeSymbolTable(table->next);

    // Finally, free the current table node
    free(table);
}