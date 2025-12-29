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
--- Enables the debugger console.
---@param show boolean
function Lime.SetShowDebugger(show) end
--- Write the debugger output to the project directory.
---@param output boolean
function Lime.SetWriteDebugOutput(output) end
--- Closes the application.
function Lime.Close() end
--- Retrieves the running Lime version.
---@return string
function Lime.GetVersion() end
--- Get the elapsed time the application has been running for.
---@return integer
function Lime.GetTime() end
--- Returns the frame rate.
---@return integer
function Lime.GetFrameRate() end
--- Returns the current delta time.
---@return number
function Lime.GetDeltaTime() end
--- Sets the frame limit.
---@param limit integer
function Lime.SetFrameLimit(limit) end
--- Sets vertical syncronization.
---@param enable boolean
function Lime.SetVSync(enable) end
--- Returns the amount of memory being used, in megabytes.
---@return integer
function Lime.GetMemoryUsage() end