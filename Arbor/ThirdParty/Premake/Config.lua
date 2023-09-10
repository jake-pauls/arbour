Config = {
    Package = {
        Include = "",
        Link = "",
    },
}

-- ThirdParty Include Directory Reference

Config.Package["DirectX_Headers"] = {
    Include = "%{wks.location}/Arbor/ThirdParty/DirectX-Headers/include/directx",
    Link = "DirectX-Headers"
}
