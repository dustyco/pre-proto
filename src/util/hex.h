#pragma once
#include <stdlib.h>

// Convert a binary buffer to hex
// Returns a newly-malloced string buffer
char* hexify(char* buf, size_t len);
