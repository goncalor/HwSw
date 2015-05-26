#define FILE_END_CODE 0
//#define debug
#define MB
#define MAX_FILE_SIZE 20*1024*1024	// max 128 MB, external memory size
#define LAST_DIGIT 10


unsigned stats[256];	// for 128 MB we might have 2^27 occurrences of a single character, hence uint32
