// Single source include file
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR(msg)                                                             \
  {                                                                            \
    printf("Error: %s\n", msg);                                                \
    exit(1);                                                                   \
  }

#define STRERROR(msg, args) printf("Error - %s: %s\n", msg, args)

#define DEBUG(msg) printf("%s\n", msg)

#define STRDEBUG(msg, args) printf("%s%s\n", msg, args)

// Token type enumeration
#include "tokens.def"

// Translation Unit structure
struct Unit {
  char *file;
  char *end;
  int size;
  char *cursor;
};

struct Token {
  enum TOK_TYPE type;
  void *value;
};
// C11 6.2.3 p1
typedef struct Unit Unit;
typedef struct Token Token;
typedef enum TOK_TYPE TOK_TYPE;
typedef struct TokenBuffer TokenBuffer;

struct TokenBuffer {
  Token **tokens;
  int bufsize;
  int count;
};

// file.c
bool loadFile(char *, Unit *);

// preprocessor.c
bool preprocessFile(Unit *);
bool compress(Unit *);

// lexer.c
TokenBuffer *lexFile(Unit *);
Token *getToken(Unit *);

// cursor.c
char look(Unit *);
char peek(Unit *, int);
int findNext(Unit*, char);
char readOne(Unit *);
void overwrite(Unit *, char);
void overwriteTo(Unit *, char, int);
bool atEOF(Unit *);
void advance(Unit *);
void advanceTo(Unit *, int);
