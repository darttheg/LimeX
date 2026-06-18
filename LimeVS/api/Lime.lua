---@meta

Lime = {}
--- @class Lime.File
Lime.File = {}

--- @param path string
--- @return boolean
function Lime.File.isDirectory(path) end

--- @param path string
--- @param extension string?
--- @return table
function Lime.File.getFilesInDirectory(path, extension) end

--- @param path string
--- @return boolean
function Lime.File.isFile(path) end

--- @param path string
--- @param seed string?
--- @return string
function Lime.File.readFile(path, seed) end

--- Writes content to file at `path`. The file will be created if it does not exist. Optionally, a provided `seed` will encrypt the data.
--- @param path string
--- @param data string
--- @param seed string?
--- @return boolean
function Lime.File.writeFile(path, data, seed) end

--- @param path string
--- @return string
function Lime.File.getExtension(path) end

--- @param path string
--- @return string
function Lime.File.getFileName(path) end

--- Creates a directory.
--- @param path string
--- @return boolean
function Lime.File.createDirectory(path) end

--- @class Lime.GUI
Lime.GUI = {}

--- **This function cannot be run until window creation.** Loads a bitmap font. Returns the name of this font, cut from `path`. (NOTE: `path` must be the path to a .xml file. The .xml files must be paired by an image file.)
--- @param path string
--- @return string
function Lime.GUI.loadXML(path) end

--- **This function cannot be run until window creation.** Sets the default font for new GUI elements to font `name`.
--- @param name string
function Lime.GUI.setDefaultFont(name) end

--- **This function cannot be run until window creation.** Returns true if the font `name` is loaded.
--- @param name string
--- @return boolean
function Lime.GUI.isFontLoaded(name) end

--- **This function cannot be run until window creation.** Sets the quality of all GUI elements using `Lime.Enum.Quality` presets, where Low is unfiltered and High is smooth.
--- @param quality Lime.Enum.Quality
function Lime.GUI.setQuality(quality) end

--- **This function cannot be run until window creation.** Unfocuses any element that is focused.
function Lime.GUI.unfocus() end

--- **This function cannot be run until window creation.** Passes a `Shader` to the renderer to be used on GUI elements. It is recommended for the `Shader` to use a non-solid material type.
--- @param shader Shader
function Lime.GUI.setPostProcessingShader(shader) end

--- **This function cannot be run until window creation.** Clears the `Shader` applied to the GUI, if any.
function Lime.GUI.clearPostProcessingShader() end

--- **This function cannot be run until window creation.** Loads a Truetype font. Provide `name` to set the name manually, otherwise Lime will register the font as fontname_size. Returns the output font name.
--- @overload fun(path: string, fontSize: number, aa: boolean?): string
--- @param path string
--- @param fontSize number
--- @param name string
--- @param aa boolean?
--- @return string
function Lime.GUI.loadTTF(path, fontSize, name, aa) end

--- @class Lime_Input_onControllerButtonReleased
Lime_Input_onControllerButtonReleased = {}
--- @param Function fun(id: number, button: Lime.Enum.Controller)
--- @return Hook
function Lime_Input_onControllerButtonReleased:hook(Function) end
function Lime_Input_onControllerButtonReleased:clear() end
--- @param ... any
function Lime_Input_onControllerButtonReleased:run(...) end
--- @return number
function Lime_Input_onControllerButtonReleased:length() end

--- @class Lime_Input_onControllerButtonPressed
Lime_Input_onControllerButtonPressed = {}
--- @param Function fun(id: number, button: Lime.Enum.Controller)
--- @return Hook
function Lime_Input_onControllerButtonPressed:hook(Function) end
function Lime_Input_onControllerButtonPressed:clear() end
--- @param ... any
function Lime_Input_onControllerButtonPressed:run(...) end
--- @return number
function Lime_Input_onControllerButtonPressed:length() end

--- @class Lime_Input_onControllerDisconnected
Lime_Input_onControllerDisconnected = {}
--- @param Function fun(id: number)
--- @return Hook
function Lime_Input_onControllerDisconnected:hook(Function) end
function Lime_Input_onControllerDisconnected:clear() end
--- @param ... any
function Lime_Input_onControllerDisconnected:run(...) end
--- @return number
function Lime_Input_onControllerDisconnected:length() end

--- @class Lime_Input_onControllerConnected
Lime_Input_onControllerConnected = {}
--- @param Function fun(id: number)
--- @return Hook
function Lime_Input_onControllerConnected:hook(Function) end
function Lime_Input_onControllerConnected:clear() end
--- @param ... any
function Lime_Input_onControllerConnected:run(...) end
--- @return number
function Lime_Input_onControllerConnected:length() end

--- @class Lime_Input_onMouseScroll
Lime_Input_onMouseScroll = {}
--- @param Function fun(delta: number)
--- @return Hook
function Lime_Input_onMouseScroll:hook(Function) end
function Lime_Input_onMouseScroll:clear() end
--- @param ... any
function Lime_Input_onMouseScroll:run(...) end
--- @return number
function Lime_Input_onMouseScroll:length() end

--- @class Lime_Input_onMouseMoved
Lime_Input_onMouseMoved = {}
--- @param Function fun(delta: Vec2)
--- @return Hook
function Lime_Input_onMouseMoved:hook(Function) end
function Lime_Input_onMouseMoved:clear() end
--- @param ... any
function Lime_Input_onMouseMoved:run(...) end
--- @return number
function Lime_Input_onMouseMoved:length() end

--- @class Lime_Input_onMouseButtonReleased
Lime_Input_onMouseButtonReleased = {}
--- @param Function fun(button: Lime.Enum.Mouse)
--- @return Hook
function Lime_Input_onMouseButtonReleased:hook(Function) end
function Lime_Input_onMouseButtonReleased:clear() end
--- @param ... any
function Lime_Input_onMouseButtonReleased:run(...) end
--- @return number
function Lime_Input_onMouseButtonReleased:length() end

--- @class Lime_Input_onMouseButtonPressed
Lime_Input_onMouseButtonPressed = {}
--- @param Function fun(button: Lime.Enum.Mouse)
--- @return Hook
function Lime_Input_onMouseButtonPressed:hook(Function) end
function Lime_Input_onMouseButtonPressed:clear() end
--- @param ... any
function Lime_Input_onMouseButtonPressed:run(...) end
--- @return number
function Lime_Input_onMouseButtonPressed:length() end

--- @class Lime_Input_onKeyReleased
Lime_Input_onKeyReleased = {}
--- @param Function fun(key: Lime.Enum.Key)
--- @return Hook
function Lime_Input_onKeyReleased:hook(Function) end
function Lime_Input_onKeyReleased:clear() end
--- @param ... any
function Lime_Input_onKeyReleased:run(...) end
--- @return number
function Lime_Input_onKeyReleased:length() end

--- @class Lime_Input_onKeyPressed
Lime_Input_onKeyPressed = {}
--- @param Function fun(key: Lime.Enum.Key)
--- @return Hook
function Lime_Input_onKeyPressed:hook(Function) end
function Lime_Input_onKeyPressed:clear() end
--- @param ... any
function Lime_Input_onKeyPressed:run(...) end
--- @return number
function Lime_Input_onKeyPressed:length() end

--- @class Lime.Input
--- @field onKeyPressed Lime_Input_onKeyPressed Event called by Lime when a key is pressed.
--- @field onKeyReleased Lime_Input_onKeyReleased Event called by Lime when a key is released.
--- @field onMouseButtonPressed Lime_Input_onMouseButtonPressed Event called by Lime when a mouse button is pressed.
--- @field onMouseButtonReleased Lime_Input_onMouseButtonReleased Event called by Lime when a mouse button is released.
--- @field onMouseMoved Lime_Input_onMouseMoved Event called by Lime when a mouse is moved.
--- @field onMouseScroll Lime_Input_onMouseScroll Event called by Lime when the mouse scroll wheel is moved.
--- @field onControllerConnected Lime_Input_onControllerConnected Event called by Lime when a controller is connected.
--- @field onControllerDisconnected Lime_Input_onControllerDisconnected Event called by Lime when a controller is disconnected.
--- @field onControllerButtonPressed Lime_Input_onControllerButtonPressed Event called by Lime when a controller button is pressed.
--- @field onControllerButtonReleased Lime_Input_onControllerButtonReleased Event called by Lime when a controller button is released.
Lime.Input = {}

--- @param key Lime.Enum.Key
--- @return boolean
function Lime.Input.isKeyDown(key) end

--- @param button Lime.Enum.Mouse
--- @return boolean
function Lime.Input.isMouseButtonDown(button) end

--- @return Vec2
function Lime.Input.getMouseDelta() end

--- @return Vec2
function Lime.Input.getMousePosition() end

--- Sets the mouse's position.
--- @param pos Vec2
function Lime.Input.setMousePosition(pos) end

--- Set the visibility of the mouse cursor.
--- @param visible boolean
function Lime.Input.setMouseVisible(visible) end

--- @return boolean
function Lime.Input.isMouseVisible() end

--- @param id number
--- @param button Lime.Enum.Controller
--- @return boolean
function Lime.Input.isButtonDown(id, button) end

--- @param id number
--- @param axis Lime.Enum.ControllerAxis
--- @return number
function Lime.Input.getControllerAxis(id, axis) end

--- @param id number
--- @return boolean
function Lime.Input.isControllerConnected(id) end

--- @param id number
--- @return string
function Lime.Input.getControllerName(id) end

--- Sets the mouse behavior type.
--- @param type Lime.Enum.MouseType
function Lime.Input.setMouseType(type) end

--- @return Lime.Enum.MouseType
function Lime.Input.getMouseType() end

--- @class Lime_onClose
Lime_onClose = {}
--- @param Function fun()
--- @return Hook
function Lime_onClose:hook(Function) end
function Lime_onClose:clear() end
--- @param ... any
function Lime_onClose:run(...) end
--- @return number
function Lime_onClose:length() end

--- @class Lime_onUpdate
Lime_onUpdate = {}
--- @param Function fun(dt: number)
--- @return Hook
function Lime_onUpdate:hook(Function) end
function Lime_onUpdate:clear() end
--- @param ... any
function Lime_onUpdate:run(...) end
--- @return number
function Lime_onUpdate:length() end

