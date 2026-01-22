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