#include "fsutil.h"

#include <stdio.h>
#include <string.h>

using namespace boost;

char* readpath(filesystem::path& p, size_t& sz, bool binary) {
	sz = file_size(p);
	char* obuf = (char*)malloc(sz);
	FILE* ifile;
	ifile = fopen(p.c_str(), (binary) ? ("rb") : ("r"));
	if(fread(obuf, 1, sz, ifile) != sz) {
		fclose(ifile);
		free(obuf);
		return NULL;
	}
	fclose(ifile);
	return obuf;
}

bool writepath(filesystem::path& p, char* buf, size_t len, bool binary) {
	FILE* ofile;
	ofile = fopen(p.c_str(), (binary) ? ("wb") : ("w"));
	if(fwrite(buf, 1, len, ofile) != len) {
		fclose(ofile);
		return false;
	}
	fclose(ofile);
	return true;
}