--- @class Lime_onStart
Lime_onStart = {}
--- @param Function fun()
--- @return Hook
function Lime_onStart:hook(Function) end
function Lime_onStart:clear() end
--- @param ... any
function Lime_onStart:run(...) end
--- @return number
function Lime_onStart:length() end

--- @class Lime_onInit
Lime_onInit = {}
--- @param Function fun()
--- @return Hook
function Lime_onInit:hook(Function) end
function Lime_onInit:clear() end
--- @param ... any
function Lime_onInit:run(...) end
--- @return number
function Lime_onInit:length() end

--- @class Lime
--- @field onInit Lime_onInit Event called by Lime prior to initializing the window.
--- @field onStart Lime_onStart Event called by Lime following window creation and rendering services are available.
--- @field onUpdate Lime_onUpdate Event called by Lime every rendering frame. This Event is run with a number `dt` argument.
--- @field onClose Lime_onClose Event called by Lime once the application closes in any way.
Lime = {}
Lime.Enum = {}

--- Prints a message to console.
--- @param msg any
--- @param color Lime.Enum.PrintColor?
function Lime.log(msg, color) end

--- If set to true, Lime will close on any error. A pop-up will be disclosed prior with error details.
--- @param doEnd boolean
function Lime.setEndOnError(doEnd) end

--- Closes the Lime application.
function Lime.close() end

--- Clears the debug console's lines, only visually.
function Lime.clearDebugConsole() end

--- @return string
function Lime.getVersion() end

--- **This function can only be run before window creation.** This function sets initial parameters for the Lime application.
--- @param driver Lime.Enum.DriverType
--- @param windowSize Vec2?
--- @param renderSize Vec2?
--- @return boolean
function Lime.setInitConfig(driver, windowSize, renderSize) end

--- @return number
function Lime.getElapsedTime() end

--- @return number
function Lime.getElapsedTimeSeconds() end

--- @return number
function Lime.getFrameRate() end

--- Sets the target frame rate in frames per second.
--- @param target number
function Lime.setFrameRate(target) end

--- Sets vertical syncronization, matching the frame rate to the current monitor's refresh rate.
--- @param vSyncOn boolean
function Lime.setVSync(vSyncOn) end

--- @return boolean
function Lime.getVSync() end

--- @return Lime.Enum.DriverType
function Lime.getDriverType() end

--- **This function can only be run before window creation.** Sets debug console configuration. If `enable` is true, the debug console will appear alongside the application. If `writeOutput` is true, the console's output will be written to a output.log file in the application directory. If `suppressWarnings" is true, all warnings will not be logged in the debug console to reduce potential clutter while testing. **Warnings should not be suppressed in shipped applications.**
--- @param enable boolean
--- @param writeOutput boolean?
--- @param suppressWarnings boolean?
function Lime.setDebugConfig(enable, writeOutput, suppressWarnings) end

--- @return number
function Lime.getMemoryUsage() end

--- Executes `cmd` in the system's command line.
--- @param cmd string
--- @return number
function Lime.executeCommandLine(cmd) end

--- @param arg string
--- @return string
function Lime.getCommandLineArg(arg) end

--- Displays a pop-up message.
--- @param title string
--- @param message string
--- @param icon Lime.Enum.PopUpIcon?
function Lime.displayMessage(title, message, icon) end

--- Loads an archive of assets to the application. Content is accessed as if it were at the application's root. For example: If the archive contains folder/image.png, the path ./folder/image.png is valid for loading `Texture` objects.
--- @param path string
--- @return boolean
function Lime.loadArchive(path) end

--- @class math
math = {}

--- Clamps `v` to `min`, `max`.
--- @param v number|Vec2|Vec3|Vec4
--- @param min number
--- @param max number
--- @return Vec4
function math.clamp(v, min, max) end

--- @class math.tween
math.tween = {}

--- Interpolates from `old` toward `target` using exponential smoothing.
--- @param old number|Vec2|Vec3|Vec4
--- @param target number|Vec2|Vec3|Vec4
--- @param factor number
--- @param dt number
--- @return Vec4
function math.tween.damp(old, target, factor, dt) end

--- Linearly interpolates from `old` to `target` over `a`, where `a` is between 0.0 and 1.0.
--- @param old number|Vec2|Vec3|Vec4
--- @param target number|Vec2|Vec3|Vec4
--- @param a number
--- @return Vec4
function math.tween.lerp(old, target, a) end

--- Starts slow, accelerates. (Sine curve)
--- @param a number
--- @return number
function math.tween.easeInSine(a) end

--- Starts fast, decelerates. (Sine curve)
--- @param a number
--- @return number
function math.tween.easeOutSine(a) end

--- Smooth start and end. (Sine curve)
--- @param a number
--- @return number
function math.tween.easeInOutSine(a) end

--- Starts slow, accelerates. (Cubic curve)
--- @param a number
--- @return number
function math.tween.easeInCubic(a) end

--- Starts fast, decelerates. (Cubic curve)
--- @param a number
--- @return number
function math.tween.easeOutCubic(a) end

--- Smooth start and end. (Cubic curve)
--- @param a number
--- @return number
function math.tween.easeInOutCubic(a) end

--- Starts by moving slightly backward, then accelerates forward.
--- @param a number
--- @return number
function math.tween.easeInBack(a) end

--- Overshoots, then settles back.
--- @param a number
--- @return number
function math.tween.easeOutBack(a) end

--- Backward start, then overshoots to settle.
--- @param a number
--- @return number
function math.tween.easeInOutBack(a) end

--- Starts slow with oscillation, like a stretched spring.
--- @param a number
--- @return number
function math.tween.easeInElastic(a) end

--- Ends with oscillation, like a spring settling.
--- @param a number
--- @return number
function math.tween.easeOutElastic(a) end

--- Oscillates at both the start and end.
--- @param a number
--- @return number
function math.tween.easeInOutElastic(a) end

--- Starts with a bounce effect.
--- @param a number
--- @return number
function math.tween.easeInBounce(a) end

--- Ends with a bounce effect.
--- @param a number
--- @return number
function math.tween.easeOutBounce(a) end

--- Bounce effect and both the start and end.
--- @param a number
--- @return number
function math.tween.easeInOutBounce(a) end

--- Starts slow and heavily speeds up at the end.
--- @param a number
--- @return number
function math.tween.easeInExpo(a) end

--- Starts fast, then plateaus.
--- @param a number
--- @return number
function math.tween.easeOutExpo(a) end

--- Slow at the start and end, but fast in the middle.
--- @param a number
--- @return number
function math.tween.easeInOutExpo(a) end

--- @class Lime_Network_onReceive
Lime_Network_onReceive = {}
--- @param Function fun(received: Packet, peerID: number, channel: number)
--- @return Hook
function Lime_Network_onReceive:hook(Function) end
function Lime_Network_onReceive:clear() end
--- @param ... any
function Lime_Network_onReceive:run(...) end
--- @return number
function Lime_Network_onReceive:length() end

--- @class Lime_Network_onPeerDisconnect
Lime_Network_onPeerDisconnect = {}
--- @param Function fun(peerID: number)
--- @return Hook
function Lime_Network_onPeerDisconnect:hook(Function) end
function Lime_Network_onPeerDisconnect:clear() end
--- @param ... any
function Lime_Network_onPeerDisconnect:run(...) end
--- @return number
function Lime_Network_onPeerDisconnect:length() end

--- @class Lime_Network_onPeerConnect
Lime_Network_onPeerConnect = {}
--- @param Function fun(peerID: number)
--- @return Hook
function Lime_Network_onPeerConnect:hook(Function) end
function Lime_Network_onPeerConnect:clear() end
--- @param ... any
function Lime_Network_onPeerConnect:run(...) end
--- @return number
function Lime_Network_onPeerConnect:length() end

--- @class Lime_Network_onDisconnect
Lime_Network_onDisconnect = {}
--- @param Function fun(code: number)
--- @return Hook
function Lime_Network_onDisconnect:hook(Function) end
function Lime_Network_onDisconnect:clear() end
--- @param ... any
function Lime_Network_onDisconnect:run(...) end
--- @return number
function Lime_Network_onDisconnect:length() end

--- @class Lime_Network_onConnect
Lime_Network_onConnect = {}
--- @param Function fun()
--- @return Hook
function Lime_Network_onConnect:hook(Function) end
function Lime_Network_onConnect:clear() end
--- @param ... any
function Lime_Network_onConnect:run(...) end
--- @return number
function Lime_Network_onConnect:length() end

--- @class Lime.Network
--- @field onConnect Lime_Network_onConnect Event called by Lime as a **peer** when the client connects to a server.
--- @field onDisconnect Lime_Network_onDisconnect Event called by Lime as a **peer** when the client disconnects from a server.
--- @field onPeerConnect Lime_Network_onPeerConnect Event called by Lime as the **host** when a peer joins the server.
--- @field onPeerDisconnect Lime_Network_onPeerDisconnect Event called by Lime as the **host** when a peer disconnects from the server.
--- @field onReceive Lime_Network_onReceive Event called by Lime when the application receives a `Packet`. If the application is a **peer**, `peerID` will be -1.
Lime.Network = {}

--- Hosts a server. Returns true on success.
--- @param port number
--- @param maxPlayers number?
--- @return boolean
function Lime.Network.host(port, maxPlayers) end

--- Attempts to connect to a server.
--- @param ip string
--- @param port integer
function Lime.Network.connect(ip, port) end

--- Disconnects from a server. If this application is the host, it will close the server.
function Lime.Network.disconnect() end

--- @return boolean
function Lime.Network.isHosting() end

--- @return boolean
function Lime.Network.isConnected() end

--- **This function can only be run by a server host.** Sets the incoming and outgoing bandwidth limits in bytes per second.
--- @param incoming number
--- @param outgoing number
function Lime.Network.setBandwidthLimits(incoming, outgoing) end

--- **This function can only be run by a server host.** Returns the state of a peer.
--- @param peerID number
--- @return Lime.Enum.PeerState
function Lime.Network.getPeerState(peerID) end

--- **This function can only be run by a server host.** Returns the ping of a peer in milliseconds.
--- @param peerID number
--- @return number
function Lime.Network.getPeerPing(peerID) end

--- **This function can only be run by a server host.** Forcefully disconnects a peer with an optional reason code.
--- @param peerID number
--- @param code number?
function Lime.Network.disconnectPeer(peerID, code) end

