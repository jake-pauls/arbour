workspace "vol"
	architecture "x64"
	configurations { "Debug", "Release" }
	startproject "Vol"

    targetdir "%{wks.location}/%{prj.name}/Build/Bin/%{cfg.buildcfg}"
    objdir "%{wks.location}/%{prj.name}/Build/Intermediates/%{cfg.buildcfg}"

	files {
		"%{prj.name}/Source/**.cpp",
		"%{prj.name}/Source/**.h"
	}

	vpaths { 
		["./"] = { "**premake5.lua" } 
	}

	filter "system:Windows" 
		defines { "WIN32" }
	filter "configurations:Debug"
		defines { "VOL_DEBUG" }
		runtime "Debug"
		symbols "On"
	filter "configurations:Release"
		defines { "VOL_RELEASE" }
		runtime "Release"
		optimize "On"
		symbols "Off"
	filter {}

include "Vol"