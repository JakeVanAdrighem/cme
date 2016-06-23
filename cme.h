// Single source include file
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define ERROR(msg) printf("Error: %s\n", msg)
#define STRERROR(msg, args) printf("Error: %s - %s\n", msg, args)

//Translation Unit structure
struct Unit {
	char* file;
	int size;
};

// Move Unit from tag namespace to ordinary namespace.
// C11 N1570 6.2.3 p1
typedef struct Unit Unit;

// preprocessor.c
bool preprocessFile(Unit *TU);
bool compress(Unit *TU);