--- **This function can only be run by a server host.** Forcefully disconnects a peer with an optional reason code, as well as appending the peer's IP address to the bans list. Returns the peer's IP address.
--- @param peerID number
--- @param code number?
--- @return number
function Lime.Network.banPeer(peerID, code) end

--- **This function can only be run by a server host.** Returns the IP address of a peer.
--- @param peerID number
--- @return number
function Lime.Network.getPeerIP(peerID) end

--- **This function can only be run by a server host.** Returns the number of connected peers.
--- @return number
function Lime.Network.getPeerCount() end

--- Bans peers under the provided IP address(es) from connecting to the server.
--- @param ... number
function Lime.Network.banIP(...) end

--- Unbans the IP address(es).
--- @param ... number
function Lime.Network.unbanIP(...) end

--- Unbans all IP addresses.
function Lime.Network.clearBannedIPs() end

--- @return number[]
function Lime.Network.getBannedIPs() end

--- **This function can only be run by a server host.** Sends a packet to a peer.
--- @param packet Packet
--- @param peerID number
--- @param channel number?
--- @param reliable boolean?
function Lime.Network.sendPacketToPeer(packet, peerID, channel, reliable) end

--- **This function can only be run by a peer of a server.** Sends a packet to the server.
--- @param packet Packet
--- @param channel number?
--- @param reliable boolean?
function Lime.Network.sendPacketToServer(packet, channel, reliable) end

--- **This function can only be run by a server host.** Sends a packet to all connected peers.
--- @param packet Packet
--- @param channel number?
--- @param reliable boolean?
function Lime.Network.sendPacketToAll(packet, channel, reliable) end

--- @class Lime.Physics
Lime.Physics = {}

--- **This function cannot be run until window creation.** Returns the direction and magnitude of global gravity.
--- @return Vec3
function Lime.Physics.getGravity() end

--- **This function cannot be run until window creation.** Sets the direction and magnitude of global gravity.
--- @param gravity Vec3
function Lime.Physics.setGravity(gravity) end

--- **This function cannot be run until window creation.** Returns whether or not the scene's physics simulation is paused.
--- @return boolean
function Lime.Physics.isPaused() end

--- **This function cannot be run until window creation.** Sets whether or not the scene's physics simulation is paused.
--- @param paused boolean
function Lime.Physics.setPaused(paused) end

--- **This function cannot be run until window creation.** Returns the physics simulation step factor.
--- @return number
function Lime.Physics.getStepFactor() end

--- **This function cannot be run until window creation.** Sets the physics simulation step factor.
--- @param factor number
function Lime.Physics.setStepFactor(factor) end

--- **This function cannot be run until window creation.** Sets the physics simulation debug mode.
--- @param type Lime.Enum.PhysicsDebugType
function Lime.Physics.setDebug(type) end

--- **This function cannot be run until window creation.** Sets whether or not collisions with objects sharing the same ID should call a collision `Event`.
--- @param ignore boolean
function Lime.Physics.setIgnoreEqualID(ignore) end

--- Sets the fixed internal simulation timestep size. Smaller timesteps are more accurate at the cost of more computation per frame. Default is 1/30.
--- @param fixedStep number
function Lime.Physics.setFixedTimeStep(fixedStep) end

--- Sets the maximum number of sub-steps allowed to run per frame. This restricts the physics simulation from taking too many sub-steps to catch up if a frame takes longer than usual to compute. Default is 8.
--- @param maxSteps number
function Lime.Physics.setMaxSubSteps(maxSteps) end

--- @return number
function Lime.Physics.getObjectCount() end

--- @class Lime.Scene
Lime.Scene = {}

--- @return number
function Lime.Scene.getObjectCount() end

--- @return number
function Lime.Scene.getTextureCount() end

--- @return number
function Lime.Scene.getMeshCount() end

--- **This function cannot be run until window creation.** Sets the ambient color of the scene to `rgba`.
--- @param rgba Vec4
function Lime.Scene.setAmbientColor(rgba) end

--- **This function cannot be run until window creation.** Sets the background color of the scene to `rgba`. This color is generally only visible when there is no `Skydome`.
--- @param rgba Vec4
function Lime.Scene.setBackgroundColor(rgba) end

--- **This function cannot be run until window creation.** Sets the color of the scene's fog to `rgba`.
--- @param rgba Vec4
function Lime.Scene.setFogColor(rgba) end

--- **This function cannot be run until window creation.** Sets where the scene's fog starts and ends.
--- @param near number
--- @param far number
function Lime.Scene.setFogRange(near, far) end

--- **This function cannot be run until window creation.** Sets the color of shadows in the scene to `rgba`.
--- @param rgba Vec4
function Lime.Scene.setShadowColor(rgba) end

--- **This function cannot be run until window creation.** Sets the light management behavior using `Lime.Enum.LightManagementType`.
--- @param type Lime.Enum.LightManagementType
function Lime.Scene.setLightManagementType(type) end

--- **This function cannot be run until window creation.** Sets the default `Texture` creation quality using `Lime.Enum.TextureCreationQuality`, where Low is optimized for speed and High is optimized for quality. **WARNING**: If post processing effects or window-render-matching are active, using this function will influence the quality of the application output.
--- @param quality Lime.Enum.TextureCreationQuality
function Lime.Scene.setTextureCreationQuality(quality) end

--- **This function cannot be run until window creation.** Returns an `Texture` of a lime and white checkerboard pattern, 2x2. Useful for missing Textures and the like.
--- @return Texture
function Lime.Scene.getErrorTexture() end

--- If set to true, the rendering resolution will be that of the window resolution, updating on any window size changes. If false, the rendering resolution will remain the same size but upscale to the window resolution using a letterboxed viewport.
--- @param dynamic boolean
function Lime.Scene.setDynamicResolution(dynamic) end

--- Sets the render size to `size`. If the render size is set to rescale to the window size then this change will not take effect. See `Lime.Scene.setDynamicResolution`.
--- @param size Vec2
function Lime.Scene.setSize(size) end

--- Sets the render quality of the scene using `Lime.Enum.Quality`. **WARNING**: This function only takes effect if post processing effects or window-render-matching are active.
--- @param quality Lime.Enum.Quality
function Lime.Scene.setRenderQuality(quality) end

--- **This function cannot be run until window creation.** Returns a `Mesh` containing a cube.
--- @param size Vec3
--- @return Mesh
function Lime.Scene.createCubeMesh(size) end

--- **This function cannot be run until window creation.** Returns a `Mesh` containing a sphere.
--- @param radius number
--- @param polyCount number?
--- @return Mesh
function Lime.Scene.createSphereMesh(radius, polyCount) end

--- **This function cannot be run until window creation.** Returns a `Mesh` containing a cylinder.
--- @param radius number
--- @param length number
--- @param polyCount number?
--- @param closed boolean?
--- @return Mesh
function Lime.Scene.createCylinderMesh(radius, length, polyCount, closed) end

--- **This function cannot be run until window creation.** Returns a `Mesh` containing a plane. Parameter `repeatCount` controls how much an applied `Texture` will repeat within one tile.
--- @param tileSize Vec2
--- @param tileCount Vec2
--- @param repeatCount Vec2?
--- @return Mesh
function Lime.Scene.createPlaneMesh(tileSize, tileCount, repeatCount) end

--- If set to false, the application will not update the scene output. A use case is a pseudo-pause your game to save on resources while loading assets.
--- @param active boolean
function Lime.Scene.setRenderingActive(active) end

--- @return boolean
function Lime.Scene.isRenderingActive() end

--- **This function cannot be run until window creation.** Passes a `Shader` to the renderer be used for special effects on the scene output.
--- @param shader Shader
function Lime.Scene.setPostProcessingShader(shader) end

--- **This function cannot be run until window creation.** Clears the `Shader` applied to the screen, if any.
function Lime.Scene.clearPostProcessingShader() end

--- **This function cannot be run until window creation.** Converts a 3D position to `Vec2` on the screen.
--- @param pos Vec3
--- @return Vec2
function Lime.Scene.toScreenPosition(pos) end

--- **This function cannot be run until window creation.** Preloads a mesh by `path` into the scene for later use. Returns true on success or if the mesh at `path` has already been preloaded.
--- @param path string
--- @return boolean
function Lime.Scene.preloadMesh(path) end

--- **This function cannot be run until window creation.** Purges a mesh by `path` from the scene. Returns true on success or if the mesh at `path` has already been purged. `Mesh` objects referencing this mesh will use an engine-defined `Mesh` instead.
--- @param path string
--- @return boolean
function Lime.Scene.purgeMesh(path) end

--- **This function cannot be run until window creation.** Preloads a texture by `path` into the scene for later use. Returns true on success or if the texture at `path` has already been preloaded.
--- @param path string
--- @return boolean
function Lime.Scene.preloadTexture(path) end

--- **This function cannot be run until window creation.** Purges a texture by `path` from the scene. Returns true on success or if the texture at `path` has already been purged. `Texture` objects referencing this texture will use an engine-defined `Texture` instead.
--- @param path string
--- @return boolean
function Lime.Scene.purgeTexture(path) end

--- **This function cannot be run until window creation.** Sets a `Texture` to be drawn over the screen, under the GUI layer. This `Texture` will inherit post processing shaders, if post processing is enabled.
--- @param texture Texture
function Lime.Scene.setOverlayTexture(texture) end

--- **This function cannot be run until window creation.** Clears the user-defined `Texture` drawn over the screen, if any.
function Lime.Scene.clearOverlayTexture() end

--- **This function cannot be run until window creation.** Fires a raycast out into the scene from `startPos` to `endPos`. Only objects with collision enabled will be tested.
--- @param startPos Vec3
--- @param endPos Vec3
--- @param rayLifeMs number?
--- @return HitResult
function Lime.Scene.fireRaycast(startPos, endPos, rayLifeMs) end

--- **This function cannot be run until window creation.** Fires a raycast out from a screenspace position `Vec2` of length `length`. Only objects with collision enabled will be tested.
--- @param startPos Vec2
--- @param length number
--- @param rayLifeMs number?
--- @return HitResult
function Lime.Scene.fireScreenspaceRaycast(startPos, length, rayLifeMs) end

--- @class Lime.Audio
Lime.Audio = {}

--- @return number
function Lime.Audio.getVelocityFactor() end

