project "Arbour"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++20"
	location "%{wks.location}/Arbour"

	pchheader "arbourpch.h"
	pchsource "arbourpch.cpp"

	-- Arbour ThirdParty Configuration
	include "ThirdParty/Premake/Config.lua"

	files {
		"%{prj.location}/*pch.**",
		"%{prj.location}/Source/Main.cpp",
		"%{prj.location}/Source/Core/**.h",
		"%{prj.location}/Source/Core/**.cpp",
	}

	vpaths {
		["PCH/*"] = { "**pch.h", "**pch.cpp" },
		["Code/**"] = { "Source/**.h", "Source/**.cpp" },
	}

	includedirs { 
		"%{prj.location}/Source",
		"%{prj.location}",
		-- ThirdParty Configuration 
		"%{Config.Package.DirectX_Headers.Include}"
	}

	filter "system:Windows"
		defines {
			"_CRT_SECURE_NO_WARNINGS"
		}

		-- Force include the PCH on MSVC
		buildoptions { "/FI arbourpch.h" }

		-- Windows only source files
		files {
			"%{prj.location}/Source/Windows/**.h",
			"%{prj.location}/Source/Windows/**.cpp",
		}

		links { 
			"d3d12", 
			"dxgi", 
			"dxguid", -- DXGI_DEBUG flags
			"d3dcompiler",
			"%{Config.Package.DirectX_Headers.Link}"
		}

		group "ThirdParty"
			include "ThirdParty/Premake/DirectX-Headers.premake5.lua"
		group ""
	filter {}