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

		assert(foundFiles.size() > 0, "FPacker: The directory has files");

		std::ofstream fileStream(targetFile, std::ios::out | std::ios::binary);
		char headerBuf[FPACKER_HEADER_LENGTH] = { FPACKER_HEADER };
		fileStream.write(headerBuf, FPACKER_HEADER_LENGTH);

		for (auto& path : foundFiles) {
			char nameBuf[FPACKER_NAMEMARKER_LENGTH] = { FPACKER_NAMEMARKER };
			fileStream.write(nameBuf, FPACKER_NAMEMARKER_LENGTH);

			std::string pathStr = std::filesystem::relative(path, std::filesystem::path(fromDir)).string();
			fileStream.write(pathStr.c_str(), strlen(pathStr.c_str()));

			std::vector<char> buf;
			std::ifstream in(path.string().c_str(), std::ios::binary);
			char c;
			while (in.get(c))
			{
				buf.emplace_back(c);
			}

			char sizeBuf[FPACKER_SIZEMARKER_LENGTH] = { FPACKER_SIZEMARKER };
			fileStream.write(sizeBuf, FPACKER_SIZEMARKER_LENGTH);

			std::string s = IntToPaddedBinary((int)buf.size());
			fileStream.write(s.c_str(), 32);

			char fileBuf[FPACKER_FILEMARKER_LENGTH] = { FPACKER_FILEMARKER };
			fileStream.write(fileBuf, FPACKER_FILEMARKER_LENGTH);

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
		std::vector<char> sizeBuf;
		uint32_t size = 0;
		std::vector<char> dataBuf;
		int section = 2; // 0: name, 1: size, 2: data

		while (cursor < fileBuf.size()) {
			switch (section) {
			case 2: // data section
			{
				for (unsigned int cur = cursor; cur < cursor + size; cur++) {
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
				cursor += size + FPACKER_NAMEMARKER_LENGTH;

				section = 0;
				continue;
			} break;
			case 1: // size section
			{
				if (LookaheadMatch(fileBuf, cursor, { FPACKER_FILEMARKER })) {
					size = PaddedBinaryToInt(VectorToString(sizeBuf).str());

					section = 2;
					cursor += FPACKER_FILEMARKER_LENGTH;
					continue;
				}
				else {
					sizeBuf.emplace_back(fileBuf[cursor]);
					cursor += 1;
					continue;
				}

			} break;
			case 0: // name section
			{
				if (LookaheadMatch(fileBuf, cursor, { FPACKER_SIZEMARKER })) {
					sizeBuf.clear();
					dataBuf.clear();

					section = 1;
					cursor += FPACKER_SIZEMARKER_LENGTH;
					continue;
				}
				else {
					nameBuf.emplace_back(fileBuf[cursor]);
					cursor += 1;
					continue;
				}
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