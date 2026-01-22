---@class Lime
--- Event called by Lime once the application ends in any way. This Event is run with a boolean didFail argument, true if the closure is because of an error.
---@field End Event
--- Event called by Lime prior to initializing the window. This Event is run with a mutable table argument. Edit this table to change driver type, window size, and more.
---@field Init Event
--- Event called by Lime every rendering frame. This Event is run with a number delta time argument.
---@field Update Event
Lime = Lime or {}

--- Prints a message to console.
---@param msg string
---@param color Lime.PrintColor?
---@return void
function Lime.Log(msg, color) end

--- If set to true, Lime will close on any error. A pop-up will be disclosed prior with error details.
---@param doEnd boolean
---@return void
function Lime.SetEndOnError(doEnd) end

--- Closes the Lime application.
---@return void
function Lime.Close() end

--- Returns the Lime version running.
---@return string
function Lime.GetVersion() end

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

---@class Event
Event = Event or {}

--- Hook a function to this Event.
---@param Function function
---@return Hook
function Event:Hook(Function) end

--- Clears all functions hooked to this Event.
---@return void
function Event:Clear() end

--- Run this Event.
---@param ... any
---@return void
function Event:Run(...) end

--- Returns the number of hooked functions on this Event.
---@return number
function Event:Length() end

---@class Hook
Hook = Hook or {}

--- Unhook a function to this Event.
---@return void
function Hook:Unhook() end

--- Returns true if this hook is still hooked to an Event.
---@return boolean
function Hook:IsHooked() end