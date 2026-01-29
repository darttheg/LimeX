---@class Lime
--- Event called by Lime prior to initializing the window.
---@field Init Event
--- Event called by Lime every rendering frame. This Event is run with a number delta time argument.
---@field Update Event
--- Event called by Lime once the application ends in any way.
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

--- Returns the number of hooked functions on this Event.
---@return number
function Event:Length() end

--- Run this Event.
---@param ... any
function Event:Run(...) end

--- Returns true if this hook is still hooked to an Event.
---@return boolean
function Hook:IsHooked() end

--- Unhook a function to this Event.
function Hook:Unhook() end

--- Closes the Lime application.
function Lime.Close() end

--- Returns the Lime version running.
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

--- Measures the angle between vectors in degrees
---@param other Vec2
---@return number
function Vec2:Angle(other) end

--- Returns a clamped vector to vectors min and max.
---@param min Vec2
---@param max Vec2
---@return Vec2
function Vec2:Clamp(min, max) end

--- Measures signed scalar area, indicating clockwise versus counter-clockwise orientation.
---@param other Vec2
---@return number
function Vec2:Cross(other) end

--- Measures alignment of two vectors; >0 - same direction, 0 - perpendicular, <0 - opposite.
---@param other Vec2
---@return number
function Vec2:Dot(other) end

--- Returns the length of the vector.
---@return number
function Vec2:GetLength() end

--- Returns the length of the vector save the square root operation.
---@return number
function Vec2:GetLengthSqr() end

--- Returns true if the vector is effectively zero.
---@param epsilon number?
---@return boolean
function Vec2:IsNearlyZero(epsilon) end

--- Returns a linearly-lerped vector interpolated from vectors a to b by number t.
---@param a Vec2
---@param b Vec2
---@param t number
---@return Vec2
function Vec2:Lerp(a, b, t) end

--- Returns a normalized unit vector.
---@return Vec2
function Vec2:Normalize() end

--- Returns a normalied vector scaled to clamp between numbers min and max.
---@param min number
---@param max number
---@return Vec2
function Vec2:NormalizeRng(min, max) end
