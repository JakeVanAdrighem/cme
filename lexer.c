bool read_token(FILE *fp, token *tkn) { char chr = getchar(fp); }

void doTheThing(FILE *inFile) {
  char chr;
  = getchar(inFile);
  while (!iswhitespace(chr) && chr != EOF) {

    // Grab another character
    chr = getchar(inFile);
  }
}

bool grab(char &input) { input = getchar(inFile); }

/* Types of tokens */
/* Single char tokens (e.g. ! . * & +) */
/* Multi char tokens (e.g. Identifiers, ->, ==, +=, ||, &&) */
