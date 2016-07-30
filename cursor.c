#include "cme.h"

// This file is mostly helper functions used
// to clean up other parts of the code base.
char look(Unit *TU) {
  if (atEOF(TU)) {
    DEBUG("Looked at EOF");
    return (char)0;
  }
  return *(TU->cursor);
}

char peek(Unit *TU, int offset){
  // FIXME: This pointer compare is really not okay
  if (TU->cursor + offset > TU->end){
    DEBUG("Peeking at or past EOF!");
    return (char)0;
  }
  return *(TU->cursor + offset);
}

// Find the next non-escaped instance of chr
// in the stream.
// FIXME: This fucks up on simple cases: '\\'
int findNext(Unit *TU, char chr){
  int pos = 0;
  while(!atEOF(TU)){
    pos++;
    if (look(TU) == chr && peek(TU, -1) != '\\') {
      advance(TU);
      return pos;
    }
    advance(TU);
  }
  return 0;
}

void overwrite(Unit *TU, char chr){
	*TU->cursor = chr;
	return;
}

void overwriteTo(Unit *TU, char chr, int amount){
	for(int i = 0; i < amount; i++)
		*TU->cursor = chr;
	return;
} 

bool atEOF(Unit *TU){
  return TU->cursor == TU->end;
}

void advance(Unit *TU){
	TU->cursor = TU->cursor + 1;
}

void advanceTo(Unit *TU, int amount){
	TU->cursor = TU->cursor + amount;
}