--- Sets the factor to scale listener velocity by. Listener velocity is calculated with the last and current active `Camera` positions. In the event that the active `Camera` is swapped, listener velocity will be 0 on the first frame automatically.
--- @param factor number
function Lime.Audio.setVelocityFactor(factor) end

--- **This function cannot be run until window creation.** Returns the application's main volume.
--- @return number
function Lime.Audio.getMainVolume() end

--- **This function cannot be run until window creation.** Sets the application's main volume.
--- @param volume number
function Lime.Audio.setMainVolume(volume) end

--- **This function cannot be run until window creation.** Sets all `Sound` objects to paused or unpaused.
--- @param paused boolean
function Lime.Audio.setAllSoundsPaused(paused) end

--- **This function cannot be run until window creation.** Sets the default minimum listening distance for new sounds.
--- @param distance number
function Lime.Audio.setDefaultMinimumDistance(distance) end

--- **This function cannot be run until window creation.** Sets the default maximum listening distance for new sounds.
--- @param distance number
function Lime.Audio.setDefaultMaximumDistance(distance) end

--- Determines unfocused window behavior in regards to application volume.
--- @param mute boolean
function Lime.Audio.setMuteWhileUnfocused(mute) end

--- **This function cannot be run until window creation.** Preloads a sound by `path` for later use. Returns true on success or if the sound at `path` has already been preloaded.
--- @param path string
--- @return boolean
function Lime.Audio.preloadSound(path) end

--- **This function cannot be run until window creation.** Returns the amount of sounds loaded in the scene.
--- @return number
function Lime.Audio.getSoundCount() end

--- @class Lime_Web_onDownloadFail
Lime_Web_onDownloadFail = {}
--- @param Function fun(status: number)
--- @return Hook
function Lime_Web_onDownloadFail:hook(Function) end
function Lime_Web_onDownloadFail:clear() end
--- @param ... any
function Lime_Web_onDownloadFail:run(...) end
--- @return number
function Lime_Web_onDownloadFail:length() end

--- @class Lime_Web_onDownloadComplete
Lime_Web_onDownloadComplete = {}
--- @param Function fun(status: number, path: string)
--- @return Hook
function Lime_Web_onDownloadComplete:hook(Function) end
function Lime_Web_onDownloadComplete:clear() end
--- @param ... any
function Lime_Web_onDownloadComplete:run(...) end
--- @return number
function Lime_Web_onDownloadComplete:length() end

--- @class Lime_Web_onGetFail
Lime_Web_onGetFail = {}
--- @param Function fun(status: number)
--- @return Hook
function Lime_Web_onGetFail:hook(Function) end
function Lime_Web_onGetFail:clear() end
--- @param ... any
function Lime_Web_onGetFail:run(...) end
--- @return number
function Lime_Web_onGetFail:length() end

--- @class Lime_Web_onGetComplete
Lime_Web_onGetComplete = {}
--- @param Function fun(status: number, content: string)
--- @return Hook
function Lime_Web_onGetComplete:hook(Function) end
function Lime_Web_onGetComplete:clear() end
--- @param ... any
function Lime_Web_onGetComplete:run(...) end
--- @return number
function Lime_Web_onGetComplete:length() end

--- @class Lime.Web
--- @field onGetComplete Lime_Web_onGetComplete Event called by Lime when a GET request has successfully completed.
--- @field onGetFail Lime_Web_onGetFail Event called by Lime when a GET request has failed.
--- @field onDownloadComplete Lime_Web_onDownloadComplete Event called by Lime when a download has successfully completed.
--- @field onDownloadFail Lime_Web_onDownloadFail Event called by Lime when a download has failed.
Lime.Web = {}

--- Sends an HTTP GET request to `url`. See `Lime.Web.onGetComplete` and `Lime.Web.onGetFail`.
--- @param url string
function Lime.Web.get(url) end

--- Downloads a file from `url` and saves it to `path`. See `Lime.Web.onDownloadComplete` and `Lime.Web.onDownloadFail`.
--- @param url string
--- @param path string
function Lime.Web.download(url, path) end

--- Cancels the ongoing download, if any.
function Lime.Web.cancelDownload() end

--- @return number
function Lime.Web.getDownloadProgress() end

--- @return number
function Lime.Web.getDownloadSpeed() end

--- @return boolean
function Lime.Web.isDownloading() end

--- @return boolean
function Lime.Web.isExtracting() end

--- Sets the timeout in milliseconds for HTTP requests.
--- @param ms number
function Lime.Web.setTimeout(ms) end

--- @class Lime_Window_onResize
Lime_Window_onResize = {}
--- @param Function fun()
--- @return Hook
function Lime_Window_onResize:hook(Function) end
function Lime_Window_onResize:clear() end
--- @param ... any
function Lime_Window_onResize:run(...) end
--- @return number
function Lime_Window_onResize:length() end

--- @class Lime.Window
--- @field onResize Lime_Window_onResize Event called by Lime once the window is resized in any way.
Lime.Window = {}

--- **This function cannot be run until window creation.** Sets the window's title to `title`.
--- @param title string
function Lime.Window.setTitle(title) end

--- **This function cannot be run until window creation.** Toggles fullscreen mode.
--- @param fullscreen boolean
function Lime.Window.setFullscreen(fullscreen) end

--- **This function cannot be run until window creation.** Returns the window's position.
--- @return Vec2
function Lime.Window.getPosition() end

--- **This function cannot be run until window creation.** Sets the window's position to `pos`.
--- @param pos Vec2
function Lime.Window.setPosition(pos) end

--- **This function cannot be run until window creation.** Returns the size of the window.
--- @return Vec2
function Lime.Window.getSize() end

--- **This function cannot be run until window creation.** Sets the window's size to `size`.
--- @param size Vec2
function Lime.Window.setSize(size) end

--- **This function cannot be run until window creation.** Returns the size of the monitor the window is running on.
--- @return Vec2
function Lime.Window.getMonitorSize() end

--- **This function cannot be run until window creation.** Returns true if the window is focused.
--- @return boolean
function Lime.Window.isFocused() end

--- @return boolean
function Lime.Window.isCreated() end

--- **This function cannot be run until window creation.** Allows the window to be resizable or locked to its intended size. This disables maximizing the application as well.
--- @param allow boolean
function Lime.Window.setResizable(allow) end

--- **This function cannot be run until window creation.** Locks the aspect ratio of the raw window size. It will not make the window immune to all resizing. Maximizing the window will not preserve the rendering aspect ratio.
--- @param locked boolean
function Lime.Window.setLockAspectRatio(locked) end

--- **This function cannot be run until window creation.** Sets the minimum window size. The window size cannot be smaller than the render resolution.
--- @param size Vec2
function Lime.Window.setMinimumSize(size) end

--- @class Object2D_onPressed
Object2D_onPressed = {}
--- @param Function fun()
--- @return Hook
function Object2D_onPressed:hook(Function) end
function Object2D_onPressed:clear() end
--- @param ... any
function Object2D_onPressed:run(...) end
--- @return number
function Object2D_onPressed:length() end

--- @class Object2D_onHovered
Object2D_onHovered = {}
--- @param Function fun()
--- @return Hook
function Object2D_onHovered:hook(Function) end
function Object2D_onHovered:clear() end
--- @param ... any
function Object2D_onHovered:run(...) end
--- @return number
function Object2D_onHovered:length() end

--- @class Object2D
--- @field position Vec2 The 2D position of this object on the screen.
--- @field size Vec2 The 2D size of this object.
--- @field visible boolean Determines the visibility of this object and its children.
--- @field border boolean Displays a border outlining this object's bounding box on the screen.
--- @field enabled boolean Determines if this object will process hooked input events, if any.
--- @field backgroundColor Vec4 The RGBA background color of this object.
--- @field onHovered Object2D_onHovered Event called by Lime when this object is hovered.
--- @field onPressed Object2D_onPressed Event called by Lime when this object is pressed.
Object2D = {}

--- Parents this object to another 2D object.
--- @param parent any
--- @return boolean
function Object2D:parentTo(parent) end

--- @return boolean
function Object2D:hasParent() end

--- @return boolean
function Object2D:isHovered() end

--- Moves this object to the front in terms of z ordering. (Rendered last, overlaps all other objects)
--- @return boolean
function Object2D:moveToFront() end

--- Moves this object to the back in terms of z ordering. (Rendered first, all other objects then overlap)
--- @return boolean
function Object2D:moveToBack() end

--- @return number
function Object2D:getReferenceCount() end

--- @return Vec2
function Object2D:getAbsolutePosition() end

--- Destroys this object.
function Object2D:destroy() end

--- @class Object3D
--- @field position Vec3 The 3D position of this object in the scene.
--- @field rotation Vec3 The 3D rotation of this object in the scene in degrees.
--- @field scale Vec3 The 3D scale of this object in the scene.
--- @field visible boolean Determines the visibility of this object and its children.
--- @field id number The identifier for this object to be used in raycasts and object selection.
--- @field debug boolean Show debug information about this object in the scene.
Object3D = {}

--- Parents this object to another 3D object.
--- @param parent any
--- @return boolean
function Object3D:parentTo(parent) end

--- @return Vec3
function Object3D:getAbsolutePosition() end

--- Updates the absolute position of this 3D object and its children. This is useful if you move a parent object and need to refresh its children's positions for the frame.
function Object3D:updateAbsolutePosition() end

--- @return boolean
function Object3D:hasParent() end

--- @return number
function Object3D:getReferenceCount() end

--- Destroys this object.
function Object3D:destroy() end

--- @return Vec4
function Object3D:getBoundingBox() end

--- @param pos Vec3
--- @return boolean
function Object3D:isPointInside(pos) end

--- Sets `key` to `value` within this object's attributes.
--- @param key any
--- @param value any
function Object3D:setAttribute(key, value) end

--- @param key any
--- @return any
function Object3D:getAttribute(key) end

--- @return table
function Object3D:getAttributes() end

--- Clears this object's attributes.
function Object3D:clearAttributes() end

--- Clears all animators attached to this object.
function Object3D:clearAnimators() end

--- Adds a Destroy animator to this object. After `ms` milliseconds, this object will destroy itself.
--- @param ms number
function Object3D:addDestroyAnimator(ms) end

--- Adds a MoveTo animator to this object. This object will move from `posA` to `posB` over `ms` milliseconds.
--- @param posA Vec3
--- @param posB Vec3
--- @param ms number
--- @param loops boolean?
--- @param pingPong boolean?
function Object3D:addMoveToAnimator(posA, posB, ms, loops, pingPong) end

