---@class Lime
Lime = Lime or {}
---@class Lime.Enum
Lime.Enum = Lime.Enum or {}
---@alias Lime.Enum.PrintColor integer
---@class Lime.Enum.PrintColorEnum
---@field White Lime.Enum.PrintColor
---@field Red Lime.Enum.PrintColor
---@field Green Lime.Enum.PrintColor
---@field Blue Lime.Enum.PrintColor
---@field Yellow Lime.Enum.PrintColor
Lime.Enum.PrintColor = {
White = 0,
Red = 1,
Green = 2,
Blue = 3,
Yellow = 4,
}
---@alias Lime.Enum.PopUpIcon integer
---@class Lime.Enum.PopUpIconEnum
---@field None Lime.Enum.PopUpIcon
---@field Warning Lime.Enum.PopUpIcon
---@field Message Lime.Enum.PopUpIcon
---@field Information Lime.Enum.PopUpIcon
Lime.Enum.PopUpIcon = {
None = 0,
Warning = 1,
Message = 2,
Information = 3,
}
---@alias Lime.Enum.DriverType integer
---@class Lime.Enum.DriverTypeEnum
---@field None Lime.Enum.DriverType
---@field Software Lime.Enum.DriverType
---@field SoftwarePlus Lime.Enum.DriverType
---@field Direct3D8 Lime.Enum.DriverType
---@field Direct3D9 Lime.Enum.DriverType
---@field OpenGL Lime.Enum.DriverType
Lime.Enum.DriverType = {
None = 0,
Software = 1,
SoftwarePlus = 2,
Direct3D8 = 3,
Direct3D9 = 4,
OpenGL = 5,
}
---@alias Lime.Enum.MaterialType integer
---@class Lime.Enum.MaterialTypeEnum
---@field Solid Lime.Enum.MaterialType
---@field SolidMultiplyLayers Lime.Enum.MaterialType
---@field Add Lime.Enum.MaterialType
---@field AlphaSoft Lime.Enum.MaterialType
---@field AlphaHarsh Lime.Enum.MaterialType
---@field SphereMap Lime.Enum.MaterialType
---@field ReflectMap Lime.Enum.MaterialType
Lime.Enum.MaterialType = {
Solid = 0,
SolidMultiplyLayers = 1,
Add = 12,
AlphaSoft = 13,
AlphaHarsh = 14,
SphereMap = 10,
ReflectMap = 11,
}
---@alias Lime.Enum.ImageWrapType integer
---@class Lime.Enum.ImageWrapTypeEnum
---@field Repeat Lime.Enum.ImageWrapType
---@field Clamp Lime.Enum.ImageWrapType
---@field ClampToEdge Lime.Enum.ImageWrapType
---@field Mirror Lime.Enum.ImageWrapType
---@field MirrorClamp Lime.Enum.ImageWrapType
---@field MirrorClampToEdge Lime.Enum.ImageWrapType
Lime.Enum.ImageWrapType = {
Repeat = 0,
Clamp = 1,
ClampToEdge = 2,
Mirror = 4,
MirrorClamp = 5,
MirrorClampToEdge = 6,
}
---@alias Lime.Enum.MaterialQuality integer
---@class Lime.Enum.MaterialQualityEnum
---@field Low Lime.Enum.MaterialQuality
---@field Medium Lime.Enum.MaterialQuality
---@field High Lime.Enum.MaterialQuality
Lime.Enum.MaterialQuality = {
Low = 0,
Medium = 1,
High = 2,
}
---@alias Lime.Enum.ImageCreationQuality integer
---@class Lime.Enum.ImageCreationQualityEnum
---@field Low Lime.Enum.ImageCreationQuality
---@field Medium Lime.Enum.ImageCreationQuality
---@field High Lime.Enum.ImageCreationQuality
Lime.Enum.ImageCreationQuality = {
Low = 0,
Medium = 1,
High = 2,
}
---@alias Lime.Enum.LightManagementType integer
---@class Lime.Enum.LightManagementTypeEnum
---@field EightNearest Lime.Enum.LightManagementType
---@field ThreeClosestToAll Lime.Enum.LightManagementType
---@field EmptyZones Lime.Enum.LightManagementType
Lime.Enum.LightManagementType = {
EightNearest = 0,
ThreeClosestToAll = 1,
EmptyZones = 2,
}
---@alias Lime.Enum.ZOrderMethod integer
---@class Lime.Enum.ZOrderMethodEnum
---@field BehindAll Lime.Enum.ZOrderMethod
---@field Default Lime.Enum.ZOrderMethod
---@field Decal Lime.Enum.ZOrderMethod
---@field ExceptDecal Lime.Enum.ZOrderMethod
---@field XRay Lime.Enum.ZOrderMethod
---@field StrictXRay Lime.Enum.ZOrderMethod
---@field OnTop Lime.Enum.ZOrderMethod
Lime.Enum.ZOrderMethod = {
BehindAll = 0,
Default = 1,
Decal = 2,
ExceptDecal = 3,
XRay = 4,
StrictXRay = 5,
OnTop = 6,
}