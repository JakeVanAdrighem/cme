//Jake VanAdrighem
//Driver for cme c compiler
#include "cme.h"

bool loadFile(char *filename, Unit *TU) {
  fpos_t filelen;
  FILE *fin = fopen(filename, "r");
  if (!fin) {
    STRERROR("Couldn't open file", filename);
    return false;
  }

  if (fseek(fin, 0, SEEK_END)) {
		ERROR("Couldn't seek to end of file.");
		fclose(fin);
    return false;
  }
	
  if (fgetpos(fin, &filelen)) {
		ERROR("Couldn't get file cursor position.");
		fclose(fin);
    return false;
  }

  if (filelen == (fpos_t)0) {
		ERROR("File length.");
		fclose(fin);
    return false;
  }

	TU->size = (int)filelen;
  // We have the character count now. Lets create
  // our internal buffer.
  char *loadedfile = (char *)malloc(sizeof(char) * TU->size);
  if (!loadedfile) {
		ERROR("Failed to malloc file size memory.");
		fclose(fin);
    return false;
  }

	// Now we have space, let's load the file up into cme.
	// First seek the the beginning of the file.
	if(fseek(fin, 0, SEEK_SET)){
		ERROR("Failed to seek to beginning of file.");
		fclose(fin);
		free(loadedfile);
		return false;
	}

	// Now we just read it all in one go.
	if(!fread(loadedfile, sizeof(char), (size_t)TU->size, fin)){
		ERROR("Couldn't read out file.");
		fclose(fin);
		free(loadedfile);
		return false;
	}

	fclose(fin);
	TU->file = loadedfile;
	return true;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    // throw error
  } else {
    Unit TU;
    if (!loadFile(argv[1], &TU)) {
      return 1;
    }

    if (!preprocessFile(&TU)) {
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