--- Adds a Rotate animator to this object. This object will rotate `rot` degrees per second.
--- @param rot Vec3
function Object3D:addRotateAnimator(rot) end

--- @class PhysicsObject_onExit
PhysicsObject_onExit = {}
--- @param Function fun()
--- @return Hook
function PhysicsObject_onExit:hook(Function) end
function PhysicsObject_onExit:clear() end
--- @param ... any
function PhysicsObject_onExit:run(...) end
--- @return number
function PhysicsObject_onExit:length() end

--- @class PhysicsObject_onInside
PhysicsObject_onInside = {}
--- @param Function fun()
--- @return Hook
function PhysicsObject_onInside:hook(Function) end
function PhysicsObject_onInside:clear() end
--- @param ... any
function PhysicsObject_onInside:run(...) end
--- @return number
function PhysicsObject_onInside:length() end

--- @class PhysicsObject_onEnter
PhysicsObject_onEnter = {}
--- @param Function fun(result: CollisionResult)
--- @return Hook
function PhysicsObject_onEnter:hook(Function) end
function PhysicsObject_onEnter:clear() end
--- @param ... any
function PhysicsObject_onEnter:run(...) end
--- @return number
function PhysicsObject_onEnter:length() end

--- @class PhysicsObject
--- @field onEnter PhysicsObject_onEnter Event called when another physics object collides with this object for the first time.
--- @field onInside PhysicsObject_onInside Event called when another physics object is inside this physics object.
--- @field onExit PhysicsObject_onExit Event called when another physics object exits this physics object.
PhysicsObject = {}

--- Destroys this object.
function PhysicsObject:destroy() end

--- A plane that faces the active camera.
--- @class Billboard : Object3D
--- @field size Vec2 The width and height of this Billboard.
Billboard = {}

--- @overload fun(material: Material): Billboard
--- @return Billboard
function Billboard.new() end

--- Loads a `Material` into this `Billboard`.
--- @param material Material
function Billboard:loadMaterial(material) end

--- A viewpoint in the 3D world.
--- @class Camera : Object3D
--- @field position Vec3 The 3D position of this object in the scene.
--- @field rotation Vec3 The 3D rotation of this object in the scene in degrees.
--- @field up Vec3 The up vector of this `Camera`.
--- @field viewPlanes Vec2 The near and far clipping planes of this `Camera`.
--- @field fieldOfView number The field of view of this `Camera` in degrees.
--- @field aspectRatio number The aspect ratio of this `Camera`.
--- @field orthogonal boolean Whether or not this `Camera` renders orthographically or not. (NOTE: If this is true, `aspectRatio` modifies the zoom factor instead.)
Camera = {}

--- @overload fun(pos: Vec3): Camera
--- @overload fun(pos: Vec3, rot: Vec3): Camera
--- @return Camera
function Camera.new() end

--- @return Vec3
function Camera:getForward() end

--- @return Vec3
function Camera:getLeft() end

--- When rendering, this `Camera` will take over as the active rendering viewpoint.
function Camera:setActive() end

--- @class Constraint
--- @field active boolean Whether or not this `Constraint` is active.
--- @field ignoreCollision boolean Whether or not the `RigidBody` objects of this `Constraint` should ignore collision between one another. This value should be altered prior to activating this `Constraint` to take effect in the scene.
--- @field breakThreshold number The impulse threshold this `Constraint` can endure before it breaks, deactivating itself. Physics objects default to unbreakable, but altering this value will enable this object to be prone to breaking.
Constraint = {}

--- Destroys this `Constraint`.
function Constraint:destroy() end

--- A physics `Constraint` that hinges two `RigidBody` objects together. Pivots and axis parameters are in local space. Axis vector values range from 0 to 1, where 1 allows rotation around said axis and vice versa.
--- @class HingeConstraint : Constraint
--- @field limits Vec2 The angular limits of the hinge, where `x` is lower limit and `y` is upper limit.
--- @field motor boolean Whether or not this `Constraint` applies angular velocity every physics step, rotating itself like a motor.
--- @field motorVelocity number If this `Constraint` is a motor, this determines the target rotational velocity.
HingeConstraint = {}

--- @param rbA RigidBody
--- @param rbB RigidBody
--- @param localPivotA Vec3
--- @param localPivotB Vec3
--- @param localAxisA Vec3
--- @param localAxisB Vec3
--- @return HingeConstraint
function HingeConstraint.new(rbA, rbB, localPivotA, localPivotB, localAxisA, localAxisB) end

--- A physics `Constraint` that twists two `RigidBody` objects together like a limited socket joint. Pivots and axis parameters are in local space. Axis vector values range from 0 to 1, where 1 allows rotation around said axis and vice versa.
--- @class ConeTwistConstraint : Constraint
--- @field maxMotorImpulse number If this `Constraint` is a motor, this determines the maximum force allowed to spin to reach the target motorVelocity.
--- @field swingLimits Vec2 The angular swing limits of this `Constraint`, where `x` is sideways and `y` is forward and backward.
ConeTwistConstraint = {}

--- @param rbA RigidBody
--- @param rbB RigidBody
--- @param localPivotA Vec3
--- @param localPivotB Vec3
--- @param localAxisA Vec3
--- @param localAxisB Vec3
--- @return ConeTwistConstraint
function ConeTwistConstraint.new(rbA, rbB, localPivotA, localPivotB, localAxisA, localAxisB) end

--- A basic 2D object that allows user input. `EditBox` objects can be used for getting user input and can even be made to look like passwords.
--- @class EditBox : Object2D
--- @field text string The text content of this object.
--- @field password boolean Determines if the text content of this `EditBox` is obfuscated.
--- @field enabled boolean Determines if this `EditBox` is enabled.
--- @field maxChars number The maximum amount of characters allowed.
--- @field cursorPosition number The position of the cursor in this `EditBox`.
--- @field multiLine boolean Determines if this `EditBox` supports multiple lines.
--- @field background boolean Determines if this `EditBox` has a visible background.
--- @field autoscroll boolean Determines if this `EditBox` will autoscroll.
--- @field focused boolean Determines if this `EditBox` is focused.
--- @field wordWrap boolean Determines if the text wraps when touching the border of its text box.
EditBox = {}

--- @overload fun(text: string): EditBox
--- @overload fun(pos: Vec2, size: Vec2): EditBox
--- @return EditBox
function EditBox.new() end

--- Sets the font to use for this object. Fonts must first be loaded. See `Lime.GUI.loadXML` and `Lime.GUI.loadTTF`.
--- @param name string
--- @return boolean
function EditBox:setFont(name) end

--- Sets the text's alignment within its bounding box.
--- @param x Lime.Enum.TextAlign
--- @param y Lime.Enum.TextAlign?
function EditBox:setAlignment(x, y) end

--- An invisible object used to mark locations, parent objects, and more.
--- @class Empty : Object3D
Empty = {}

--- @overload fun(pos: Vec3): Empty
--- @return Empty
function Empty.new() end

--- A container of functions that will run in sequence when called upon.
--- @class Event
Event = {}

--- @return Event
function Event.new() end

--- Hook a function to this Event.
--- @param Function fun(...)
--- @return Hook
function Event:hook(Function) end

--- Clears all functions hooked to this Event.
function Event:clear() end

--- Run this Event.
--- @param ... any
function Event:run(...) end

--- @return number
function Event:length() end

--- A handle from Event hooking.
--- @class Hook
Hook = {}

--- Unhook a function to this Event.
function Hook:unhook() end

--- @return boolean
function Hook:isHooked() end

--- A basic 2D object to display images. Without loading a Texture, this object can be used as a container for other GUI objects if parented together.
--- @class Image2D : Object2D
--- @field scaleToFit boolean Determines whether or not this `Image2D` scales to its size boundaries or not.
Image2D = {}

--- @overload fun(tex: Texture): Image2D
--- @overload fun(pos: Vec2, size: Vec2): Image2D
--- @return Image2D
function Image2D.new() end

--- Loads `texture` into this object.
--- @param texture Texture
--- @return boolean
function Image2D:loadTexture(texture) end

--- A source of light.
--- @class Light : Object3D
--- @field type Lime.Enum.LightType Sets the type of this `Light`.
--- @field intensity number Scales the intensity of luminosity from this `Light`.
--- @field radius number The cut-off distance for light reach around its center. Not effective for directional light sources.
--- @field falloff number Dictates the blend from inner to outer cones for spotlights. For example, <1.0 is soft, 1.0 is linear, 2.0 is a bit harsher, 10.0+ is a very harsh cut-off.
--- @field diffuseColor Vec4 Sets the diffuse color for this `Light`, the main light color.
--- @field ambientColor Vec4 Sets the ambient color for this `Light`, the atmospheric color applied to all objects.
--- @field specularColor Vec4 Sets the specular color for this `Light`, the color that appears on shiny objects.
--- @field attenuation Vec3 Sets the attenuation, or spread behavior, of this `Light`. Format is `(Constant, Linear, Quadratic)`, all ranging from 0.0 to 1.0. Not effective for directional light sources.
Light = {}

--- @overload fun(type: Lime.Enum.LightType): Light
--- @return Light
function Light.new() end

--- An object used to hold material parameters for 3D objects. A Material has at most two layers, with `Lime.Enum.MaterialType` allowing for different combinations of said layers.
--- @class Material
--- @field ID number An ID to identify this `Material` with, being useful for raycast hit results as those can contain a hit ID.
--- @field type Lime.Enum.MaterialType Sets the type of this `Material`, determing how the layers interact with themselves and the world.
--- @field fog boolean Enables fog for this `Material`.
--- @field lighting boolean Enables lighting for this `Material`.
--- @field backfaceCulling boolean Change backface culling behavior for this `Material`.
--- @field frontfaceCulling boolean Change frontface culling behavior for this `Material`.
--- @field quality Lime.Enum.Quality Sets the quality of this `Material` using `Lime.Enum.Quality` presets, where Low is unfiltered and High is smooth.
--- @field wireframe boolean Enables wireframe view for this `Material`.
--- @field zMethod Lime.Enum.ZOrderMethod Sets Z ordering method for this `Material` using `Lime.Enum.ZOrderMethod`.
--- @field opacity number Sets the opacity of this `Material` from 0.0 (invisible) to 1.0 (visible), affecting the transparency of objects with this `Material` applied. (NOTE: Will not affect solid types)
--- @field mipmaps boolean Enables the generation of mipmaps.
--- @field shine number Sets the shine for this `Material`, ranging from 0 (soft and wide shine) to 1 (harsh and small shine).
--- @field writeToDepth boolean Enables this `Material` writing to the depth buffer on render, where false is common for transparent objects.
--- @field ambientColor Vec4 Sets the ambient color for this `Material`, the base color.
--- @field diffuseColor Vec4 Sets the diffuse color for this `Material`, the light-affected base color.
--- @field specularColor Vec4 Sets the specular color for this `Material`, the shine color.
--- @field emissiveColor Vec4 Sets the emissive color for this `Material`, the color that is seen through shadows, lighting, and fog.
Material = {}

