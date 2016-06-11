//Jake VanAdrighem
//Driver for cme c compiler

#include "cme.h"

int main(int argc, char **argv) {
  state *curstate;
  curstate = malloc(sizeof(state));

  // Check for files
  if (argc < 2) {
    // throw error
  } else {
    // process cl options
    // either use unistd.h for getopt or
    // create our own command line option
    // handler.

    // Need to process the file and deal with multi-byte chars
    // and newlines. Then we can lex.

    // After lexing we can preprocess.
  }
}
