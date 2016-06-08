// Define the AST structure here.
// Should be a bottom up construction.
// i.e. instruction -> structure(BB or Loop maybe?) -> function -> module
// Though modules can have special children like globals
// Hopefully we can reach something like debuginfo as well

struct Parts{
	
}
// Defines the instruction type
// TODO: How do we define parts of instructions?
struct instruction{
	char* name;
	instruction* operands;
	//TODO: Long way off from handling debug info
	//DebugInfo* dbginf;
}

// TODO: it would really be good to have
// references to loops in the bb structure
// and the like.
struct bb{
	char* name; // Can be void(TODO: right now it always is)
	instruction* instrs;
	//loop* loops;
}

struct function{
	char* name; // Can be void
	bb* blocks;
		
}
