// Copyright (c) 2022 Oxi. All rights reserved.
#include "fpacker.h"

namespace FPacker
{
	bool Pack(const std::string& fromDir, const std::string& toFile) {
		std::vector<std::filesystem::path> foundFiles;
		for (std::filesystem::recursive_directory_iterator i(fromDir), end; i != end; ++i) {
			if (!is_directory(i->path())) {
				foundFiles.push_back(i->path());
			}
		}

		if (foundFiles.size() == 0) {
			return false;
		}

		char* buffer = (char*)malloc(BUFFERSIZE_MIN);
		if (buffer == nullptr) { return false; }

		size_t bufferSize = BUFFERSIZE_MIN;
		size_t initialBufferSize = BUFFERSIZE_MIN;
		size_t bufferCursor = 0;

		char headerBuf[FPACKER_HEADER_LENGTH] = { FPACKER_HEADER };
		for (size_t i = 0; i < FPACKER_HEADER_LENGTH; i++) {
			memcpy(buffer + bufferCursor, &headerBuf[i], 1);
			bufferCursor += 1;
		}

		for (auto& path : foundFiles) {
			std::string pathStr = std::filesystem::relative(path, std::filesystem::path(fromDir)).string();
			size_t pathSize = pathStr.size();

			size_t oldBufferSize = bufferSize;
			while (bufferCursor + sizeof(size_t) + pathSize > bufferSize) {
				bufferSize += initialBufferSize;
			}
			if (bufferSize != oldBufferSize) {
				char* oldBuffer = buffer;
				buffer = (char*)malloc(bufferSize);
				if (buffer == nullptr) { return false; }
				memcpy(buffer, oldBuffer, oldBufferSize);
				free(oldBuffer);
			}
			memcpy(buffer + bufferCursor, &pathSize, sizeof(size_t));
			bufferCursor += sizeof(size_t);
			memcpy(buffer + bufferCursor, pathStr.c_str(), pathSize);
			bufferCursor += pathSize;

			std::ifstream in(path.string().c_str(), std::ios::binary);
			std::streampos fsize = 0;
			fsize = in.tellg();
			in.seekg(0, std::ios::end);
			fsize = in.tellg() - fsize;
			in.seekg(0, std::ios::beg);
			size_t fsizeVal = (size_t)fsize;

			oldBufferSize = bufferSize;
			while (bufferCursor + sizeof(size_t) + fsizeVal > bufferSize) {
				bufferSize += initialBufferSize;
			}
			if (bufferSize != oldBufferSize) {
				char* oldBuffer = buffer;
				buffer = (char*)malloc(bufferSize);
				if (buffer == nullptr) { return false; }
				memcpy(buffer, oldBuffer, oldBufferSize);
				free(oldBuffer);
			}
			memcpy(buffer + bufferCursor, &fsizeVal, sizeof(size_t));
			bufferCursor += sizeof(size_t);
			in.read(buffer + bufferCursor, fsizeVal);
			bufferCursor += fsizeVal;
		}

		std::ofstream fileStream(toFile, std::ios::out | std::ios::binary);
		fileStream.write(buffer, bufferCursor);
		fileStream.close();

		free(buffer);

		return true;
	}

	bool _ValidPackage(char* buf) {
		bool valid = true;

		char headerBuf[FPACKER_HEADER_LENGTH] = { FPACKER_HEADER };
		for (int i = 0; i < FPACKER_HEADER_LENGTH; i++) {
			if (headerBuf[i] != buf[i]) {
				valid = false;
			}
		}

		return valid;
	}

	bool Unpack(const std::string& fromFile, const std::string& toDir) {
		std::ifstream in(fromFile.c_str(), std::ios::binary);
		std::streampos fsize = 0;
		fsize = in.tellg();
		in.seekg(0, std::ios::end);
		fsize = in.tellg() - fsize;
		in.seekg(0, std::ios::beg);
		size_t fsizeVal = (size_t)fsize;
		
		char* fileBuf = (char*)malloc(fsizeVal);
		if (fileBuf == nullptr) { return false; }

		in.read(fileBuf, fsizeVal);
		in.close();

		assert(fsizeVal > FPACKER_HEADER_LENGTH);
		assert(_ValidPackage(fileBuf));

		size_t cursor = FPACKER_HEADER_LENGTH;
		while (cursor < fsizeVal) {
			size_t nameSize = 0;
			memcpy(&nameSize, fileBuf + cursor, sizeof(size_t));
			cursor += sizeof(size_t);

			char* nameBuf = (char*)malloc(nameSize+1);
			if (nameBuf == nullptr) { return false; }

			memcpy(nameBuf, fileBuf + cursor, nameSize);
			nameBuf[nameSize] = '\0';
			cursor += nameSize;

			size_t dataSize = 0;
			memcpy(&dataSize, fileBuf + cursor, sizeof(size_t));
			cursor += sizeof(size_t);

			char* dataBuf = (char*)malloc(dataSize);
			if (dataBuf == nullptr) { return false; }

			memcpy(dataBuf, fileBuf + cursor, dataSize);
			cursor += dataSize;

			std::string nameStr = std::string(nameBuf);
			std::filesystem::create_directories(std::filesystem::path(toDir + "/" + nameStr).remove_filename());
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
