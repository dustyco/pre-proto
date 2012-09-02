#include "hex.h"
#include <string.h>
#include <stdio.h>

char* hexify(char* buf, size_t len) {
	size_t l = len*2+1;
	char* out = (char*)malloc(l);
	memset(out, 0, l);
	for(int i=0;i<len;i++) {
		snprintf(&out[i*2], 3, "%02hhx", buf[i]);
	}
	return out;
}

