workspace "FPacker"
	architecture "x64"
	flags {"MultiProcessorCompile"}

	configurations 
	{
		"Debug", 
		"Release"
	}

outputdir = "%{cfg.buildcfg}"

include "example"

project "FPacker"
	kind "StaticLib"
	language "C++"


	targetdir ("bin/" .. outputdir)
	objdir ("bin-int/" .. outputdir)
	debugdir "%{cfg.targetdir}"

	files 
	{
		"src/**.h",
		"src/**.cpp",
		"include/**.h"
	}

	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"

	filter "configurations:Debug"
		defines "DEBUG"
		symbols "On"
		staticruntime "on"
		runtime "Debug"
		targetname "fpacker-d"

	filter "configurations:Release"
		defines "RELEASE"
		optimize "On"
		staticruntime "on"
		runtime "Release"
		targetname "fpacker"
