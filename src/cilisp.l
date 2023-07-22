%option noyywrap
%option noinput
%option nounput

%{
    #include "cilisp.h"
    #define llog(token) {fprintf(flex_bison_log_file, "LEX: %s \"%s\"\n", #token, yytext); fflush(flex_bison_log_file);}
%}

digit       [0-9]
int         [+-]?{digit}+
float       [+-]?{digit}+\.{digit}*
letter      [a-zA-Z$_]

func        (neg)|(abs)|(add)|(sub)|(mult)|(div)|(remainder)|(exp)|(exp2)|(pow)|(log)|(sqrt)|(cbrt)|(hypot)|(max)|(min)|(rand)|(read)|(equal)|(less)|(greater)|(print)

type        "int"|"double"

number      {(int)|(float)}

symbol      {letter}({letter}|{digit})*


%%

{int} {
    llog(INT);
    yylval.dval = strtod(yytext, NULL);
    return INT;
}

{float} {
    llog(FLOAT);
    yylval.dval = strtod(yytext, NULL);
    return FLOAT;
}

"(" {
    llog(LPAREN);
    return LPAREN;
}

")" {
    llog(RPAREN);
    return RPAREN;
}

{func} {
    llog(FUNC);
    yylval.ival = resolveFunc(yytext);
    return FUNC;
}

[\n] {
    llog(EOL);
    return EOL;
    }

[\xff] {
    llog(EOFT);
    return EOFT;
    }

{type} {
    llog(TYPE);
    yylval.nval = resolveNumType(yytext);
    return TYPE;
}

[ \t\r] ; /* skip whitespace */

"quit" {
    llog(QUIT);
    return QUIT;
}

"let" {
    llog(LET);
    return let;
}

"cond" {
    llog(COND);
    return cond;
}

{symbol} {
    llog(SYMBOL);
    yylval.cval = strdup(yytext);
    return SYMBOL;
}

. { // anything else
    llog(INVALID);
    warning("Invalid character >>%s<<", yytext);
    }

%%

// Edit at your own risk.

#include <stdio.h>
#include "yyreadprint.c"

int main(int argc, char **argv)
{
    flex_bison_log_file = fopen(BISON_FLEX_LOG_PATH, "w");

    if (argc > 2) read_target = fopen(argv[2], "r");
    else read_target = stdin;

    bool input_from_file;
    if ((input_from_file = argc > 1))
    {
        stdin = fopen(argv[1], "r");
    }

    char *s_expr_str = NULL;
    size_t s_expr_str_len = 0;
    size_t s_expr_postfix_padding = 2;
    YY_BUFFER_STATE buffer;

    while (true)
    {
        printf("\n> ");
        fflush(stdout);

        s_expr_str = NULL;
        s_expr_str_len = 0;
        yyreadline(&s_expr_str, &s_expr_str_len, stdin, s_expr_postfix_padding);

        while (s_expr_str[0] == '\n')
        {
            yyreadline(&s_expr_str, &s_expr_str_len, stdin, s_expr_postfix_padding);
        }

        if (input_from_file)
        {
            yyprintline(s_expr_str, s_expr_str_len, s_expr_postfix_padding);
        }

        buffer = yy_scan_buffer(s_expr_str, s_expr_str_len);

        yyparse();

        yy_flush_buffer(buffer);
        yy_delete_buffer(buffer);
        free(s_expr_str);
    }
}
