

function SetTarget( _configuration, _platform )
	local platformname = _platform
	local archname = _platform
	if _platform == "x32" then
		platformname = "win32"
		archname = "x86"
	end
	local strtarget = string.format( "./bin/%s_%s/", _configuration, platformname ) 
	local strobj = string.format( "./intermediate/%s_%s", _configuration, platformname ) 
	configuration {_configuration, _platform}
		targetdir( strtarget )
		objdir( strobj )
end

function SetLibs( _configuration, _platform )
	local platformname = _platform
	local archname = _platform
	if _platform == "x32" then
		platformname = "win32"
		archname = "x86"
	end
	local strSDL = string.format( "../3rdparty/SDL2-2.0.1/lib/%s/%s", archname, _configuration ) 
	local strGlew = string.format( "../3rdparty/glew-1.10.0/lib/%s/%s", _configuration, platformname )
	local strTinyxml = string.format( "../3rdparty/tinyxml2/tinyxml2/bin/%s-%s-Dll", platformname, _configuration )

	configuration {_configuration, _platform}
		libdirs { strSDL, strGlew, strTinyxml }

end

solution "bigball_auto"
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	location "src"
 
	project "bigball_auto"
		location "src"
		kind "StaticLib"
		language "C++"
		files { "**.h", "**.cpp" }
		targetname "bigball"
		
		defines { "_CRT_SECURE_NO_WARNINGS", "TINYXML2_IMPORT", "_WINDOWS", "_USRDLL", "_BIGBALL_EXPORTS_" }
		flags { "NoPCH", "NoNativeWChar", "NoEditAndContinue" }

		--libdirs { "../3rdparty/SDL2-2.0.1/lib/x86/$(Configuration)", "../3rdparty/glew-1.10.0/lib/release\Win32", "../3rdparty/tinyxml2/tinyxml2/bin/$(Platform)-$(Configuration)-Dll" } 
		includedirs { "../3rdparty/SDL2-2.0.1/include", "../3rdparty/glew-1.10.0/include", "../3rdparty/tinyxml2" }
		--links { "user32", "gdi32" }

		SetTarget( "Debug", "x32" )
		SetTarget( "Debug", "x64" )
		SetTarget( "Release", "x32" )
		SetTarget( "Release", "x64" )
		SetLibs( "Debug", "x32" )
		SetLibs( "Debug", "x64" )
		SetLibs( "Release", "x32" )
		SetLibs( "Release", "x64" )

		configuration "Debug"
			defines { "_DEBUG" }
			flags { "Symbols" }
	 
		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize" } 
			
		
		
--Enable debugging information	flags
--Optimize for size or speed	flags
--Turn compiler or linker features on or off	flags, buildoptions, linkoptions
--Set the name or location of the compiled target file	targetname, targetextension,
--											targetprefix, targetdir