--- @overload fun(img: Texture): Material
--- @overload fun(other: Material): Material
--- @overload fun(quality: Lime.Enum.Quality): Material
--- @return Material
function Material.new() end

--- Loads a `Shader` into this `Material`.
--- @param shader Shader
function Material:loadShader(shader) end

--- Clears the `Shader` applied to this `Material`, if any.
function Material:clearShader() end

--- Loads a `Texture` into this `Material`.
--- @param texture Texture
--- @param layer number?
function Material:loadTexture(texture, layer) end

--- Clears the `Texture` in this `Material`.
--- @param layer number?
function Material:clearTexture(layer) end

--- Changes the method for `Texture` UV wrapping.
--- @param uMethod Lime.Enum.TextureWrapType
--- @param vMethod Lime.Enum.TextureWrapType
--- @param layer number?
function Material:setTextureWrapMethod(uMethod, vMethod, layer) end

--- Sets the scale of the mapping of a `Texture`.
--- @param scale Vec2
--- @param layer number?
function Material:setTextureScale(scale, layer) end

--- Sets the coordinate offset of a `Texture`.
--- @param scroll Vec2
--- @param layer number?
function Material:setTextureOffset(scroll, layer) end

--- @param layer number?
--- @return Vec2
function Material:getTextureOffset(layer) end

--- A scene object capable of displaying a mesh.
--- @class Mesh : Object3D
--- @field vertexColor Vec4 Sets the color of all vertexes in this `Mesh`. The `Material` of this `Mesh` must have type `VertexAlpha` to take effect.
--- @field collision boolean Allows response to raypicks and other simple collision methods. (NOTE: This flag does not affect this `Mesh` when wrapped by a physics object.)
--- @field frame number Controls the current frame of animation.
Mesh = {}

--- @overload fun(path: string): Mesh
--- @overload fun(buffer: MeshBuffer): Mesh
--- @return Mesh
function Mesh.new() end

--- Loads a 3D model into this `Mesh`.
--- @param path string|MeshBuffer
--- @return boolean
function Mesh:loadMesh(path) end

--- Loads a `Material` into this `Mesh`.
--- @param material Material
--- @param layer number?
--- @return boolean
function Mesh:loadMaterial(material, layer) end

--- @return number
function Mesh:getMaterialCount() end

--- Converges all child `Mesh` objects into this `Mesh`. Child `Mesh` objects are not destroyed in the process.
--- @return boolean
function Mesh:converge() end

--- @return number
function Mesh:getVertexCount() end

--- @return number
function Mesh:getFrameCount() end

--- Recalculates the bounding box of this `Mesh`. This is useful for potential loading errors, where this `Mesh` stops rendering even if it is within the active `Camera`'s view.
function Mesh:recalculateBoundingBox() end

--- Informs the graphics system of how this `Mesh` should be stored. By default, `Mesh` objects use Static. Use Dynamic (or more intensely, Stream) if the `Mesh` is updated frequently.
--- @param hint Lime.Enum.StorageHint
function Mesh:setStorageHint(hint) end

--- Writes this `Mesh` to `path` in .OBJ format.
--- @param path string
--- @return boolean
function Mesh:write(path) end

--- Clears the `MeshBuffer` from within this `Mesh`. This will not remove its `MeshBuffer` from memory.
function Mesh:clear() end

--- Purges this `Mesh`, effectively removing its mesh buffer from memory. Objects using the `MeshBuffer` of this `Mesh` will use an engine-defined `Mesh` instead, but it is recommended to remove references to this `Mesh` first.
function Mesh:purge() end

--- A container for vertices.
--- @class MeshBuffer
MeshBuffer = {}

--- @overload fun(arg: Mesh): MeshBuffer
--- @return MeshBuffer
function MeshBuffer.new() end

--- Recalculates the bounding box of this `MeshBuffer`. This should be called after modifying this `MeshBuffer`.
function MeshBuffer:recalculateBoundingBox() end

--- @return number
function MeshBuffer:getVertexCount() end

--- Pushes a face to this `MeshBuffer`.
--- @param pos1 Vec3
--- @param pos2 Vec3
--- @param pos3 Vec3
--- @param normal1 Vec3
--- @param normal2 Vec3
--- @param normal3 Vec3
--- @param uvw1 Vec3
--- @param uvw2 Vec3
--- @param uvw3 Vec3
--- @param color1 Vec4
--- @param color2 Vec4
--- @param color3 Vec4
function MeshBuffer:pushFace(pos1, pos2, pos3, normal1, normal2, normal3, uvw1, uvw2, uvw3, color1, color2, color3) end

--- @return number
function MeshBuffer:getReferenceCount() end

--- Clears this `MeshBuffer`. This will not remove itself from memory.
function MeshBuffer:clear() end

--- Purges this `MeshBuffer`, effectively removing it from memory. Objects using this `MeshBuffer` will use an engine-defined `MeshBuffer` instead, but it is recommended to remove references to this `MeshBuffer` first.
function MeshBuffer:purge() end

--- Loads the `MeshBuffer` stored within the provided `Mesh`.
--- @param mesh Mesh
--- @param layer number?
--- @return boolean
function MeshBuffer:loadMesh(mesh, layer) end

--- An object that holds gradient noise. Useful for random terrain, clouds, and more.
--- @class Noise
--- @field seed number Sets the seed of this `Noise`.
--- @field octaves number Sets the number of octaves in this `Noise`.
Noise = {}

--- @overload fun(seed: number): Noise
--- @overload fun(seed: number, octaves: number): Noise
--- @return Noise
function Noise.new() end

--- @param x number
--- @param y number?
--- @param z number?
--- @return number
function Noise:get(x, y, z) end

--- An object used to store data to send across a network.
--- @class Packet
--- @field position number The read position.
Packet = {}

--- @return Packet
function Packet.new() end

--- Writes an unsigned 8 bit number to this `Packet`.
--- @param v number
function Packet:writeUint8(v) end

--- Writes an unsigned 16 bit number to this `Packet`.
--- @param v number
function Packet:writeUint16(v) end

--- Writes an unsigned integer to this `Packet`.
--- @param v number
function Packet:writeUint32(v) end

--- Writes an integer to this `Packet`.
--- @param v number
function Packet:writeInt(v) end

--- Writes a float point number to this `Packet`.
--- @param v number
function Packet:writeFloat(v) end

--- Writes a string to this `Packet`.
--- @param v string
function Packet:writeString(v) end

--- Writes a file from `path` to this `Packet`. Returns true if the file was found and written successfully.
--- @param path string
--- @return boolean
function Packet:writeFile(path) end

--- @return number
function Packet:readUint8() end

--- @return number
function Packet:readUint16() end

--- @return number
function Packet:readUint32() end

--- @return number
function Packet:readInt() end

--- @return number
function Packet:readFloat() end

--- @return string
function Packet:readString() end

--- Saves a file to `path` from this `Packet`. Returns true on success.
--- @param path string
--- @return boolean
function Packet:readFile(path) end

--- Clears the data of this `Packet`.
function Packet:clear() end

function Packet:getSize() end

--- An object with various emitter types to emit particles.
--- @class ParticleSystem : Object3D
--- @field type Lime.Enum.EmitterType The emitter type.
--- @field global boolean Whether or not particles emitted stay parented to this `ParticleSystem`.
--- @field active boolean Whether or not this `ParticleSystem` is actively emitting particles.
--- @field particlesPerSecond Vec2 The particles per second emitted.
--- @field speed number The particle emit speed in units per second, where 0.001 is one unit per second.
--- @field maxAngle number The max angle variation for emitting particles. If set to 0 (360... etc.) then it will emit omnidirectionally.
--- @field scaleRange Vec2 The minimum and maximum range of scale for particles first being emitted.
--- @field lifeRange Vec2 The minimum and maximum range of lifetime for particles first being emitted.
--- @field boxSize Vec3 If the emitter type is **box**, this alters the size of the box.
--- @field radius number If the emitter type is **sphere**, **ring**, or **cylinder**, this alters the radius of the emitter.
--- @field ringThickness number If the emitter type is **ring**, this alters the ring thickness.
--- @field cylinderLength number If the emitter type is **cylinder**, this alters the length of the cylinder.
ParticleSystem = {}

--- @overload fun(type: Lime.Enum.EmitterType): ParticleSystem
--- @return ParticleSystem
function ParticleSystem.new() end

--- Sets the minimum color particles are influenced by on creation.
--- @param color Vec4
function ParticleSystem:setMinColor(color) end

--- Sets the maximum color particles are influenced by on creation.
--- @param color Vec4
function ParticleSystem:setMaxColor(color) end

--- Clears all active particles from this `ParticleSystem`.
function ParticleSystem:clear() end

--- Emit `amount` particles from this `ParticleSystem` once.
--- @param amount number?
function ParticleSystem:burst(amount) end

--- Loads a `Material` to use for new particles.
--- @param material Material
function ParticleSystem:loadMaterial(material) end

--- Adds an attraction affector to this `ParticleSystem`. This affector influences particle movement to attract to/detract from a point. For `affectAxis`, 0 = false and 1 = true.
--- @param pos Vec3
--- @param spd number
--- @param attract boolean
--- @param affectAxis Vec3
function ParticleSystem:addAttractionAffector(pos, spd, attract, affectAxis) end

--- Adds a fade out affector to this `ParticleSystem`. This affector influences particle color over `ms` milliseconds.
--- @param color Vec4
--- @param ms number
function ParticleSystem:addFadeOutAffector(color, ms) end

