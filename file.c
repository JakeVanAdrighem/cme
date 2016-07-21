// Jake VanAdrighem
// File loading and other stuff
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

  TU->size = (int)filelen + 1;
  // We have the character count now. Lets create
  // our internal buffer.
  char *loadedfile = (char *)malloc(TU->size * sizeof(char));
  if (!loadedfile) {
    ERROR("Failed to malloc file size memory.");
    fclose(fin);
    return false;
  }

  // Now we have space, let's load the file up into cme.
  // First seek the the beginning of the file.
  if (fseek(fin, 0, SEEK_SET)) {
    ERROR("Failed to seek to beginning of file.");
    fclose(fin);
    free(loadedfile);
    return false;
  }

  // Now we just read it all in one go.
  if (!fread(loadedfile, sizeof(char), (size_t)TU->size, fin)) {
    ERROR("Couldn't read out file.");
    fclose(fin);
    free(loadedfile);
    return false;
  }

  fclose(fin);
  TU->file = loadedfile;
  TU->end = loadedfile + (TU->size - 1);
  *TU->end = '0';
  TU->cursor = TU->file;
  return true;
}
