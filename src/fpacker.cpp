// Copyright (c) 2022 Oxi. All rights reserved.
#include "fpacker.h"

namespace FPacker
{
	std::stringstream VectorToString(std::vector<char> charVec) {
		std::stringstream s;
		for (char c : charVec) {
			s << c;
		}
		return s;
	}

	std::string IntToPaddedBinary(int i) {
		char buf[50];
		sprintf_s(buf, "%032x", i);
		return std::string(buf);
	}

	int PaddedBinaryToInt(std::string s) {
		int i = 0;
		sscanf_s(s.c_str(), "%032x", &i);
		return i;
	}

	Package Package::LoadDir(std::string dir) {
		Package newPackage;
		newPackage.fromDir = dir;
		return newPackage;
	}

	Package Package::LoadPackageFile(std::string file) {
		Package newPackage;
		newPackage.fromFile = file;
		return newPackage;
	}

	bool Package::Pack(std::string targetFile) {
		assert(fromDir != "", "FPacker: From directory is set");

		std::vector<std::filesystem::path> foundFiles;
		for (std::filesystem::recursive_directory_iterator i(fromDir), end; i != end; ++i) {
			if (!is_directory(i->path())) {
				foundFiles.push_back(i->path());
			}
		}

		if (foundFiles.size() == 0) {
			return false;
		}

		std::ofstream fileStream(targetFile, std::ios::out | std::ios::binary);
		char headerBuf[FPACKER_HEADER_LENGTH] = { FPACKER_HEADER };
		fileStream.write(headerBuf, FPACKER_HEADER_LENGTH);

		for (auto& path : foundFiles) {
			std::string pathStr = std::filesystem::relative(path, std::filesystem::path(fromDir)).string();

			std::string pS = IntToPaddedBinary((int)pathStr.size());
			fileStream.write(pS.c_str(), 32);

			fileStream.write(pathStr.c_str(), strlen(pathStr.c_str()));

			std::vector<char> buf;
			std::ifstream in(path.string().c_str(), std::ios::binary);
			char c;
			while (in.get(c))
			{
				buf.emplace_back(c);
			}

			std::string s = IntToPaddedBinary((int)buf.size());
			fileStream.write(s.c_str(), 32);

			for (char d : buf) {
				char cbuf[1] = { d };
				fileStream.write(cbuf, 1);
			}
		}

		fileStream.close();

		return true;
	}

	bool Package::ValidPackage(std::vector<char> buf) {
		bool valid = true;

		char headerBuf[FPACKER_HEADER_LENGTH] = { FPACKER_HEADER };
		for (int i = 0; i < FPACKER_HEADER_LENGTH; i++) {
			if (headerBuf[i] != buf[i]) {
				valid = false;
			}
		}

		return valid;
	}

	bool Package::LookaheadMatch(std::vector<char> buf, int cursor, std::vector<char> seq) {
		if (seq.size() + cursor >= buf.size()) { return false; }
		if (buf[cursor] != seq[0]) { return false; }
		for (int i = cursor; i < seq.size()+cursor; i++) {
			if (buf[i] != seq[i - cursor]) { return false; }
		}

		return true;
	}

	bool Package::Unpack(std::string targetDir) {
		assert(fromFile != "", "FPacker: From file is set");

		std::vector<char> fileBuf;

		std::ifstream in(fromFile.c_str(), std::ios::binary);
		char c;
		while (in.get(c))
		{
			fileBuf.push_back(c);
		}

		assert(fileBuf.size() > FPACKER_HEADER_LENGTH, "FPacker: File is valid length");
		assert(ValidPackage(fileBuf), "FPacker: File is valid package file");

		unsigned int cursor = FPACKER_HEADER_LENGTH;
		std::vector<char> nameBuf;
		std::vector<char> nameSizeBuf;
		uint32_t nameSize = 0;
		std::vector<char> dataBuf;
		std::vector<char> dataSizeBuf;
		uint32_t dataSize = 0;
		int section = 0; // 0: nameSize, 1: name, 2: dataSize, 3: data

		while (cursor < fileBuf.size()) {
			switch (section) {
			case 3: // data section
			{
				for (unsigned int cur = cursor; cur < cursor + dataSize; cur++) {
					dataBuf.emplace_back(fileBuf[cur]);
				}
				if (nameBuf.size() > 0 && dataBuf.size() > 0) {
					std::filesystem::create_directories(std::filesystem::path(targetDir + "/" + VectorToString(nameBuf).str()).remove_filename());
					std::ofstream fileStream(targetDir + "/" + VectorToString(nameBuf).str(), std::ios::out | std::ios::binary);
					for (auto c : dataBuf) {
						char cBuf[1] = { c };
						fileStream.write(cBuf, 1);
					}
					fileStream.close();
				}

				nameBuf.clear();
				nameSizeBuf.clear();
				cursor += dataSize;

				section = 0;
				continue;
			} break;
			case 2: // data size section
			{
				for (unsigned int cur = cursor; cur < cursor + 32; cur++) {
					dataSizeBuf.emplace_back(fileBuf[cur]);
				}

				dataSize = PaddedBinaryToInt(VectorToString(dataSizeBuf).str());

				section = 3;
				cursor += 32;
				continue;

			} break;
			case 1: // name section
			{
				for (unsigned int cur = cursor; cur < cursor + nameSize; cur++) {
					nameBuf.emplace_back(fileBuf[cur]);
				}
				
				dataSizeBuf.clear();
				dataBuf.clear();

				section = 2;
				cursor += nameSize;
				continue;
			} break;
			case 0: // name size section
			{
				for (unsigned int cur = cursor; cur < cursor + 32; cur++) {
					nameSizeBuf.emplace_back(fileBuf[cur]);
				}

				nameSize = PaddedBinaryToInt(VectorToString(nameSizeBuf).str());

				section = 1;
				cursor += 32;
				continue;
			} break;
			}
		}

		if (nameBuf.size() > 0 && dataBuf.size() > 0) {
			std::filesystem::create_directories(std::filesystem::path(targetDir + "/" + VectorToString(nameBuf).str()).remove_filename());
			std::ofstream fileStream(targetDir + "/" + VectorToString(nameBuf).str(), std::ios::out | std::ios::binary);
			for (auto c : dataBuf) {
				char cBuf[1] = { c };
				fileStream.write(cBuf, 1);
			}
			fileStream.close();
		}

		return true;
	}
}