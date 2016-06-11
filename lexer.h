#include <cstdio.h>

#ifdef LEXER_VERBOSE
#define lex_echo(STR) printf(STR)
#else
#define lex_echo(STR) (void)STR
#endif

struct token{
	token_type type;
	char* value;	
};

bool read_token(FILE* fp, token *tkn);
