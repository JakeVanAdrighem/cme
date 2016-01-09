// Single source include file

//scanner.c
void scanner_preproc_include(void);
void scanner_newline(void);
void scanner_setecho(bool);
void scanner_useraction(void);
void scanner_badchar(unsigned char);
void scanner_badtoken(char*);
int  yylval_token(int);
void yyerror(const char*);
void error_destructor(astree_node*);
astree_node* new_treeroot(void);
