/*

Copyright (C) 2024 Mason Lee Back

File name: main.cpp

*/

#include "pakfile.h"

#include <iostream>
#include <fstream>
#include <vector>

PakType type;

void usage(char* name)
{
  printf("%s (file).pak\n", name);
}

bool detect_pak(std::fstream* stream)
{
  printf("Attempting to detect pak..\n");

  uint64_t header;
  stream->read((char*)&header, sizeof(uint64_t));

  if (header == PAKMAGIC_EX) {
    printf("Gen2 pak found. Extracting..\n");
    type = PakType::Gen2;
  }
  else {
    printf("Gen1 pak found. Extracting...\n");
    type = PakType::Gen1;
  }

  return false;
}

bool write_file(std::fstream* stream, const char* fileName, uint64_t offset, uint64_t size) {
  std::ofstream outputFile(fileName, std::ios::binary);
  if (!outputFile.is_open()) {
    std::cerr << "Error: Could not open output file: " << fileName << std::endl;
    return false;
  }

  stream->seekg(offset, std::ios::beg);
  if (!(*stream)) {
    std::cerr << "Error: Failed to seek to the offset in input stream." << std::endl;
    return false;
  }

  std::vector<char> buffer(size);
  stream->read(buffer.data(), size);
  if (!(*stream)) {
    std::cerr << "Error: Failed to read the specified amount of data from the input stream." << std::endl;
    return false;
  }

  outputFile.write(buffer.data(), size);
  if (!outputFile) {
    std::cerr << "Error: Failed to write data to the output file." << std::endl;
    return false;
  }

  outputFile.close();

  return true;
}

bool extract_gen1(std::fstream* stream) {
  stream->seekg(12, std::ios::beg);

  std::vector<TocEntry> entries;

  // 14 entries
  for (int i = 0; i < 14; i++) {
    TocEntry entry;
    stream->read((char*)&entry, sizeof(TocEntry));

    if (entry.fileName[0] == '\0') {
      continue;
    }

    printf("File name: %s\nVersion: %s\nFile offset: 0x%x\nFile size: 0x%x\n",
      entry.fileName,
      entry.versionString,
      entry.fileOffset,
      entry.fileSize);

    entries.push_back(entry);
  }

  // Valid entries
  for (int i = 0; i < entries.size(); i++) {
    TocEntry* entry = &entries[i];
    if (!write_file(stream, entry->fileName, entry->fileOffset, entry->fileSize))
      return false;
  }

  return true;
}

bool extract_gen2(std::fstream* stream) {
  stream->seekg(24, std::ios::beg);

  std::vector<TocEntryEx> entries;

  // 13 entries
  for (int i = 0; i < 13; i++) {
    TocEntryEx entry;
    stream->read((char*)&entry, sizeof(TocEntryEx));

    if (entry.fileName[0] == '\0') {
      continue;
    }

    printf("File name: %s\nVersion: %s\nFile offset: 0x%llu\nFile size: 0x%llu\n",
      entry.fileName,
      entry.versionString,
      entry.fileOffset,
      entry.fileSize);

    entries.push_back(entry);
  }

  for (int i = 0; i < entries.size(); i++) {
    TocEntryEx* entry = &entries[i];
    if (!write_file(stream, entry->fileName, entry->fileOffset, entry->fileSize))
      return false;
  }

  return true;
}

int main(int argc, char* argv[])
{
  bool result = false;

  if (argc == 1) {
    usage(argv[0]);

    return -1;
  }

  std::fstream stream;
  stream.open(argv[1], std::ios::binary | std::ios::in);

  if (!stream) {
    printf("Failed to find file: %s\n", argv[1]);
    return -1;
  }

  detect_pak(&stream);

  // To do: Gen1 and Gen2 don't seem to have
  // an actual count of objects that are in the
  // pack file.

  if (type == PakType::Gen2) {
    result = extract_gen2(&stream);
  }

  if (type == PakType::Gen1) {
    result = extract_gen1(&stream);
  }

  if (!result) {
    printf("Failed to extract file\n");
    return -1;
  }

  stream.close();

  return 0;
}

