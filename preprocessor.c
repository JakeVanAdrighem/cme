#include "cme.h"

// TODO:
// Phase 1. C11 N1570 5.1.1.2 p1
// So here we're supposed to convert
// source multi-byte chars to our
// implementation of them.
bool phase1(Unit *TU){
	ERROR("Phase 1 of preprocessor not implemented.");
	return true;
}

// Phase 2. C11 N1570 5.1.1.2 p2
bool phase2(Unit *TU) {
  char *iter = TU->file;
  while (*iter != EOF) {
    // FIXME: If there is a '\' at the end
    // of the file, won't this delete it?
    if (*iter == '\\' && *(iter + 1) == '\n') {
      // FIXME: This isn't really a good pattern.
      // and is likely to end up happening all over
      // the place during preprocessing. Something
      // more generic and high level but still c-like
      // would be great to have here. The same
      // applies for the '\\n' check.
      *iter = ' ';
      *(iter + 1) = ' ';
    }
    iter++;
  }
  return true;
}

// Phase 3. C11 N1570 5.1.1.2 p3
bool phase3(Unit *TU){
	// Confirm the file ends in a newline
	if(*(TU->file + (TU->size - 1)) != '\n'){
		ERROR("File does not end in newline");
		return false;
	}
	// Decompose into preprocessing tokens
	char* iter = TU->file;
	bool instring = false;
	while(*iter != EOF){
		if(*iter == '#' && !instring){
			// Start of directive
			// #include #pragma
			// _Pragma TODO: Uggggghh
			// #define #defined
			// #if #elif #ifdef #ifndef #endif #undef
			// # ## 
			// #line #error
		}
		iter++;
	}
	return true;
}

bool isMaybePreprocesserDirective(char* str){
	return true;	
}

bool preprocessFile(Unit *TU){
	bool success = false;
	// Phase 1
	// success |= phase1(TU)

	// Phase 2
	success |= phase2(TU);

	// Phase 3
	success |= phase3(TU);
	return success;
}

bool compress(Unit *TU){
	return true;	
}
