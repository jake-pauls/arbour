project "DirectX-Headers"
    kind "StaticLib"
    language "C++"
    staticruntime "On"
    architecture "x86_64"

    targetdir "%{wks.location}/Arbour/ThirdParty/Build/Binaries/%{prj.name}"
    objdir "%{wks.location}/Arbour/ThirdParty/Build/Intermediates/%{prj.name}"

    files {
        "../%{prj.name}/src/**.cpp"
    }

    includedirs {
        "../%{prj.name}/include",
        "../%{prj.name}/include/directx",
        "../%{prj.name}/include/dxguids",
        "../%{prj.name}/include/wsl"
    }

    filter "system:Windows" 
        defines {
            "_DX_HEADERS_WIN32"
        }
    filter {}