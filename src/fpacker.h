// Copyright (c) 2022 Oxi. All rights reserved.
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
	bool _ValidPackage(char* buf);

	bool Pack(const std::string& fromDir, const std::string& toFile);
	bool Unpack(const std::string& fromFile, const std::string& toDir);
}