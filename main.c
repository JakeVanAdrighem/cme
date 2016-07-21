//Jake VanAdrighem
//Driver for cme c compiler
#include "cme.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    // throw error
  } else {
    Unit TU;

    if (!loadFile(argv[1], &TU)) {
      DEBUG("Failed loading file.");
      return 1;
    }

    if (!preprocessFile(&TU)) {
      DEBUG("Failed preprocessing file.");
      return 1;
    }
    // process cl options
    // either use unistd.h for getopt or
    // create our own command line option
    // handler.

    // Need to process the file and deal with multi-byte chars
    // and newlines. Then we can lex.

    // After lexing we can preprocess.
  }
}
