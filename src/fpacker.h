// Copyright (c) 2022 Oxi. All rights reserved.
#pragma once

#define FPACKER_HEADER 0x46,0x4A,0x4F,0x49,0x4E,0x45,0x52,0x5F,0x50,0x41,0x43,0x4B,0x41,0x47,0x45
#define FPACKER_HEADER_LENGTH 15

#include <string>
#include <vector>
#include <filesystem>
#include <cassert>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace FPacker
{
	class Package {
	protected:
		std::string fromDir;
		std::string fromFile;

	private:
		static bool ValidPackage(std::vector<char> buf);
		static bool LookaheadMatch(std::vector<char> buf, int cursor, std::vector<char> seq);

	public:
		static Package LoadDir(const std::string& dir);
		static Package LoadPackageFile(const std::string& file);
		
		bool Pack(std::string targetFile);
		bool Unpack(std::string targetDir);
	};
}