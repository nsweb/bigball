
solution "bigball_auto"
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	location "src"
 
	project "bigball_auto"
		location "src"
		kind "StaticLib"
		language "C++"
		files { "**.h", "**.cpp" }
		
		defines { "_CRT_SECURE_NO_WARNINGS", "TINYXML2_IMPORT", "_WINDOWS", "_USRDLL", "_BIGBALL_EXPORTS_" }
		flags { "NoPCH", "NoNativeWChar", "NoEditAndContinue" }
		targetdir "bin/$(Platform)_$(Configuration)/"
	 
		configuration "Debug"
			defines { "_DEBUG" }
			flags { "Symbols" }
	 
		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize" } 
			
		--links { "png", "zlib" }
		--libdirs { "libs", "../mylibs" } 
		libdirs { "../../3rdparty/SDL2-2.0.1/lib/x86/$(Configuration)", "../../3rdparty/glew-1.10.0/lib/release\Win32", "../../3rdparty/tinyxml2/tinyxml2/bin/$(Platform)-$(Configuration)-Dll" } 
		includedirs { "../../3rdparty/SDL2-2.0.1/include", "../../3rdparty/glew-1.10.0/include", "../../3rdparty/tinyxml2" }
		
--Enable debugging information	flags
--Optimize for size or speed	flags
--Turn compiler or linker features on or off	flags, buildoptions, linkoptions
--Set the name or location of the compiled target file	targetname, targetextension,
--											targetprefix, targetdir


