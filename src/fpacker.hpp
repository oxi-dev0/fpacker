// FPACKER V2.0.0 [https://github.com/oxi-dev0/fpacker]
// Copyright (c) 2023 Oxi. All rights reserved.
#pragma once

#define FPACKER_HEADER 0x46,0x50,0x41,0x43,0x4B,0x45,0x52,0x5F,0x50,0x41,0x43,0x4B,0x41,0x47,0x45
#define FPACKER_HEADER_LENGTH 15
#define BUFFERSIZE_MIN 1024

#include <string>
#include <vector>
#include <filesystem>
#include <cassert>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace FPacker
{
	inline bool _ValidPackage(char* buf)
	{
		bool valid = true;

		// Check if the start of the buffer matches the magic header (could use strcmp)
		char headerBuf[FPACKER_HEADER_LENGTH] = { FPACKER_HEADER };
		for (int i = 0; i < FPACKER_HEADER_LENGTH; i++) {
			if (headerBuf[i] != buf[i]) {
				valid = false;
			}
		}

		return valid;
	}

	inline bool Pack(const std::string& fromDir, const std::string& toFile)
	{
		// Find files for packing
		std::vector<std::filesystem::path> foundFiles;
		for (std::filesystem::recursive_directory_iterator i(fromDir), end; i != end; ++i) {
			if (!is_directory(i->path())) {
				foundFiles.push_back(i->path());
			}
		}

		if (foundFiles.size() == 0) {
			return false;
		}

		// Create buffer to write data to before dumping to file
		char* buffer = (char*)malloc(BUFFERSIZE_MIN);
		if (buffer == nullptr) { return false; }

		size_t bufferSize = BUFFERSIZE_MIN;
		size_t initialBufferSize = BUFFERSIZE_MIN;
		size_t bufferCursor = 0;

		// Write magic number to file buffer
		char headerBuf[FPACKER_HEADER_LENGTH] = { FPACKER_HEADER };
		for (size_t i = 0; i < FPACKER_HEADER_LENGTH; i++) {
			memcpy(buffer + bufferCursor, &headerBuf[i], 1);
			bufferCursor += 1;
		}

		for (auto& path : foundFiles) {
			std::string pathStr = std::filesystem::relative(path, std::filesystem::path(fromDir)).string();
			size_t pathSize = pathStr.size();

			// If the buffer is too small to write this data, inflate it
			size_t oldBufferSize = bufferSize;
			while (bufferCursor + sizeof(size_t) + pathSize > bufferSize) {
				bufferSize += initialBufferSize;
			}
			if (bufferSize != oldBufferSize) {
				// Create a new, bigger, buffer and write old data to it
				char* oldBuffer = buffer;
				buffer = (char*)malloc(bufferSize);
				if (buffer == nullptr) { free(oldBuffer); return false; }
				memcpy(buffer, oldBuffer, oldBufferSize);
				free(oldBuffer);
			}

			// Write name size, then the name data to the buffer
			memcpy(buffer + bufferCursor, &pathSize, sizeof(size_t));
			bufferCursor += sizeof(size_t);
			memcpy(buffer + bufferCursor, pathStr.c_str(), pathSize);
			bufferCursor += pathSize;

			// Open the file and calculate it's size
			std::ifstream in(path.string().c_str(), std::ios::binary);
			std::streampos fsize = 0;
			fsize = in.tellg();
			in.seekg(0, std::ios::end);
			fsize = in.tellg() - fsize;
			in.seekg(0, std::ios::beg);
			size_t fsizeVal = (size_t)fsize;

			// If the buffer is too small to write this data, inflate it
			oldBufferSize = bufferSize;
			while (bufferCursor + sizeof(size_t) + fsizeVal > bufferSize) {
				bufferSize += initialBufferSize;
			}
			if (bufferSize != oldBufferSize) {
				// Create a new, bigger, buffer and write old data to it
				char* oldBuffer = buffer;
				buffer = (char*)malloc(bufferSize);
				if (buffer == nullptr) { free(oldBuffer); return false; }
				memcpy(buffer, oldBuffer, oldBufferSize);
				free(oldBuffer);
			}

			// Write data size, then the data to the buffer
			memcpy(buffer + bufferCursor, &fsizeVal, sizeof(size_t));
			bufferCursor += sizeof(size_t);
			in.read(buffer + bufferCursor, fsizeVal);
			bufferCursor += fsizeVal;
		}

		// Dump buffer to file
		std::ofstream fileStream(toFile, std::ios::out | std::ios::binary);
		fileStream.write(buffer, bufferCursor);
		fileStream.close();

		free(buffer);

		return true;
	}

	inline bool Unpack(const std::string& fromFile, const std::string& toDir)
	{
		// Open the package file and calculate its size
		std::ifstream in(fromFile.c_str(), std::ios::binary);
		std::streampos fsize = 0;
		fsize = in.tellg();
		in.seekg(0, std::ios::end);
		fsize = in.tellg() - fsize;
		in.seekg(0, std::ios::beg);
		size_t fsizeVal = (size_t)fsize;
		
		char* fileBuf = (char*)malloc(fsizeVal);
		if (fileBuf == nullptr) { return false; }

		// Dump file contents to buffer
		in.read(fileBuf, fsizeVal);
		in.close();
		
		// Validate it is an FPacker package
		if (fsizeVal < FPACKER_HEADER_LENGTH) { free(fileBuf); return false; }
		if (!_ValidPackage(fileBuf)) { free(fileBuf); return false; }

		// Start cursor after magic number
		size_t cursor = FPACKER_HEADER_LENGTH;

		// While the cursor is not at the end of the file
		while (cursor < fsizeVal) {
			// Write the name size to a variable
			size_t nameSize = 0;
			memcpy(&nameSize, fileBuf + cursor, sizeof(size_t));
			cursor += sizeof(size_t);

			// Create a buffer for the name data
			char* nameBuf = (char*)malloc(nameSize+1);
			if (nameBuf == nullptr) { free(fileBuf); return false; }

			// Write the name data to the name buffer
			memcpy(nameBuf, fileBuf + cursor, nameSize);
			nameBuf[nameSize] = '\0';
			cursor += nameSize;

			// Write the data size to a variable
			size_t dataSize = 0;
			memcpy(&dataSize, fileBuf + cursor, sizeof(size_t));
			cursor += sizeof(size_t);

			// Create a buffer for the data
			char* dataBuf = (char*)malloc(dataSize);
			if (dataBuf == nullptr) { free(nameBuf); free(fileBuf); return false; }

			// Write the data to the buffer
			memcpy(dataBuf, fileBuf + cursor, dataSize);
			cursor += dataSize;

			// Create directory for the file if needed
			std::string nameStr = std::string(nameBuf);
			std::filesystem::create_directories(std::filesystem::path(toDir + "/" + nameStr).remove_filename());

			// Create the file and dump the data to it
			std::ofstream newFileStream(toDir + "/" + nameStr, std::ios::out | std::ios::binary);
			newFileStream.write(dataBuf, dataSize);
			newFileStream.close();

			free(nameBuf);
			free(dataBuf);
		}
		
		free(fileBuf);

		return true;
	}
}
