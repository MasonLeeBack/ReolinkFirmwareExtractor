/*

Copyright (C) 2024 Mason Lee Back

File name: pakfile.h

*/

#ifndef __PAKFILE_H_
#define __PAKFILE_H_

#include <stdint.h>

enum PakType {
	Gen1,
	Gen2
};

struct PakHeader {
	uint32_t Magic;
	uint32_t Unknown1;
	uint32_t Unknown2;
};

#define PAKMAGIC_EX 0x32725913
struct PakHeaderEx {
	uint64_t Magic;
	uint64_t Unknown1;
	uint64_t Unknown2;
};

struct TocEntry {
	char fileName[32];
	char versionString[24];
	uint32_t fileOffset;
	uint32_t fileSize;
};

struct TocEntryEx {
	char fileName[32];
	char versionString[24];
	uint64_t fileOffset;
	uint64_t fileSize;
};

#endif // __PAKFILE_H_
