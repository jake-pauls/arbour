workspace "arbor"
	architecture "x64"
	configurations { "Debug", "Release" }
	startproject "Arbor"

    targetdir "%{wks.location}/%{prj.name}/Build/Bin/%{cfg.buildcfg}"
    objdir "%{wks.location}/%{prj.name}/Build/Intermediates/%{cfg.buildcfg}"

	vpaths { 
		["./"] = { "**premake5.lua" } 
	}

	filter "system:Windows" 
		defines { "WIN32" }
	filter "configurations:Debug"
		defines { "ARBOR_DEBUG" }
		runtime "Debug"
		symbols "On"
	filter "configurations:Release"
		defines { "ARBOR_RELEASE" }
		runtime "Release"
		optimize "On"
		symbols "Off"
	filter {}

include "Arbor"