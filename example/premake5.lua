outputdir = "%{cfg.buildcfg}"

workspace "FPacker-Example"
	architecture "x64"
	flags {"MultiProcessorCompile"}

	configurations 
	{
		"Debug", 
		"Release"
	}

outputdir = "%{cfg.buildcfg}"

project "Example"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir)
	objdir ("bin-int/" .. outputdir)
	debugdir "%{cfg.targetdir}"

	files 
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"../src"
	}
		
	postbuildcommands {
		"{COPY} examplefolder bin/"..outputdir.."/examplefolder"
	}

	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"

	filter "configurations:Debug"
		defines "DEBUG"
		symbols "On"
		staticruntime "on"
		runtime "Debug"

	filter "configurations:Release"
		defines "RELEASE"
		optimize "On"
		staticruntime "on"
		runtime "Release"