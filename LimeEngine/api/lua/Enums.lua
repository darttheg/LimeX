---@class Lime
Lime = Lime or {}
---@alias Lime.PrintColor integer
---@class Lime.PrintColorEnum
---@field White Lime.PrintColor
---@field Red Lime.PrintColor
---@field Green Lime.PrintColor
---@field Blue Lime.PrintColor
Lime.PrintColor = {
White = 0,
Red = 1,
Green = 2,
Blue = 3,
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