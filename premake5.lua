workspace "arbour"
	architecture "x64"
	configurations { "Debug", "Release" }
	startproject "Arbour"

    targetdir "%{wks.location}/%{prj.name}/Build/Binaries/%{cfg.buildcfg}"
    objdir "%{wks.location}/%{prj.name}/Build/Intermediates/%{cfg.buildcfg}"

	vpaths { 
		["./"] = { "**premake5.lua" } 
	}

	filter "system:Windows" 
		defines { "WIN32" }
	filter "configurations:Debug"
		defines { "ARBOUR_DEBUG" }
		runtime "Debug"
		symbols "On"
	filter "configurations:Release"
		defines { "ARBOUR_RELEASE" }
		runtime "Release"
		optimize "On"
		symbols "Off"
	filter {}

include "Arbour"