---@class Lime
--- Event called by Lime prior to initializing the window. This Event is run with a mutable table argument. Edit this table to change driver type, window size, and more.
---@field Init Event
--- Event called by Lime every rendering frame. This Event is run with a number delta time argument.
---@field Update Event
--- Event called by Lime once the application ends in any way. This Event is run with a boolean didFail argument, true if the closure is because of an error.
---@field End Event
Lime = Lime or {}

---@class Event
Event = Event or {}
---@overload fun(): Event
---@return Event
function Event.new() end

---@class Hook
Hook = Hook or {}
---@overload fun(): Hook
---@return Hook
function Hook.new() end

---@class Vec2
---@field x number
---@field y number
Vec2 = Vec2 or {}
---@overload fun(): Vec2
---@overload fun(x:number, y:number): Vec2
---@overload fun(all:number): Vec2
---@return Vec2
function Vec2.new() end

--- Clears all functions hooked to this Event.
function Event:Clear() end

--- Hook a function to this Event.
---@param Function function
---@return Hook
function Event:Hook(Function) end

---@overload fun(test:number): number
---@return number
function Event:Length() end

--- Run this Event.
---@param ... any
function Event:Run(...) end

---@return boolean
function Hook:IsHooked() end

--- Unhook a function to this Event.
function Hook:Unhook() end

--- Closes the Lime application.
function Lime.Close() end

---@return string
function Lime.GetVersion() end

--- Prints a message to console.
---@param msg string
---@param color Lime.PrintColor?
---@return void
function Lime.Log(msg, color) end

--- If set to true, Lime will close on any error. A pop-up will be disclosed prior with error details.
---@param doEnd boolean
function Lime.SetEndOnError(doEnd) end

--- IMPORTANT: This function should always be run prior to window creation (pre-Lime.Update Event) as only here can the driver type be changed. This function sets initial parameters for the Lime application.
---@param driver Lime.DriverType
---@param vSync boolean?
---@param frameRate number?
---@param windowSize Vec2?
---@param renderSize Vec2?
---@param scaleRenderToWindow boolean?
---@param fullscreen boolean?
---@return boolean
function Lime.SetInitConfig(driver, vSync, frameRate, windowSize, renderSize, scaleRenderToWindow, fullscreen) end
