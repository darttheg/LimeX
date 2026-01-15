---@class Lime
Lime = Lime or {}

--- Prints a message to console.
---@param msg string
---@param color Lime.PrintColor?
function Lime.Log(msg, color) end

--- End application if an error occurs?
---@param endOnError boolean
function Lime.SetEndOnError(endOnError) end

--- Closes the application.
function Lime.Close() end

--- Retrieves the running Lime version.
---@return string
function Lime.GetVersion() end