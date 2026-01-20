---@class Lime
---@field End Event
---@field Init Event
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

---@class Event
Event = Event or {}

--- Hook a function to this Event.
---@param Event function
---@return Hook
function Event:Hook(Event) end

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