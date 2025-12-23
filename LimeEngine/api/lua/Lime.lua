---@class Lime
Lime = Lime or {}
--- Prints a message to console.
---@param msg string
---@param color Lime.PrintColor?
function Lime.Log(msg, color) end
--- Displays a message above the application.
---@param msg string
---@param title string?
---@param icon Lime.PopUpIcon?
function Lime.DisplayMessage(msg, title, icon) end
--- End application if an error occurs?
---@param endOnError boolean
function Lime.SetEndOnError(endOnError) end
--- Closes the application.
function Lime.Close() end
--- Retrieves the running Lime version.
---@return string
function Lime.GetVersion() end
--- Get the elapsed time the application has been running for.
---@return integer
function Lime.GetTime() end