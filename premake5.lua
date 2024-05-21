workspace "TerrainDemo"
	configurations { "Debug", "Release" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}"

project "TerrainDemo"
	location "TerrainDemo"
	kind "ConsoleApp"
	language "C++"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/imgui/*.cpp",
		"%{prj.name}/vendor/imgui/*.h",
		"%{prj.name}/vendor/imgui/backends/imgui_impl_glfw.cpp",
		"%{prj.name}/vendor/imgui/backends/imgui_impl_glfw.h",
		"%{prj.name}/vendor/imgui/backends/imgui_impl_opengl3.cpp",
		"%{prj.name}/vendor/imgui/backends/imgui_impl_opengl3.h",
		"%{prj.name}/vendor/imGuIZMO.quat/imGuIZMO.quat/**.cpp",
		"%{prj.name}/vendor/imGuIZMO.quat/imGuIZMO.quat/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/glm/glm/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.h",
	}
	removefiles {
		"%{prj.name}/vendor/glm/glm/glm.cppm"
	}

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/dependencies/assimp/include",
		"%{prj.name}/dependencies/GLEW/include",
		"%{prj.name}/dependencies/GLFW/include",
		"%{prj.name}/vendor/glm",
		"%{prj.name}/vendor/imgui",
		"%{prj.name}/vendor/imgui/backends",
		"%{prj.name}/vendor/imGuIZMO.quat",
		"%{prj.name}/vendor/stb_image",
		"%{prj.name}/vendor",
		"%{prj.name}/src"
	}

	defines {
		"WIN32",
		"GLEW_STATIC"
	}

	filter "system:windows"
		libdirs { 
			"%{prj.name}/Dependencies/zlib/lib",
			"%{prj.name}/Dependencies/GLEW/lib/Release/Win32",
			"%{prj.name}/Dependencies/GLFW/lib-vc2022",
			"%{prj.name}/Dependencies/assimp/lib"
		}
		links { "opengl32", "glfw3", "glew32s", "assimp-vc143-mt", "zlibstaticd" }


	postbuildcommands {
		"{COPYDIR} %[%{prj.location}/res] %[%{cfg.buildtarget.directory}/res]",
	}
