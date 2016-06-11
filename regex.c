// Simple regex handler. We need this to build the Finite State Machines
// used in scanning and parsing the source. A state machine greatly
// reduces the complexity of handling situations like = vs == in the 
// actual implementation. Using some kind of switch mechanism quickly
// becomes unwieldy.

//[<stuff>] defines a character class
// <-> indicates a rangewhen used between two characters in a
// character class.
// <*> indicates we should expect 0 or more of whatever it's applied to
// <+> indicates we should expect 1 or more of whatever it's applied to
// <\> escapes things

//SAMPLES:
//[a-zA-Z]*
