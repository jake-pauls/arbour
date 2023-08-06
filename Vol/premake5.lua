project "Vol"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++20"
	location "%{wks.location}/Vol"

	pchheader "volpch.h"
	pchsource "volpch.cpp"

	buildoptions { "/FI volpch.h" }

	vpaths {
		["PCH/*"] = { "**pch.h", "**pch.cpp" },
		["Code/**"] = { "Source/**.h", "Source/**.cpp" },
	}

	includedirs { 
		"%{prj.location}/Source",
		"%{prj.location}"
	}