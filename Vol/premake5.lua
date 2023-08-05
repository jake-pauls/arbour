project "Vol"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	location "%{wks.location}/Vol"

	pchheader "volpch.h"
	pchsource "volpch.cpp"

	vpaths {
		["PCH/*"] = { "**pch.h", "**pch.cpp" },
		["Code/**"] = { "Source/**.h", "Source/**.cpp" },
	}

	includedirs { 
		"%{prj.location}/Source",
		"%{prj.location}"
	}