--- Adds a gravity affector to this `ParticleSystem`. This affector influences particle gravity to fully take over by `ms` milliseconds.
--- @param gravity Vec3
--- @param ms number
function ParticleSystem:addGravityAffector(gravity, ms) end

--- Adds a rotation affector to this `ParticleSystem`. This affector influences particle movement to rotate around a local `pos`.
--- @param rotSpd Vec3
--- @param pos Vec3
function ParticleSystem:addRotationAffector(rotSpd, pos) end

--- Adds a scalar affector to this `ParticleSystem`. This affector influences particle scale.
--- @param scalar number
function ParticleSystem:addScalarAffector(scalar) end

--- Clears all active affectors on this `ParticleSystem`.
function ParticleSystem:clearAffectors() end

--- Sets the emitter type to **point**. This emitter emits particles from a point.
function ParticleSystem:setEmitterPoint() end

--- Sets the emitter type to **box**. This emitter emits particles in a box.
--- @param boxSize Vec3
function ParticleSystem:setEmitterBox(boxSize) end

--- Sets the emitter type to **sphere**. This emitter emits particles in a sphere.
--- @param center Vec3
--- @param radius number
function ParticleSystem:setEmitterSphere(center, radius) end

--- Sets the emitter type to **ring**. This emitter emits particles in a ring.
--- @param center Vec3
--- @param radius number
--- @param thickness number
function ParticleSystem:setEmitterRing(center, radius, thickness) end

--- Sets the emitter type to **cylinder**. This emitter emits particles in a cylinder.
--- @param center Vec3
--- @param radius number
--- @param length number
--- @param outlineOnly boolean?
function ParticleSystem:setEmitterCylinder(center, radius, length, outlineOnly) end

--- A wrapper to `Mesh` objects that allows for them to react to physics. It can be created with a `Mesh` as its visual and collision shape, or with a custom collision shape independent of any 3D object.
--- @class RigidBody : PhysicsObject
--- @field position Vec3 The 3D position of this object in the scene.
--- @field gravity Vec3 The force of gravity applied to this object per physics step.
--- @field rotation Vec3 The 3D rotation of this object in the scene in degrees.
--- @field friction number Sets the friction coefficient.
--- @field anisotropicFriction Vec3 Sets per-axis friction scaling.
--- @field mass number Sets the mass and recalculates inertia.
--- @field sleeping boolean Whether or not this physics object is sleeping.
--- @field ghost boolean Sets whether or not other physics objects can pass through this object.
--- @field linearDamping number Sets the linear damping, reducing linear velocity over time.
--- @field angularDamping number Sets the angular damping, reducing rotational velocity over time.
--- @field elasticity number Sets the elasticity, controlling bounciness on collision.
--- @field sleepThreshold number Sets the velocity threshold this `RigidBody` must be below in order to trigger sleeping.
--- @field linearVelocity Vec3 Sets the linear velocity.
--- @field angularVelocity Vec3 Sets the angular velocity.
--- @field linearFactor Vec3 Sets the linear factor, controlling which axes linear motion is allowed on. (0 locks, 1 allows)
--- @field angularFactor Vec3 Sets the angular factor, controlling which axes rotation is allowed on. (0 locks, 1 allows)
RigidBody = {}

--- @overload fun(root: any, collision: Mesh): RigidBody
--- @param base Mesh
--- @return RigidBody
function RigidBody.new(base) end

--- Applies a preset to this `RigidBody` that alters its physical properties.
--- @param preset Lime.Enum.PhysicalPreset
function RigidBody:loadPreset(preset) end

--- Loads a new visual `Mesh` into this `RigidBody`.
--- @return boolean
function RigidBody:loadMesh() end

--- Clears all forces applied to this `RigidBody`.
function RigidBody:clearForces() end

--- @return Vec3
function RigidBody:getCenterOfMass() end

--- Rotates this `RigidBody` to look at a position in world space.
--- @param pos Vec3
function RigidBody:lookAt(pos) end

--- @return Vec3
function RigidBody:getForward() end

--- Applies a continous force to this `RigidBody`, where `pos` is relative to the center of this `RigidBody`.
--- @param force Vec3
--- @param pos Vec3
function RigidBody:applyForceRelative(force, pos) end

--- Applies an impulse to this `RigidBody`, where `pos` is relative to the center of this `RigidBody`.
--- @param impulse Vec3
--- @param pos Vec3
function RigidBody:applyImpulseRelative(impulse, pos) end

--- Applies a continous force to this `RigidBody`, where `pos` is in world space.
--- @param force number
--- @param pos Vec3
function RigidBody:applyForce(force, pos) end

--- Applies an impulse to this `RigidBody`, where `pos` is in world space.
--- @param force number
--- @param pos Vec3
function RigidBody:applyImpulse(force, pos) end

--- Applies a torque to this `RigidBody`, where `pos` is in world space.
--- @param force Vec3
--- @param impulse boolean
function RigidBody:applyTorque(force, impulse) end

--- A special material that can produce custom effects. Apply `Shader` objects to `Material` objects or to the screen with `Lime.Scene.setPostProcessingShader`. By default, all `Shader` objects set internal parameters `uWorldViewProj` to the current world-view projection matrix, `uWorld` to just the current world matrix, and `uTime` to the elapsed time in seconds. (decimal)
--- @class Shader
Shader = {}

--- @overload fun(hlslShaderPath: string, type: Lime.Enum.MaterialType?): Shader
--- @param vertexShaderPath string
--- @param pixelShaderPath string
--- @param type Lime.Enum.MaterialType?
--- @return Shader
function Shader.new(vertexShaderPath, pixelShaderPath, type) end

--- Sets a uniform shader parameter within this `Shader`.
--- @param name string
--- @param value number|Vec2|Vec3|Vec4
function Shader:setParameter(name, value) end

--- @return string
function Shader:getVSPath() end

--- @return string
function Shader:getPSPath() end

--- @return number
function Shader:getType() end

--- A dome rendered behind all scene objects, like a sky.
--- @class Skydome : Object3D
Skydome = {}

--- @overload fun(material: Material): Skydome
--- @return Skydome
function Skydome.new() end

--- Loads a new Material into this Skydome
--- @param material Material
function Skydome:loadMaterial(material) end

--- A source of sound, whether that be for sound effects or music.
--- @class Sound
--- @field paused boolean Whether or not this `Sound` is paused.
--- @field looping boolean Whether or not this `Sound` loops on playback.
--- @field volume number The volume of this `Sound`.
--- @field speed number The playback speed of this `Sound`.
--- @field pan number The pan of this `Sound`, where -1.0 is left and 1.0 is right.
--- @field minimumDistance number Sets the minimum listening distance for this `Sound`. Only applicable if this object is played in 3D.
--- @field maximumDistance number Sets the maximum listening distance for this `Sound`. Only applicable if this object is played in 3D.
--- @field playbackPosition number The current playback position of this `Sound`.
--- @field velocity Vec3 The velocity of this `Sound`. Only applicable if this object is played in 3D.
--- @field position Vec3 The position of this `Sound` in the scene. Only applicable if this `Sound` is played in 3D.
--- @field effects boolean Whether or not sound effects are enabled on playback. This flag must first be enabled to apply effects, as it is false by default. Sound effects are more resource-intensive.
--- @field debug boolean Show debug information about this object in the scene.
Sound = {}

--- @overload fun(arg: Sound): Sound
--- @overload fun(path: string, type: Lime.Enum.SoundType?): Sound
--- @return Sound
function Sound.new() end

--- Play this `Sound`.
--- @param is3D boolean?
--- @return boolean
function Sound:play(is3D) end

--- Stop this `Sound`.
function Sound:stop() end

--- @return boolean
function Sound:isPlaying() end

--- @return number
function Sound:getLength() end

--- @return string
function Sound:getPath() end

--- Parents this `Sound` to a 3D object. (NOTE: This `Sound` must be playing in 3D)
--- @param parent any
--- @return boolean
function Sound:parentTo(parent) end

--- @return boolean
function Sound:hasParent() end

--- Loads a new sound into this `Sound`. (WARNING: Unused sounds should be purged to free up unused memory)
--- @param path string
--- @param type Lime.Enum.SoundType?
--- @return boolean
function Sound:load(path, type) end

--- Destroys this `Sound`, which stops itself from playing in the scene as well as detaching from a parent 3D object. To free this sound from memory, see `SoundSource:purge`.
function Sound:destroy() end

--- Purges this `Sound`, effectively removing it from memory. If other `Sound` objects use this sound, there may be issues.
function Sound:purge() end

--- Clears all effects applied to this `Sound`. Stopping or destroying this `Sound` will clear its effects.
function Sound:clearEffects() end

--- Enables distortion on this `Sound`. Only applicable if this `Sound` is playing. This effect messes with the sound's frequency and other attributes to produce an odd result. Params
--- @param gain number
--- @param edge number
--- @return boolean
function Sound:addDistortionEffect(gain, edge) end

--- Enables echoing on this `Sound`. Only applicable if this `Sound` is playing. This effect repeats the sound with decay over time. Params
--- @param wetDry number
--- @param feedback number
--- @param delayMs number
--- @return boolean
function Sound:addEchoEffect(wetDry, feedback, delayMs) end

--- Enables reverb on this `Sound`. Only applicable if this `Sound` is playing. This effect mixes the sound to bounce off surfaces in a room or a cave. Params
--- @param inputGain number
--- @param mix number
--- @param timeMs number
--- @param freqRatio number
--- @return boolean
function Sound:addReverbEffect(inputGain, mix, timeMs, freqRatio) end

--- Enables compression on this `Sound`. Only applicable if this `Sound` is playing. This effect reduces the dynamic range of the sound's waveform. Params
--- @param threshold number
--- @param ratio number
--- @return boolean
function Sound:addCompressionEffect(threshold, ratio) end

--- Enables parametric equilization on this `Sound`. Only applicable if this `Sound` is playing. This effect amplifies or attenuates signals at a given frequency. Params
--- @param threshold number
--- @param ratio number
--- @return boolean
function Sound:addParamEqEffect(threshold, ratio) end

--- A basic 2D object to display text. Text objects support colors and basic styling. Use tags `<#HEX>` for color, `<s>` for strike, `<d>` for drop shadow, `<u>` for underline, `<b>` for bold, and `<r>` to reset styles. Example: `<#6ABE30>This is green! <b>Now, it's green and bold! <r>Now, it's back to normal.`
--- @class Text2D : Object2D
--- @field text string The text content of this object.
--- @field wordWrap boolean Determines if the text wraps when touching the border of its text box.
--- @field opacity number The opacity of the text, from 0 to 255. For individual characters being not fully opaque, use color tags with an alpha value.
Text2D = {}

