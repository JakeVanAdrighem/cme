%{

#include "scanner.h"

#define YY_USER_ACTION { scanner_useraction(); }
#define IGNORE(THING) { }

%}

%option 8bit
%option ecs
%option debug
%option nodefault
%option nounput
%option noyywrap
%option perf-report
%option verbose
%option warn

LETTER        [A-Za-z_]
DIGIT         [0-9]
TOK_IDENT     ({LETTER}({LETTER}|{DIGIT})*)
TOK_CHARCON   ('([^\\'\n]|\\[\\'"0nt])') 
TOK_STRINGCON (\"([^\\"\n]|\\[\\'"0nt])*\")
TOK_INTCON    ({DIGIT}+)
BADSTRING     (\"[^\n\"\\]*(.|\n))
ABADCHAR      ('(.*)|\n')
BADCHAR       ('[\n\'\\]'|'.[^\'])
BADIDENT      ({TOK_INTCON}+{TOK_IDENT})

%%

"#include".*  { scanner_include(); }
[ \t]+        { IGNORE (white space) }
\n            { scanner_newline(); }

/* Basic Data Types */
"int"           { return yylval_token (TOK_INT);}
"char"          { return yylval_token (TOK_CHAR);}
"bool"          { return yylval_token (TOK_BOOL);}
"void"          { return yylval_token (TOK_VOID);}

/* Objects */
"struct"        { return yylval_token (TOK_STRUCT);}

/* Control Flow */
"if"            { return yylval_token (TOK_IF);}
"else"          { return yylval_token (TOK_ELSE);}
"while"         { return yylval_token (TOK_WHILE);}
"for"			{ return yylval_token (TOK_FOR);}
"return"        { return yylval_token (TOK_RETURN);}


"false"         { return yylval_token (TOK_FALSE);}
"true"          { return yylval_token (TOK_TRUE);}
"null"          { return yylval_token (TOK_NULL);}
"new"           { return yylval_token (TOK_NEW);}
"="             { return yylval_token ('='); }
"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"^"             { return yylval_token ('^'); }
","             { return yylval_token (','); }
"."             { return yylval_token ('.'); }
"["             { return yylval_token ('['); }
"]"             { return yylval_token (']'); }
"{"             { return yylval_token ('{'); }
"}"             { return yylval_token ('}'); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
";"             { return yylval_token (';'); }
"<"             { return yylval_token (TOK_LT);}
">"             { return yylval_token (TOK_GT);}
"=="            { return yylval_token (TOK_EQ);}
"!="            { return yylval_token (TOK_NE);}
"<="            { return yylval_token (TOK_LE);}
">="            { return yylval_token (TOK_GE);}
"!"             { return yylval_token ('!');}
"%"             { return yylval_token ('%');}
"[]"            { return yylval_token (TOK_ARRAY);}

{TOK_IDENT}     { return yylval_token (TOK_IDENT);}
{TOK_CHARCON}   { return yylval_token (TOK_CHARCON);}
{TOK_STRINGCON} { return yylval_token (TOK_STRINGCON);}
{TOK_INTCON}    { return yylval_token (TOK_INTCON);}

{BADIDENT}      { scanner_badtoken(yytext);}
{BADCHAR}       { scanner_badtoken(yytext);}
{BADSTRING}     { scanner_badtoken(yytext);}

<<EOF>>         {scanner_eof();}

.               { scanner_badtoken(yytext);}

%%

