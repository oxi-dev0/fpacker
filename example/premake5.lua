outputdir = "%{cfg.buildcfg}"

project "Example"
	kind "ConsoleApp"
	language "C++"
	
	dependson "FPacker"

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

	libdirs { "../bin/" .. outputdir}

	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"

	filter "configurations:Debug"
		defines "DEBUG"
		symbols "On"
		staticruntime "on"
		runtime "Debug"
		links { "fpacker-d.lib" }

	filter "configurations:Release"
		defines "RELEASE"
		optimize "On"
		staticruntime "on"
		runtime "Release"
		links { "fpacker.lib" }