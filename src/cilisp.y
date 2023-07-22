%{
    #include "cilisp.h"
    #define ylog(r, p) {fprintf(flex_bison_log_file, "BISON: %s ::= %s \n", #r, #p); fflush(flex_bison_log_file);}
    int yylex();
    void yyerror(char*, ...);
%}

%union {
    double dval;
    int ival;
    char *cval;

    struct ast_node *astNode;
    struct symbol_table_node *tableNode;

    int nval;
};

%token <ival> FUNC
%token <dval> INT FLOAT
%token <cval> SYMBOL

%token <nval> TYPE

%token QUIT EOL EOFT LPAREN RPAREN let cond

%type <astNode> s_expr number f_expr s_expr_section s_expr_list
%type <tableNode> let_elem let_list let_section

%%

program:
    s_expr EOL {
        ylog(program, s_expr EOL);
        if ($1) {
            printRetVal(eval($1));
            freeNode($1);
        }
        YYACCEPT;
    }
    | s_expr EOFT {
        ylog(program, s_expr EOFT);
        if ($1) {
            printRetVal(eval($1));
            freeNode($1);
        }
        exit(EXIT_SUCCESS);
    }
    | EOL {
        ylog(program, EOL);
        YYACCEPT;  // paranoic; main skips blank lines
    }
    | EOFT {
        ylog(program, EOFT);
        exit(EXIT_SUCCESS);
    };


s_expr:
    f_expr {
        ylog(s_expr, f_expr);
        $$ = $1;
    }
    |
    number {
        ylog(s_expr, NUMBER);
        $$ = $1;
    }
    |
    SYMBOL {
        ylog(s_expr, SYMBOL);
        $$ = createSymbolNode($1);
    }
    |
    LPAREN let_section s_expr RPAREN {
        ylog(s_expr, LPAREN let_section s_expr RPAREN);
        $$ = createScopeNode($2, $3);
    }
    |
    LPAREN cond s_expr s_expr s_expr RPAREN {
        ylog(s_expr, LPAREN cond s_expr s_expr s_expr RPAREN);
        $$ = createCondNode($3, $4, $5);
    }
    |
    QUIT {
        ylog(s_expr, QUIT);
        exit(EXIT_SUCCESS);
    }
    | error {
        ylog(s_expr, error);
        yyerror("unexpected token");
        $$ = NULL;
    };

number:
    INT {
        ylog(number, INT);
        $$ = createNumberNode($1, INT_TYPE);
    }
    | FLOAT {
        ylog(number, FLOAT);
        $$ = createNumberNode($1, DOUBLE_TYPE);
    };

f_expr:
    LPAREN FUNC s_expr_section RPAREN {
        ylog(f_expr, LPAREN FUNC s_expr_section RPAREN);
        $$ = createFunctionNode($2, $3);
    };

s_expr_section:
    s_expr_list {
        ylog(s_expr_section, s_expr_list);
        $$ = $1;
    }
    |
    {
        ylog(s_expr_section, NULL);
        $$ = NULL;
    };

s_expr_list:
    s_expr {
        ylog(s_expr_list, s_expr);
        $$ = $1;
    }
    |
    s_expr s_expr_list {
        ylog(s_expr_list, s_expr s_expr_list);
        $$ = addExpressionToList($1, $2);
    };


let_section:
    LPAREN let let_list RPAREN {
        ylog(let_section, LPAREN let let_list RPAREN);
        $$ = $3;
    }
    |
    {   //Can't be empty
        ylog(s_expr_section, NULL);
        yyerror("let_section can NOT be null");
        $$ = NULL;
    };

let_list:
    let_elem {
        ylog(let_list, let_elem);
        $$ = $1;
    }
    |
    let_elem let_list{
        ylog(let_list, let_elem);
        $$ = addSymbolToTable($1, $2);
    };

let_elem:
    LPAREN SYMBOL s_expr RPAREN {
        ylog(let_elem, LPAREN SYMBOL s_expr RPAREN);
        $$ = createSymbolTable(NO_TYPE, $2, $3);
    }
    |
    LPAREN TYPE SYMBOL s_expr RPAREN {
        ylog(let_elem, LPAREN TYPE SYMBOL s_expr RPAREN);
        $$ = createSymbolTable($2, $3, $4);
    };

%%

