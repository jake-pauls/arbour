project "Vol"
	kind "WindowedApp"
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

	filter "system:Windows"
		-- Force include the PCH on MSVC
		buildoptions { "/FI volpch.h" }

		links { 
			"d3d12", 
			"dxgi", 
			"d3dcompiler" 
		}
	filter {}
