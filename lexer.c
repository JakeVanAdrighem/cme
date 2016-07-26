#include "cme.h"

// TODO: Make this a macro or something
Token* CREATE_TOKEN(TOK_TYPE type){
  Token* val = (Token*)calloc(sizeof(Token), 1);
  val->type = type;
  return val;
}

static bool
addTokenToBuffer(TokenBuffer *tokbuf, Token *tkn) {
  // Resize the token buffer
  if(tokbuf->bufsize == tokbuf->count){
    // Hold onto our tokens for now
    Token **tokens = tokbuf->tokens;
    // New buffer is sizeof(current_buffer) * 2
    int newbufsize = tokbuf->bufsize * 2;
    Token **new_tokbuf = malloc(sizeof(TokenBuffer) * newbufsize);
    for(int i = 0; i < tokbuf->count; i++){
      new_tokbuf[i] = tokbuf->tokens[i];
    }
    tokbuf->tokens = new_tokbuf;
    free(tokens);
    tokbuf->bufsize = newbufsize;
  }
  tokbuf->tokens[tokbuf->count+1] = tkn;
  tokbuf->count += 1;
  return true;
}

static void skipWhitespace(Unit *TU){
  while(look(TU) == ' ' || look(TU) == '\n'){
    advance(TU);
  }
}

TokenBuffer* lexFile(Unit *TU){
  TokenBuffer* tokbuf = malloc(sizeof(TokenBuffer));
  tokbuf->tokens = malloc(sizeof(Token) * 1024);
  tokbuf->count = 0;
  Token* tok;
  // Once before lexing and then
  // once each time after getting
  // a token.
  skipWhitespace(TU);
  DEBUG("Lexing...");
  while (!atEOF(TU)) {
    tok = getToken(TU);
    if(tok)
      addTokenToBuffer(tokbuf, tok);
    else
      return tokbuf;
    skipWhitespace(TU);
  }
  return tokbuf;
}

static Token* isMaybePreprocessorToken(Unit *TU){
  Token* tok;
  if(look(TU) == '#'){
    if(peek(TU,1) == 'i' && peek(TU, 2) == 'f'){
     tok = CREATE_TOKEN(PTOK_IF);
      advanceTo(TU, 3);
      return tok;
     }
  }
  return NULL;
}

// TODO: Need to format escaped characters
// and any other special formatting we might
// find inside of the string. This should be
// done right after we've grabbed the string
// itself via some helpers or something.
static Token *isMaybeStringLiteral(Unit *TU) {
  if (look(TU) == '"') {
    advance(TU);
    // char* cursorPtr = TU->cursor;
    int strlen = findNext(TU, '"');
    if (strlen == 0)
      ERROR("Incomplete String Literal");
    Token *tok = CREATE_TOKEN(TOK_STRING_LITERAL);
    // Copy string value
    // tok->value = ...
    advance(TU);
    return tok;
  }
  return NULL;
}

// TODO: Handle negative numbers
static Token* isMaybeIntegerConstant(Unit* TU){
  if (look(TU) >= '0' && look(TU) <= '9') {
    int *val = calloc(sizeof(int), 1);
    while (look(TU) >= '0' && look(TU) <= '9') {
      (*val) = (*val) + atoi(TU->cursor);
      (*val) = (*val) << 2;
      advance(TU);
    }
        Token* tok = CREATE_TOKEN(TOK_INTEGER_CONSTANT);
    tok->value = val;
    return tok;
  }
  return NULL;
}

bool isIdentifierNondigit(char in) {
  if ((in >= 'a' && in <= 'z') || (in >= 'A' && in <= 'Z') || in == '_')
    return true;
  return false;
}

bool isIdentifierDigit(char in) {
  if (in >= '0' && in <= '9')
    return true;
  return false;
}

inline bool isIdentifierValue(char in){
  return (isIdentifierDigit(in) || isIdentifierNondigit(in));
}

static Token *isMaybeIdentifier(Unit *TU) {
  if (isIdentifierNondigit(look(TU))) {
    int peekCount = 1;
    while (isIdentifierValue(peek(TU, peekCount))) {
      peekCount++;
    }
    // FIXME: This is stupid.
    // Can't have an identifier that's just a single underscore.
    if (peekCount == 1 && look(TU) == '_')
      return NULL;
    // Allocate + 1 so we have space for '\0'
    char* val = calloc(sizeof(char), peekCount + 1);
    strncpy(val, TU->cursor, peekCount);
    Token *tok = CREATE_TOKEN(TOK_IDENTIFIER);
    tok->value = val;
    advanceTo(TU, peekCount);
    return tok;
  }
  return NULL;
}

