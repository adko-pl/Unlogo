--by Jeff Crouse (March 30, 2011)
--Run 'premake4 gmake' to generate buildfiles


solution "Moustachizer" 
	configurations { "Debug", "Release" }

project "moustachizer"
	kind "SharedLib"
	language "C++"
	files {  
		"src/*.h",
		"src/*.c",
		"src/*.cpp",
		"src/FaceTracker/*.h", 
		"src/FaceTracker/*.cc",
	}
	links { 
		"opencv_core", 
		"opencv_imgproc", 
		"opencv_objdetect",
	}
	libdirs { 
		"/usr/local/lib", 
		os.findlib("opencv_core"),
	}
	includedirs { 
		"src",
		"src/FaceTracker",
		"/usr/local/include",
		"/usr/include"
	}
	
configuration "Debug"
	defines { "DEBUG" }
	flags { "Symbols" }
	
configuration "Release"
	defines { "NDEBUG" }
	flags { "Optimize" }  