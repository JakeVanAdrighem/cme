#include "cme.h"

static bool addTokenToBuffer(TokenBuffer* tokbuf, Token* tkn){
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
     tok = malloc(sizeof(Token)); 
      tok->type = PTOK_IF;
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
static Token* isMaybeStringLiteral(Unit* TU){
  if(look(TU) == '"'){
    advance(TU);
    // char* cursorPtr = TU->cursor;
    int strlen = findNext(TU, '"');
    if(strlen == 0)
      ERROR("Incomplete String Literal");
    Token* tok = (Token*)malloc(sizeof(Token));
    tok->type = TOK_STRING_LITERAL;
    // Copy string value
    // tok->value = ...
    advance(TU);
    DEBUG("TOK_STRING_LITERAL");
    return tok;
  }
  return NULL;
}

static Token* isMaybeIdentifier(Unit* TU){
  switch(look(TU)){
    return NULL; 
  }
  return NULL; 
}

static Token *isMaybePunctuator(Unit *TU) {
  Token *tok;
  switch (look(TU)) {
  case '+':
    if (peek(TU, 1) == '+') {
      DEBUG("TOK_INCREMENT");
      tok = malloc(sizeof(Token));
      tok->type = TOK_INCREMENT;
      advanceTo(TU, 2);
      return tok;
    }
    if (peek(TU, 1) == '=') {
      // TOK_INCREMENT
      tok = malloc(sizeof(Token));
      tok->type = TOK_INCREMENT;
      advanceTo(TU, 2);
      return tok;
    }
    if (peek(TU, 1) == '=')
      return NULL;
    // increment assign token
    // return token for addition operator
    case '-':
      if(peek(TU, 1) == '-'){
        tok = malloc(sizeof(Token));
        tok->type = TOK_DECREMENT;
      }
  }
  return NULL;
}

static Token* isMaybeKeyword(Unit* TU){
  switch(look(TU)){
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
    DEBUG("TOK_CHARACTER_CONSTANT");
    advance(TU);
    return tok;
  }
  return NULL;
}

Token* getToken(Unit *TU) {
  Token* tkn = NULL;
  if ((tkn = isMaybePreprocessorToken(TU)))
    return tkn;
  if((tkn = isMaybeStringLiteral(TU)))
    return tkn;
  if((tkn = isMaybeIdentifier(TU)))
    return tkn;
  if((tkn = isMaybePunctuator(TU)))
    return tkn;
  if((tkn = isMaybeKeyword(TU)))
    return tkn;
  if((tkn = isMaybeCharacterConstant(TU)))
    return tkn;
  STRERROR("Lexer failure", TU->cursor);
  printf("Cursor: %p, End: %p\n", TU->cursor, TU->end);
  return NULL;
}
