#include "cme.h"

// TODO:
// Phase 1. C11 N1570 5.1.1.2 p1
// So here we're supposed to convert
// source multi-byte chars to our
// implementation of them.
bool phase1(Unit *TU){
  if(!TU->cursor)
	  ERROR("Phase 1 of preprocessor not implemented.");
  ERROR("Phase 1 of preprocessor not implemented.");
  return true;
}

// Phase 2. C11 N1570 5.1.1.2 p2
bool phase2(Unit *TU) {
  while (!atEOF(TU)) {
    if (look(TU) == '/') {
      switch (peek(TU, 1)) {
      // Single line comment.
      case '/':
        while (look(TU) != '\n') {
          overwrite(TU, ' ');
          advance(TU);
        }
        overwrite(TU, ' ');
        advance(TU);
        break;
      // Multi-line comment.
      case '*':
        while (look(TU) != '*' && peek(TU, 1) != '/') {
          overwrite(TU, ' ');
          advance(TU);
        }
        overwriteTo(TU, ' ', 2);
        advanceTo(TU, 2);
        break;
      }
    } else {
      advance(TU);
    }
  }
  // Reset the cursor to the beinning of the file
  TU->cursor = TU->file;
  return true;
}

// Phase 3. C11 N1570 5.1.1.2 p3
bool phase3(Unit *TU){
  if (*TU->end != '\n') {
    ERROR("No newline at end of file.");
    return false;
  }
	return true;
}

bool preprocessFile(Unit *TU){
  bool success = false;
	// Phase 1
	// success |= phase1(TU)

	// Phase 2
	success |= phase2(TU);

	// Phase 3
	//success |= phase3(TU);
	return success;
}

bool compress(Unit *TU) {
  while (!atEOF(TU)) {
    if (look(TU) == ' ' && peek(TU, 1) == ' ') {
      // Nice to compress >= 2 adjacent whitespaces to
      // a single whitespace character.
      // This function will need to either do some crazy
      // shift of the entire memory mapped file or
      // reallocate a full size worth and copy the things
      // we want as "compression".
    } else {
      while (look(TU) == ' ')
        advance(TU);
      continue;
    }
    advance(TU);
  }
  return true;
}
