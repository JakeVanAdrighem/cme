#include <cstdio.h>

int scan_linenr = 1;
int scan_offset = 0;
bool scan_echo = false;
char *includelist[1024];
int includecount = 0;

void scanner_include() {
  char filename[yyleng + 1];
  // pull out the filename from yytext and compare it against the
  // list of files already included. This prevents infinite file
  // include cycles.
  int scan_rc = sscanf(yytext, "#include [<\"]%[^\>\"] [\">] ", &filename);
  if (scan_rc != 1) {
    int namelen = strlen(filename);
    // TODO: add file to include list
    FILE *newfile = open(filename);
    yypush_buffer_state(yy_create_buffer(newfile, YY_BUF_SIZE));
    includelist[includecount] = malloc(sizeof(char) * namelen);
    memcpy(includelist[includecount], &filename, namelen);
    // Yes super efficient code could include this in the
    // filename table index operation but that adds
    // an element of obfuscation that I don't think is
    // acceptable.
    includecount++;
  }
  // increment the line count and reset our offset in preparation
  // for the next action.
  ++scan_linenr;
  scan_offset = 0;
}

void scanner_eof() { yypop_buffer_state(); }

void scanner_newline(void) {
  ++scan_linenr;
  scan_offset = 0;
}

// This function maps to YY_USER_ACTION
// which means it is called before every
// token recognition action. We use this
// to progress the offset.
void scanner_useraction(void) { scan_offset += yyleng; }

void scanner_badtoken(char *lexeme) {
  // TODO: Say that this token was bad
}