--- @overload fun(text: string): Text2D
--- @overload fun(pos: Vec2, size: Vec2): Text2D
--- @return Text2D
function Text2D.new() end

--- Sets the font to use for this object. Fonts must first be loaded. See `Lime.GUI.loadXML` and `Lime.GUI.loadTTF`.
--- @param name string
--- @return boolean
function Text2D:setFont(name) end

--- Enables word wrap.
--- @param wrap boolean
function Text2D:setWordWrap(wrap) end

--- Sets the text's alignment within its bounding box.
--- @param x Lime.Enum.TextAlign
--- @param y Lime.Enum.TextAlign?
function Text2D:setAlignment(x, y) end

--- A basic 3D object to display text. This object is essentially a `Text2D` tied to a 3D scene node. Text objects support colors and basic styling. Use tags `<#HEX>` for color, `<s>` for strike, `<d>` for drop shadow, `<u>` for underline, `<b>` for bold, and `<r>` to reset styles. Example: `<#6ABE30>This is green! <b>Now, it's green and bold! <r>Now, it's back to normal.`
--- @class Text3D : Object3D
--- @field text string The text content of this object.
--- @field opacity number The opacity of the text, from 0 to 255. For individual characters being not fully opaque, use color tags with an alpha value.
--- @field wordWrap boolean Determines if the text wraps when touching the border of its text box.
--- @field size Vec2 The size of the 2D text box.
Text3D = {}

--- @overload fun(text: string): Text3D
--- @return Text3D
function Text3D.new() end

--- Sets the font to use for this object. Fonts must first be loaded. See `Lime.GUI.loadXML` and `Lime.GUI.loadTTF`.
--- @param name string
--- @return boolean
function Text3D:setFont(name) end

--- Enables word wrap.
--- @param wrap boolean
function Text3D:setWordWrap(wrap) end

--- Sets the text's alignment within its bounding box.
--- @param x Lime.Enum.TextAlign
--- @param y Lime.Enum.TextAlign?
function Text3D:setAlignment(x, y) end

--- A texture that is the foundation for all images for 2D and 3D objects.
--- @class Texture
Texture = {}

--- @overload fun(size: Vec2, name: string?): Texture
--- @overload fun(path: string): Texture
--- @return Texture
function Texture.new() end

--- @return string
function Texture:getPath() end

--- @return Vec2
function Texture:getSize() end

--- Writes this `Texture` to `path`.
--- @param path string
--- @return boolean
function Texture:write(path) end

--- Crops the `Texture` to the dimensions provided. Be mindful as cropping creates a new `Texture` in the renderer.
--- @param topLeft Vec2
--- @param bottomRight Vec2
--- @return boolean
function Texture:crop(topLeft, bottomRight) end

--- Appends another `Texture` onto this `Texture`.
--- @param toAppend Texture
--- @param pos Vec2
--- @return boolean
function Texture:append(toAppend, pos) end

--- @param pos Vec2
--- @return Vec4
function Texture:getColor(pos) end

--- Sets a pixel's `color`, or fill an area of pixels with `color`.
--- @overload fun(pos: Vec2, color: Vec4): boolean
--- @param topLeft Vec2
--- @param bottomRight Vec2
--- @param fillColor Vec4
--- @return boolean
function Texture:setColor(topLeft, bottomRight, fillColor) end

--- Clears this `Texture`. Params
--- @param color Vec4
function Texture:clear(color) end

--- Removes the color `keyColor` from anywhere in this `Texture`.
--- @param keyColor Vec4
--- @return boolean
function Texture:keyColor(keyColor) end

--- @return number
function Texture:getReferenceCount() end

--- Renders the scene to this `Texture`. Returns the name of this `Texture`.
--- @param size Vec2
--- @param viewpoint Camera?
--- @return string
function Texture:renderToTexture(size, viewpoint) end

--- Purges this `Texture`, effectively removing it from memory. Objects using this `Texture` will use an engine-defined `Texture` instead, but it is recommended to remove references to this `Texture` first.
function Texture:purge() end

--- A two-dimensional vector object.
--- @class Vec2
--- @operator add(Vec2): Vec2
--- @operator sub(Vec2): Vec2
--- @operator mul(number): Vec2
--- @operator div(number): Vec2
--- @operator mul(Vec2): Vec2
--- @operator div(Vec2): Vec2
--- @operator eq(Vec2): boolean
--- @field x number
--- @field y number
Vec2 = {}

--- @overload fun(x: number, y: number): Vec2
--- @overload fun(all: number): Vec2
--- @overload fun(other: Vec2): Vec2
--- @return Vec2
function Vec2.new() end

--- Sets the components of this vector to the components of `other`. This is useful for copying as a typical assignment may lead to unexpected results.
--- @overload fun(other: Vec3): nil
--- @param x number
--- @param y number
function Vec2:set(x, y) end

--- @return number
function Vec2:length() end

--- @return number
function Vec2:lengthSqr() end

--- @param other Vec2
--- @return number
function Vec2:distance(other) end

--- @param other Vec2
--- @return number
function Vec2:distanceSqr(other) end

--- @param epsilon number?
--- @return boolean
function Vec2:isNearlyZero(epsilon) end

--- Measures alignment of two vectors; >0 - same direction, 0 - perpendicular, <0 - opposite.
--- @param other Vec2
--- @return number
function Vec2:dot(other) end

--- Measures signed scalar area, indicating clockwise versus counter-clockwise orientation.
--- @param other Vec2
--- @return number
function Vec2:cross(other) end

--- Measures the angle between vectors in degrees
--- @param other Vec2
--- @return number
function Vec2:angle(other) end

--- @return Vec2
function Vec2:normalize() end

--- @param min number
--- @param max number
--- @return Vec2
function Vec2:normalizeRng(min, max) end

--- @param min Vec2
--- @param max Vec2
--- @return Vec2
function Vec2:clamp(min, max) end

--- A three-dimensional vector object.
--- @class Vec3
--- @operator add(Vec3): Vec3
--- @operator sub(Vec3): Vec3
--- @operator mul(number): Vec3
--- @operator div(number): Vec3
--- @operator mul(Vec3): Vec3
--- @operator div(Vec3): Vec3
--- @operator eq(Vec3): boolean
--- @field x number
--- @field y number
--- @field z number
Vec3 = {}

--- @overload fun(x: number, y: number, z: number): Vec3
--- @overload fun(all: number): Vec3
--- @overload fun(other: Vec3): Vec3
--- @return Vec3
function Vec3.new() end

--- Sets the components of this vector to the components of `other`. This is useful for copying as a typical assignment may lead to unexpected results.
--- @overload fun(other: Vec3): nil
--- @param x number
--- @param y number
--- @param z number
function Vec3:set(x, y, z) end

--- @return number
function Vec3:length() end

--- @return number
function Vec3:lengthSqr() end

--- @param other Vec3
--- @return number
function Vec3:distance(other) end

--- @param other Vec3
--- @return number
function Vec3:distanceSqr(other) end

--- @param epsilon number?
--- @return boolean
function Vec3:isNearlyZero(epsilon) end

--- Measures alignment of two vectors; >0 - same direction, 0 - perpendicular, <0 - opposite.
--- @param other Vec3
--- @return number
function Vec3:dot(other) end

--- Measures signed scalar area, indicating clockwise versus counter-clockwise orientation.
--- @param other Vec3
--- @return Vec3
function Vec3:cross(other) end

--- Measures the angle between vectors in degrees
--- @param other Vec3
--- @return number
function Vec3:angle(other) end

--- @return Vec3
function Vec3:normalize() end

--- @param min number
--- @param max number
--- @return Vec3
function Vec3:normalizeRng(min, max) end

--- @param min Vec3
--- @param max Vec3
--- @return Vec3
function Vec3:clamp(min, max) end

--- A four-dimensional vector object.
--- @class Vec4
--- @operator add(Vec4): Vec4
--- @operator sub(Vec4): Vec4
--- @operator mul(number): Vec4
--- @operator div(number): Vec4
--- @operator mul(Vec4): Vec4
--- @operator div(Vec4): Vec4
--- @operator eq(Vec4): boolean
--- @field x number
--- @field y number
--- @field z number
--- @field w number
Vec4 = {}

--- @overload fun(x: number, y: number, z: number, w: number): Vec4
--- @overload fun(all: number): Vec4
--- @overload fun(HEX: string): Vec4
--- @overload fun(other: Vec4): Vec4
--- @return Vec4
function Vec4.new() end

--- Sets the components of this vector to the components of `other`. This is useful for copying as a typical assignment may lead to unexpected results.
--- @overload fun(other: Vec4): nil
--- @param x number
--- @param y number
--- @param z number
--- @param w number
function Vec4:set(x, y, z, w) end

--- @return string
function Vec4:getHEX() end

--- An object that stores collision data between two physics objects.
--- @class CollisionResult
--- @field depth number Penetration depth
--- @field posA Vec3 Contact point on physics object A
--- @field posB Vec3 Contact point on physics object B
--- @field normal Vec3 Collision normal from physics object A to B
--- @field linearVelocityA Vec3 Linear velocity of physics object A
--- @field linearVelocityB Vec3 Linear velocity of physics object B
--- @field angularVelocityA Vec3 Angular velocity of physics object A
--- @field angularVelocityB Vec3 Angular velocity of physics object B
--- @field velocityAtPointA Vec3 Actual velocity at physics object A contact point
--- @field velocityAtPointB Vec3 Actual velocity at physics object B contact point
--- @field relativeVelocity Vec3 velocityAtPointB - velocityAtPointA
--- @field impactSpeed number Impact speed
CollisionResult = {}

--- An object that stores raycast hit data.
--- @class HitResult
--- @field startPos Vec3 The starting position of this raycast.
--- @field endPos Vec3 The ending position of this raycast. If an object was hit, this will be the hit position.
--- @field normal Vec3 The normal vector of the hit triangle, if any.
--- @field objectID number If hit, this will be the hit object's ID. Else, 0.
--- @field materialID number If hit, this will be the hit material's ID. Else, 0.
--- @field hit boolean True if the raycast hit a collidable object.
HitResult = {}
