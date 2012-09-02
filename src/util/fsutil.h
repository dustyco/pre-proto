#pragma once
#include <stdlib.h>
#include <boost/filesystem.hpp>

// Read in all the data in a passed file
// and return it as a malloc()ed buffer
// of length boost::file_size(p) or NULL
// if we can't read all the data from the
// file.
char* readpath(boost::filesystem::path& p, size_t& s, bool binary=true);

// Write all the data in the given buffer
// to the passed path, and return success.
bool writepath(boost::filesystem::path& p, char* buf, size_t len, bool binary=true);
