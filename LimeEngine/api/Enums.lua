---@class Lime
Lime = Lime or {}
---@alias Lime.PrintColor integer
---@class Lime.PrintColorEnum
---@field White Lime.PrintColor
---@field Red Lime.PrintColor
---@field Green Lime.PrintColor
---@field Blue Lime.PrintColor
---@field Yellow Lime.PrintColor
Lime.PrintColor = {
White = 0,
Red = 1,
Green = 2,
Blue = 3,
Yellow = 4,
}
---@alias Lime.PopUpIcon integer
---@class Lime.PopUpIconEnum
---@field None Lime.PopUpIcon
---@field Warning Lime.PopUpIcon
---@field Message Lime.PopUpIcon
---@field Information Lime.PopUpIcon
Lime.PopUpIcon = {
None = 0,
Warning = 1,
Message = 2,
Information = 3,
}
---@alias Lime.DriverType integer
---@class Lime.DriverTypeEnum
---@field None Lime.DriverType
---@field Software Lime.DriverType
---@field SoftwarePlus Lime.DriverType
---@field Direct3D8 Lime.DriverType
---@field Direct3D9 Lime.DriverType
---@field OpenGL Lime.DriverType
Lime.DriverType = {
None = 0,
Software = 1,
SoftwarePlus = 2,
Direct3D8 = 3,
Direct3D9 = 4,
OpenGL = 5,
}
---@alias Lime.MaterialType integer
---@class Lime.MaterialTypeEnum
---@field Solid Lime.MaterialType
---@field SolidMultiplyLayers Lime.MaterialType
---@field Add Lime.MaterialType
---@field AlphaSoft Lime.MaterialType
---@field AlphaHarsh Lime.MaterialType
---@field SphereMap Lime.MaterialType
---@field ReflectMap Lime.MaterialType
Lime.MaterialType = {
Solid = 0,
SolidMultiplyLayers = 1,
Add = 12,
AlphaSoft = 13,
AlphaHarsh = 14,
SphereMap = 10,
ReflectMap = 11,
}
---@alias Lime.ImageWrapType integer
---@class Lime.ImageWrapTypeEnum
---@field Repeat Lime.ImageWrapType
---@field Clamp Lime.ImageWrapType
---@field ClampToEdge Lime.ImageWrapType
---@field Mirror Lime.ImageWrapType
---@field MirrorClamp Lime.ImageWrapType
---@field MirrorClampToEdge Lime.ImageWrapType
Lime.ImageWrapType = {
Repeat = 0,
Clamp = 1,
ClampToEdge = 2,
Mirror = 4,
MirrorClamp = 5,
MirrorClampToEdge = 6,
}
---@alias Lime.MaterialQuality integer
---@class Lime.MaterialQualityEnum
---@field Low Lime.MaterialQuality
---@field Medium Lime.MaterialQuality
---@field High Lime.MaterialQuality
---@field Ultra Lime.MaterialQuality
Lime.MaterialQuality = {
Low = 0,
Medium = 1,
High = 2,
Ultra = 3,
}
---@alias Lime.ZOrderMethod integer
---@class Lime.ZOrderMethodEnum
---@field BehindAll Lime.ZOrderMethod
---@field Default Lime.ZOrderMethod
---@field Decal Lime.ZOrderMethod
---@field ExceptDecal Lime.ZOrderMethod
---@field XRay Lime.ZOrderMethod
---@field StrictXRay Lime.ZOrderMethod
---@field OnTop Lime.ZOrderMethod
Lime.ZOrderMethod = {
BehindAll = 0,
Default = 1,
Decal = 2,
ExceptDecal = 3,
XRay = 4,
StrictXRay = 5,
OnTop = 6,
}