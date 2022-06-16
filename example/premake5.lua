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
		"../include"
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
		links { "fjoiner-d.lib" }

	filter "configurations:Release"
		defines "RELEASE"
		optimize "On"
		staticruntime "on"
		runtime "Release"
		links { "fjoiner.lib" }