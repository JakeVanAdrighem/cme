%{

#include "parser.c"

%}

%debug
%defines
%error-verbose
%token-table
%verbose

%token TOK_VOID TOK_BOOL TOK_CHAR TOK_INT TOK_STRING
%token TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_FALSE TOK_TRUE TOK_NULL TOK_NEW TOK_ARRAY
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token TOK_BLOCK TOK_CALL TOK_IFELSE TOK_INITDECL
%token TOK_POS TOK_NEG TOK_NEWARRAY TOK_TYPEID TOK_FIELD
%token TOK_ORD TOK_CHR TOK_ROOT

%token TOK_RETURNVOID TOK_PARAMLIST TOK_PROTOTYPE TOK_DECLID
%token TOK_NEWSTRING TOK_VARDECL TOK_INDEX TOK_FUNCTION

%right    TOK_IFELSE TOK_IF TOK_ELSE
%right    '='
%left     TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left     '+' '-'
%left     '*' '/' '%'
%right    TOK_NEG TOK_POS '!' TOK_ORD TOK_CHR
%left     TOK_FUNCTION TOK_ARRAY TOK_FIELD
%left     '[' '.'

%nonassoc TOK_NEW
%nonassoc '('

%start start

%%

start	: program			{ yyparse_ast = $1; };

program : program structdef { $$ = adopt1($1, $2); }
		| program function  { $$ = adopt1($1, $2); }
		| program statement { $$ = adopt1($1, $2); }
		| program error '}' { $$ = $1; }
		| program error ';' { $$ = $1; }
		| 					{ $$ = new_empty_ast(); }
		;



basetype	: TOK_VOID		{$$ = $1;}
			| TOK_BOOL		{$$ = $1;}
			| TOK_CHAR		{$$ = $1;}
			| TOK_INT		{$$ = $1;}
			| TOK_STRING	{$$ = $1;}
			| TOK_IDENT		{$$ = changesymbol($1, TOK_TYPEID);}
			;

structdef   : TOK_STRUCT TOK_IDENT '{' structfields '}' ';'
			;

structfields: structfields structfield	{ }
			| structfield				{$$ =  }
			; 

structfield : basetype TOK_IDENT ';'			{ }
			| basetype TOK_ARRAY TOK_IDENT ';'	{ }
			| 
			;
