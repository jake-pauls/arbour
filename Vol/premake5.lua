project "Vol"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	location "%{wks.location}/Vol"

	vpaths {
		["Code/**"] = { "Source/**.cpp", "Source/**.h" }
	}