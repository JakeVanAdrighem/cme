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
    Token **new_tokbuf = malloc(sizeof(Token*) * newbufsize);
    for(int i = 0; i < tokbuf->count; i++){
      new_tokbuf[i] = tokbuf->tokens[i];
    }
    tokbuf->tokens = new_tokbuf;
    free(tokens);
    tokbuf->bufsize = newbufsize;
  }
  tokbuf->tokens[tokbuf->count] = tkn;
  tokbuf->count += 1;
  return true;
}

static void skipWhitespace(Unit *TU){
  while(look(TU) == ' ' || look(TU) == '\n'){
    advance(TU);
  }
}

TokenBuffer* lexFile(Unit *TU){
  TokenBuffer* tokbuf = calloc(sizeof(TokenBuffer), 1);
  int numtokens = 256;
  tokbuf->tokens = calloc(sizeof(Token*), numtokens);
  tokbuf->count = 0;
  tokbuf->bufsize = numtokens * sizeof(Token*);
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
    char* strHeadPtr = TU->cursor;
    int strlen = findNext(TU, '"');
    if (strlen == 0)
      ERROR("Incomplete String Literal");
    Token *tok = CREATE_TOKEN(TOK_STRING_LITERAL);
    // Copy string value
    tok->value = calloc(sizeof(char), strlen + 1);
    strncpy(tok->value, strHeadPtr, strlen);
    return tok;
  }
  return NULL;
}