static Token *isMaybePunctuator(Unit *TU) {
  Token *tok = NULL;
  switch (look(TU)) {
  case '+':
    if (peek(TU, 1) == '+') {
            tok = CREATE_TOKEN(TOK_INCREMENT);
      advanceTo(TU, 2);
      return tok;
    } else if (peek(TU, 1) == '=') {
            tok = CREATE_TOKEN(TOK_PLUS_EQ);
      advanceTo(TU, 2);
      return tok;
    } else {
            tok = CREATE_TOKEN(TOK_PLUS);
      advance(TU);
      return tok;
    }
  case '-':
    if (peek(TU, 1) == '-') {
      tok = CREATE_TOKEN(TOK_DECREMENT);
      advanceTo(TU, 2);
      return tok;
    } else if (peek(TU, 1) == '=') {
      tok = CREATE_TOKEN(TOK_MINUS_EQ);
      advanceTo(TU, 2);
      return tok;
    } else if (peek(TU, 1) == '>'){
            tok = CREATE_TOKEN(TOK_ARROW);
      advanceTo(TU, 2);
      return tok;
    } else {
            tok = CREATE_TOKEN(TOK_MINUS_EQ);
      advance(TU);
      return tok;
    }
  case '*':
    if (peek(TU, 1) == '=') {
            tok = CREATE_TOKEN(TOK_TIMES_EQ);
      advanceTo(TU, 2);
      return tok;
    } else {
            tok = CREATE_TOKEN(TOK_DEREFERENCE);
      advance(TU);
      return tok;
    }
  case '!':
    if(peek(TU, 1) == '='){
    tok = CREATE_TOKEN(TOK_NEQ);
    advanceTo(TU, 2);
    return tok;
  } else {
    tok = CREATE_TOKEN(TOK_BANG);
    advance(TU);
    return tok;
  }
  case '=':
    if (peek(TU, 1) == '=') {
      tok = CREATE_TOKEN(TOK_EQ);
      advanceTo(TU, 2);
      return tok;
    } else {
      tok = CREATE_TOKEN(TOK_ASSIGNMENT);
      advance(TU);
      return tok;
    }
  case '[':
    tok = CREATE_TOKEN(TOK_LSQUARE);
    advance(TU);
    return tok;
  case ']':
    tok = CREATE_TOKEN(TOK_RSQUARE);
    advance(TU);
    return tok;
  case '(':
    tok = CREATE_TOKEN(TOK_LPAREN);
    advance(TU);
    return tok;
   case ')':
    tok = CREATE_TOKEN(TOK_RPAREN);
    advance(TU);
    return tok;
   case '<':
    tok = CREATE_TOKEN(TOK_LANGLE);
    advance(TU);
    return tok;
   case '>':
    tok = CREATE_TOKEN(TOK_RANGLE);
    advance(TU);
    return tok;
  case '{':
    tok = CREATE_TOKEN(TOK_LBRACE);
    advance(TU);
    return tok;
  case '}':
    tok = CREATE_TOKEN(TOK_RBRACE);
    advance(TU);
    return tok;
  case ';':
    tok = CREATE_TOKEN(TOK_SEMICOLON);
    advance(TU);
    return tok;
  }
  return NULL;
}

// FIXME: Ensure that the keyword we've found
// isn't simply a prefix of an identifier in
// a general way. Currently we have to check
// after each strncmp which is HORRIBLE.
// FIXME: Implement all keywords. Otherwise
// they end up as identifiers.
static Token *isMaybeKeyword(Unit *TU) {
  switch (look(TU)) {
  case 'i':
    // Zero if lhs and rhs compare equal.
    if (!strncmp(TU->cursor, "if", 2)) {
      if(isIdentifierValue(peek(TU, 2)))
        return NULL;
      advanceTo(TU, 2);
      return CREATE_TOKEN(TOK_IF);
    }
    else if (!strncmp(TU->cursor, "int", 3)){
      if(isIdentifierValue(peek(TU, 3)))
        return NULL;
      advanceTo(TU, 3);
      return CREATE_TOKEN(TOK_INT);
    }
  case 'c':
    if (!strncmp(TU->cursor, "char", 4)){
      if(isIdentifierValue(peek(TU, 4)))
        return NULL;
      advanceTo(TU, 4);
      return CREATE_TOKEN(TOK_CHAR);
    }
  case 'r':
    if (!strncmp(TU->cursor, "return", 6)) {
      if (isIdentifierValue(peek(TU, 6)))
        return NULL;
      advanceTo(TU, 6);
      return CREATE_TOKEN(TOK_RETURN);
    }
  case 'f':
    if (!strncmp(TU->cursor, "for", 3)) {
      if (isIdentifierValue(peek(TU, 3)))
        return NULL;
      advanceTo(TU, 3);
      return CREATE_TOKEN(TOK_RETURN);
    }
  default:
    return NULL;
  }
  return NULL;
}

static Token *isMaybeCharacterConstant(Unit *TU) {
  Token* tok;
  char curChar = look(TU);
  if(curChar == '\''){
    //char *cursorPtr = TU->cursor;
    advance(TU);
    int charlen = findNext(TU, '\'');
    if(charlen == 0)
      ERROR("Incomplete character constant.");
    // Copy string from cursorPtr to
    // cursorPtr + charlen.
    tok = malloc(sizeof(Token));
    tok->type = TOK_CHARACTER_CONSTANT;
    advance(TU);
    return tok;
  }
  return NULL;
}

Token* getToken(Unit *TU) {
  Token* tkn = NULL;
  if ((tkn = isMaybePreprocessorToken(TU)))
    return tkn;
  if((tkn = isMaybePunctuator(TU)))
    return tkn;
  if((tkn = isMaybeStringLiteral(TU)))
    return tkn;
  if((tkn = isMaybeCharacterConstant(TU)))
    return tkn;
  if((tkn = isMaybeKeyword(TU)))
    return tkn;
  if((tkn = isMaybeIntegerConstant(TU)))
    return tkn;
  if((tkn = isMaybeIdentifier(TU)))
    return tkn;
  STRERROR("Lexer failure", TU->cursor);
  printf("Cursor: %p, End: %p\n", TU->cursor, TU->end);
  return NULL;
}