// TODO: Handle negative numbers
static Token *isMaybeIntegerConstant(Unit *TU) {
  if (look(TU) >= '0' && look(TU) <= '9') {
    int *val = calloc(sizeof(int), 1);
    (*val) = atoi(TU->cursor);
    while (look(TU) >= '0' && look(TU) <= '9') {
      advance(TU);
    }
    Token *tok = CREATE_TOKEN(TOK_INTEGER_CONSTANT);
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
    // FIXME: This is stupid. But we can't
    // have an identifier that's just a single underscore.
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

bool actuallyPunctuator(Unit *TU, char *str) {
  if (!strncmp(TU->cursor, str, strlen(str))) {
    advanceTo(TU, strlen(str));
    return true;
  }
  return false;
}

static Token *isMaybePunctuator(Unit *TU) {
  Token *tok = NULL;
  switch (look(TU)) {
  case '+':
    if (actuallyPunctuator(TU, "++")) {
      tok = CREATE_TOKEN(TOK_INCREMENT);
    } else if (actuallyPunctuator(TU, "+=")) {
      tok = CREATE_TOKEN(TOK_PLUS_EQ);
    } else {
      tok = CREATE_TOKEN(TOK_PLUS);
      advance(TU);
    }
    break;
  case '-':
    if (actuallyPunctuator(TU, "--")) {
      tok = CREATE_TOKEN(TOK_DECREMENT);
    } else if (actuallyPunctuator(TU, "-=")) {
      tok = CREATE_TOKEN(TOK_MINUS_EQ);
    } else if (actuallyPunctuator(TU, "->")) {
      tok = CREATE_TOKEN(TOK_ARROW);
    } else {
      tok = CREATE_TOKEN(TOK_MINUS_EQ);
      advance(TU);
    }
    break;
  case '*':
    if (actuallyPunctuator(TU, "*=")) {
      tok = CREATE_TOKEN(TOK_TIMES_EQ);
    } else {
      tok = CREATE_TOKEN(TOK_DEREFERENCE);
      advance(TU);
    }
    break;
  case '/':
    if (actuallyPunctuator(TU, "/=")) {
      tok = CREATE_TOKEN(TOK_DIVIDE_EQ);
    } else {
      tok = CREATE_TOKEN(TOK_DIVIDE);
      advance(TU);
    }
    break;
  case '!':
    if (actuallyPunctuator(TU, "!=")) {
      tok = CREATE_TOKEN(TOK_NEQ);
    } else {
      tok = CREATE_TOKEN(TOK_BANG);
      advance(TU);
    }
    break;
  case '=':
    if (actuallyPunctuator(TU, "==")) {
      tok = CREATE_TOKEN(TOK_EQ);
    } else {
      tok = CREATE_TOKEN(TOK_ASSIGNMENT);
      advance(TU);
    }
    break;
  case '%':
    if (actuallyPunctuator(TU, "%=")){
      tok = CREATE_TOKEN(TOK_MODULO_EQ);
    } else {
      tok = CREATE_TOKEN(TOK_PERCENT);
      advance(TU);
    }
    break;
  case '<':
    if (actuallyPunctuator(TU, "<<=")) {
      tok = CREATE_TOKEN(TOK_LSHIFT_EQ);
    } else if (actuallyPunctuator(TU, "<<")) {
      tok = CREATE_TOKEN(TOK_LSHIFT);
    } else if (actuallyPunctuator(TU, "<=")) {
      tok = CREATE_TOKEN(TOK_LEQ);
    } else {
      tok = CREATE_TOKEN(TOK_LANGLE);
      advance(TU);
    }
    break;
  case '>':
    if (actuallyPunctuator(TU, ">>=")) {
      tok = CREATE_TOKEN(TOK_RSHIFT_EQ);
    } else if (actuallyPunctuator(TU, ">>")) {
      tok = CREATE_TOKEN(TOK_RSHIFT);
    } else if (actuallyPunctuator(TU, ">=")) {
      tok = CREATE_TOKEN(TOK_GEQ);
    } else {
      tok = CREATE_TOKEN(TOK_RANGLE);
      advance(TU);
    }
    break;
  case '&':
    if (actuallyPunctuator(TU, "&=")) {
      tok = CREATE_TOKEN(TOK_BITWISE_AND_EQ);
    } else if (actuallyPunctuator(TU, "&&")) {
      tok = CREATE_TOKEN(TOK_AND);
    } else {
      tok = CREATE_TOKEN(TOK_AMPERSAND);
      advance(TU);
    }
    break;
  case '^':
    if (actuallyPunctuator(TU, "^=")) {
      tok = CREATE_TOKEN(TOK_BITWISE_NOT_EQ);
    } else {
      tok = CREATE_TOKEN(TOK_BITWISE_NOT);
      advance(TU);
    }
    break;
  case '|':
    if (actuallyPunctuator(TU, "||")) {
      tok = CREATE_TOKEN(TOK_OR);
    } else if (actuallyPunctuator(TU, "|=")) {
      tok = CREATE_TOKEN(TOK_BITWISE_OR_EQ);
    } else {
      tok = CREATE_TOKEN(TOK_BITWISE_OR);
      advance(TU);
    }
    break;
  }

  if (tok)
    return tok;
  else if (actuallyPunctuator(TU, "["))
    tok = CREATE_TOKEN(TOK_LSQUARE);
  else if (actuallyPunctuator(TU, "]"))
    tok = CREATE_TOKEN(TOK_RSQUARE);
  else if (actuallyPunctuator(TU, "("))
    tok = CREATE_TOKEN(TOK_LPAREN);
  else if (actuallyPunctuator(TU, ")"))
    tok = CREATE_TOKEN(TOK_RPAREN);
  else if (actuallyPunctuator(TU, "{"))
    tok = CREATE_TOKEN(TOK_LBRACE);
  else if (actuallyPunctuator(TU, "}"))
    tok = CREATE_TOKEN(TOK_RBRACE);
  else if (actuallyPunctuator(TU, "."))
    tok = CREATE_TOKEN(TOK_DOT);
  else if (actuallyPunctuator(TU, ";"))
    tok = CREATE_TOKEN(TOK_SEMICOLON);
  else if (actuallyPunctuator(TU, ":"))
    tok = CREATE_TOKEN(TOK_COLON);
  else if (actuallyPunctuator(TU, "..."))
    tok = CREATE_TOKEN(TOK_ELIPSES);
  else if (actuallyPunctuator(TU, "~"))
    tok = CREATE_TOKEN(TOK_TILDA);
  else if (actuallyPunctuator(TU, "?"))
    tok = CREATE_TOKEN(TOK_QMARK);
  else if (actuallyPunctuator(TU, ","))
    tok = CREATE_TOKEN(TOK_COMMA);
  else
    return NULL;
  return tok;
}

bool actuallyKeyword(Unit *TU, char* str) {
  if (!strncmp(TU->cursor, str, strlen(str)) &&
      !isIdentifierValue(peek(TU, strlen(str)))) {
    advanceTo(TU, strlen(str));
    return true;
  }
  return false;
}

// FIXME: Ensure that the keyword we've found
// isn't simply a prefix of an identifier in
// a general way. Currently we have to check
// after each strncmp which is HORRIBLE.
// FIXME: Implement all keywords. Otherwise
// they end up as identifiers.
static Token *isMaybeKeyword(Unit *TU) {
  // Zero if lhs and rhs compare equal.
  if (actuallyKeyword(TU, "if")) {
    return CREATE_TOKEN(TOK_IF);
  } else if (actuallyKeyword(TU, "int")) {
    return CREATE_TOKEN(TOK_INT);
  } else if (actuallyKeyword(TU, "char")) {
    return CREATE_TOKEN(TOK_CHAR);
  } else if (actuallyKeyword(TU, "do")) {
    return CREATE_TOKEN(TOK_DO);
  } else if (actuallyKeyword(TU, "return")) {
    return CREATE_TOKEN(TOK_RETURN);
  } else if (actuallyKeyword(TU, "for")) {
    return CREATE_TOKEN(TOK_FOR);
  } else if (actuallyKeyword(TU, "while")) {
    return CREATE_TOKEN(TOK_WHILE);
  } else if (actuallyKeyword(TU, "else")) {
    return CREATE_TOKEN(TOK_ELSE);
  } else if (actuallyKeyword(TU, "static")) {
    return CREATE_TOKEN(TOK_STATIC);
  } else if (actuallyKeyword(TU, "struct")) {
    return CREATE_TOKEN(TOK_STRUCT);
  } else if (actuallyKeyword(TU, "extern")) {
    return CREATE_TOKEN(TOK_EXTERN);
  } else if (actuallyKeyword(TU, "long")) {
    return CREATE_TOKEN(TOK_LONG);
  } else if (actuallyKeyword(TU, "inline")) {
    return CREATE_TOKEN(TOK_INLINE);
  }
  return NULL;
}

static Token *isMaybeCharacterConstant(Unit *TU) {
  Token* tok;
  if(look(TU) == '\''){
    advance(TU);
    // Single escaped character.
    if(look(TU) == '\\' && peek(TU, 2) == '\'')
      advance(TU);
    // Multi-character constants not supported!
    if(peek(TU, 1) != '\'')
      ERROR("Incomplete character constant.");
    tok = calloc(sizeof(Token), 1);
    // Copy single character into value
    tok->value = malloc(sizeof(char));
    strncpy(tok->value, TU->cursor, 1);
    tok->type = TOK_CHARACTER_CONSTANT;
    advanceTo(TU, 2);
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
  return NULL;
}
