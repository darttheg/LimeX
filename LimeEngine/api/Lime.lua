---@class Lime
---@field onInit Event @Event called by Lime prior to initializing the window.
---@field onStart Event @Event called by Lime following window creation and rendering services are available.
---@field onUpdate Event @Event called by Lime every rendering frame. This Event is run with a number `dt` argument.
---@field onClose Event @Event called by Lime once the application closes in any way.
Lime = Lime or {}

---@class Lime.Input
---@field onKeyPressed Event @Event called by Lime when a key is pressed. This Event is run with a Lime.Enum.Key `key` argument.
---@field onKeyReleased Event @Event called by Lime when a key is released. This Event is run with a Lime.Enum.Key `key` argument.
---@field onMouseButtonPressed Event @Event called by Lime when a mouse button is pressed. This Event is run with a Lime.Enum.Mouse `button` argument.
---@field onMouseButtonReleased Event @Event called by Lime when a mouse button is released. This Event is run with a Lime.Enum.Mouse `button` argument.
---@field onMouseMoved Event @Event called by Lime when a mouse is moved. This Event is run with a Vec2 `delta` argument.
---@field onMouseScroll Event @Event called by Lime when the mouse scroll wheel is moved. This Event is run with a number `delta` argument.
---@field onControllerConnected Event @Event called by Lime when a controller is connected. This Event is run with a number `id` argument.
---@field onControllerDisconnected Event @Event called by Lime when a controller is disconnected. This Event is run with a number `id` argument.
---@field onControllerButtonPressed Event @Event called by Lime when a controller button is pressed. This Event is run with number `id`, Lime.Enum.Controller `button` arguments.
---@field onControllerButtonReleased Event @Event called by Lime when a controller button is released. This Event is run with number `id`, Lime.Enum.Controller `button` arguments.
Lime.Input = Lime.Input or {}

---@class Lime.Scene
Lime.Scene = Lime.Scene or {}

---@class Lime.Window
Lime.Window = Lime.Window or {}

---@class Camera
---@field position Vec3 @The 3D position of this object in the scene.
---@field rotation Vec3 @The 3D rotation of this object in the scene in degrees.
---@field up Vec3 @The up vector of this `Camera`.
---@field viewPlanes Vec2 @The near and far clipping planes of this `Camera`.
---@field fieldOfView number @The field of view of this `Camera` in degrees.
---@field aspectRatio number @The aspect ratio of this `Camera`.
---@field orthogonal boolean @Whether or not this `Camera` renders orthographically or not. (NOTE: If this is true, `aspectRatio` modifies the zoom factor instead.)
---@field scale Vec3 @The 3D scale of this object in the scene.
---@field visible boolean @Determines the visibility of this object and its children.
---@field id number @The identifier for this object to be used in raycasts and object selection.
Camera = Camera or {}
--- A viewpoint in the 3D world.
---@overload fun(pos:Vec3): Camera
---@overload fun(pos:Vec3, rot:Vec3): Camera
---@return Camera
function Camera.new() end

---@class Event
Event = Event or {}
--- A container of functions that will run in sequence when called upon.
---@return Event
function Event.new() end

---@class Hook
Hook = Hook or {}
--- A handle from Event hooking.
---@return Hook
function Hook.new() end

---@class Image
Image = Image or {}
--- An image that is the foundation for all texturing, for 2D and 3D objects.
---@overload fun(w:number, h:number, name:string?): Image
---@overload fun(path:string): Image
---@return Image
function Image.new() end

---@class Material
---@field ID number @An ID to identify this `Material` with, being useful for raycast hit results as those can contain a hit ID.
---@field type Lime.Enum.MaterialType @Sets the type of this `Material`, determing how the layers interact with themselves and the world.
---@field fog boolean @Enables fog for this `Material`.
---@field lighting boolean @Enables lighting for this `Material`.
---@field backfaceCulling boolean @Change backface culling behavior for this `Material`.
---@field frontfaceCulling boolean @Change frontface culling behavior for this `Material`.
---@field quality Lime.Enum.MaterialQuality @Sets the quality of this Material using `Lime.Enum.MaterialQuality` presets, where Low is unfiltered and High is smooth.
---@field wireframe boolean @Enables wireframe view for this `Material`.
---@field zMethod Lime.Enum.ZOrderMethod @Sets Z ordering method for this `Material` using `Lime.Enum.ZOrderMethod`.
---@field opacity number @Sets the opacity of this Material from 0.0 (invisible) to 1.0 (visible), affecting the transparency of objects with this `Material` applied. (NOTE: Will not affect solid types)
---@field mipmaps boolean @Enables the generation of mipmaps.
---@field shine number @Sets the shine for this `Material`, ranging from 0 (soft and wide shine) to 1 (harsh and small shine).
---@field writeToDepth boolean @Enables this `Material` writing to the depth buffer on render, where false is common for transparent objects.
---@field ambientColor Vec4 @Sets the ambient color for this `Material`, the base color.
---@field diffuseColor Vec4 @Sets the diffuse color for this `Material`, the light-affected base color.
---@field specularColor Vec4 @Sets the specular color for this `Material`, the shine color.
---@field emissiveColor Vec4 @Sets the emissive color for this `Material`, the color that is seen through shadows, lighting, and fog.
Material = Material or {}
--- An object used to hold material parameters for 3D objects. A Material has at most two layers, with `Lime.Enum.MaterialType` allowing for different combinations of said layers.
---@overload fun(img:Image): Material
---@overload fun(other:Material): Material
---@overload fun(quality:Lime.Enum.MaterialQuality): Material
---@return Material
function Material.new() end

---@class Skydome
---@field position Vec3 @The 3D position of this object in the scene.
---@field rotation Vec3 @The 3D rotation of this object in the scene in degrees.
---@field scale Vec3 @The 3D scale of this object in the scene.
---@field visible boolean @Determines the visibility of this object and its children.
---@field id number @The identifier for this object to be used in raycasts and object selection.
Skydome = Skydome or {}
--- A dome rendered behind all scene objects, like a sky.
---@overload fun(material:Material): Skydome
---@return Skydome
function Skydome.new() end

---@class Vec2
---@field x number
---@field y number
Vec2 = Vec2 or {}
--- A two-dimensional vector object.
---@overload fun(x:number, y:number): Vec2
---@overload fun(all:number): Vec2
---@return Vec2
function Vec2.new() end

---@class Vec3
---@field x number
---@field y number
---@field z number
Vec3 = Vec3 or {}
--- A three-dimensional vector object.
---@overload fun(x:number, y:number, z:number): Vec3
---@overload fun(all:number): Vec3
---@return Vec3
function Vec3.new() end

---@class Vec4
---@field x number
---@field y number
---@field z number
---@field w number
Vec4 = Vec4 or {}
--- A four-dimensional vector object.
---@overload fun(x:number, y:number, z:number, w:number): Vec4
---@overload fun(all:number): Vec4
---@return Vec4
function Vec4.new() end

--- Returns the forward vector.
---@return Vec3
function Camera:getForward() end

--- Returns the left vector.
---@return Vec3
function Camera:getLeft() end

--- Parents this object to another 3D object.
---@param child any
---@return void
function Camera:parentTo(child) end

--- When rendering, this `Camera` will take over as the active rendering viewpoint.
---@return void
function Camera:setActive() end

--- Clears all functions hooked to this Event.
function Event:clear() end

--- Hook a function to this Event.
---@param Function function
---@return Hook
function Event:hook(Function) end

--- Returns the number of hooked functions on this Event.
---@return number
function Event:length() end

--- Run this Event.
---@param ... any
function Event:run(...) end

--- Returns true if this hook is still hooked to an Event.
---@return boolean
function Hook:isHooked() end

--- Unhook a function to this Event.
function Hook:unhook() end

--- Appends another `Image` onto this `Image`.
---@param toAppend Image
---@param pos Vec2
---@return void
function Image:append(toAppend, pos) end

--- Crops the `Image` to the dimensions provided. Be mindful as cropping creates a new `Image` in the renderer.
---@param topLeft Vec2
---@param bottomRight Vec2
---@return void
function Image:crop(topLeft, bottomRight) end

--- Returns the color of the pixel at `pos` in this `Image`.
---@param pos Vec2
---@return Vec4
function Image:getColor(pos) end

--- Returns the path of this `Image`.
---@return string
function Image:getPath() end

--- Returns the dimensions of this `Image`.
---@return Vec2
function Image:getSize() end

--- Removes the color `keyColor` from anywhere in this `Image`.
---@param keyColor Vec4
---@return void
function Image:keyColor(keyColor) end

--- Replaces the pixel at `pos` with a pixel of color `color`.
---@param pos Vec2
---@param color Vec4
---@return void
function Image:setColor(pos, color) end

--- Writes the `Image` to path `path`.
---@param path string
---@return void
function Image:write(path) end

--- Closes the Lime application.
function Lime.close() end

--- Returns the elapsed time the application has been running in milliseconds.
---@return number
function Lime.getElapsedTime() end

--- Returns the frame rate in frames per second.
---@return number
function Lime.getFrameRate() end

--- Returns the application's memory usage in megabytes.
---@return number
function Lime.getMemoryUsage() end

--- Returns true if vertical syncronization is on.
---@return boolean
function Lime.getVSync() end

--- Returns the Lime version running.
---@return string
function Lime.getVersion() end

--- Prints a message to console.
---@param msg any
---@param color Lime.Enum.PrintColor?
---@return void
function Lime.log(msg, color) end

--- Sets debug console configuration. If `enable` is true, the debug console will appear alongside the application. If `writeOutput` is true, the console's output will be written to a output.log file in the application directory.
---@param enable boolean
---@param writeOutput boolean?
---@return void
function Lime.setDebugConfig(enable, writeOutput) end

--- If set to true, Lime will close on any error. A pop-up will be disclosed prior with error details.
---@param doEnd boolean
function Lime.setEndOnError(doEnd) end

--- Sets the target frame rate in frames per second.
---@param target number
---@return void
function Lime.setFrameRate(target) end

--- IMPORTANT: This function should always be run prior to window creation (pre-`Lime.onUpdate` Event) as only here can the driver type be changed. This function sets initial parameters for the Lime application.
---@param driver Lime.Enum.DriverType
---@param windowSize Vec2?
---@param renderSize Vec2?
---@return boolean
function Lime.setInitConfig(driver, windowSize, renderSize) end

--- If true, Lime will not render the scene at the end of each `Lime.onUpdate` cycle. Instead, use `Lime.Scene.render` for 3D scene rendering and `Lime.GUI.render` for GUI rendering within the `Lime.onUpdate` Event.
---@param isManual boolean
---@return void
function Lime.setManualRendering(isManual) end

--- Sets vertical syncronization, matching the frame rate to the primary monitor's refresh rate.
---@param vSyncOn boolean
---@return void
function Lime.setVSync(vSyncOn) end

--- Returns the change in mouse position within the last frame.
---@return Vec2
function Lime.Input.getMouseDelta() end

--- Returns the mouse's position.
---@return Vec2
function Lime.Input.getMousePosition() end

--- Returns true if Lime.Enum.Key `key` is currently pressed.
---@param key Lime.Enum.Key
---@return boolean
function Lime.Input.isKeyDown(key) end

--- Returns true if Lime.Enum.Mouse `button` is currently pressed.
---@param button Lime.Enum.Mouse
---@return boolean
function Lime.Input.isMouseButtonDown(button) end

--- Sets the mouse's position.
---@param pos Vec2
---@return void
function Lime.Input.setMousePosition(pos) end

--- Set the visibility of the mouse cursor.
---@param visible boolean
---@return void
function Lime.Input.setMouseVisible(visible) end

--- Returns an `Image` of a lime and white checkerboard pattern, 2x2. Useful for missing Images and the like.
---@return Image
function Lime.Scene.getErrorImage() end

--- Returns the amount of 3D objects in the scene.
---@return number
function Lime.Scene.getObjectCount() end

--- Renders the scene using the active `Camera` and current rendering parameters. Clearing the back buffer will replace previously rendered items with the background color. Clearing the Z buffer ensures the previous depth pass is not used. (NOTE: Manual rendering must be on, otherwise this function has no effect. See `Lime.setManualRendering`.)
---@param clearBackBuffer boolean?
---@param clearZBuffer boolean?
---@return void
function Lime.Scene.render(clearBackBuffer, clearZBuffer) end

--- Sets the ambient color of the scene to `rgba`.
---@param rgba Vec4
---@return void
function Lime.Scene.setAmbientColor(rgba) end

--- Sets the background color of the scene to `rgba`. This color is generally only visible when there is no `Skydome`.
---@param rgba Vec4
---@return void
function Lime.Scene.setBackgroundColor(rgba) end

--- Sets the color of the scene's fog to `rgba`.
---@param rgba Vec4
---@return void
function Lime.Scene.setFogColor(rgba) end

--- Sets where the scene's fog starts and ends.
---@param planes Vec2
---@return void
function Lime.Scene.setFogPlanes(planes) end

--- Sets the default `Image` creation quality using `Lime.Enum.ImageCreationQuality`, where Low is optimized for speed and High is optimized for quality.
---@return void
function Lime.Scene.setImageCreationQuality() end

--- Sets the light management type using `Lime.Enum.LightManagementType`.
---@param type Lime.Enum.LightManagementType
---@return void
function Lime.Scene.setLightManagementType(type) end

--- If set to true, the rendering resolution will be that of the window resolution, updating on any window size changes. If false, the rendering resolution will remain the same size but upscale without anti-aliasing to the window resolution.
---@param doRescale boolean
---@return void
function Lime.Scene.setRescaleRenderToWindowSize(doRescale) end

--- Sets the color of shadows in the scene to `rgba`.
---@param rgba Vec4
---@return void
function Lime.Scene.setShadowColor(rgba) end

--- Sets the render size to `size`. If the render size is set to rescale to the window size then this change will not take effect. See `Lime.Scene.setRescaleRenderToWindowSize`.
---@param size Vec2
---@return void
function Lime.Scene.setSize(size) end

--- Returns the size of the monitor the window is running on.
---@return Vec2
function Lime.Window.getMonitorSize() end

--- Returns the window's position.
---@return Vec2
function Lime.Window.getPosition() end

--- Returns the size of the window.
---@return Vec2
function Lime.Window.getSize() end

--- Returns true if the window is focused.
---@return boolean
function Lime.Window.isFocused() end

--- Toggles fullscreen mode.
---@param fullscreen boolean
---@return void
function Lime.Window.setFullscreen(fullscreen) end

--- Locks the aspect ratio of the raw window size regardless of resizing/maximizing.
---@param locked boolean
---@return void
function Lime.Window.setLockAspectRatio(locked) end

--- Sets the window's position to `pos`.
---@param pos Vec2
---@return void
function Lime.Window.setPosition(pos) end

--- Allows the window to be resizable or locked to its intended size.
---@param allow boolean
---@return void
function Lime.Window.setResizable(allow) end

--- Sets the window's size to `size`.
---@param size Vec2
---@return void
function Lime.Window.setSize(size) end

--- Sets the window's title to `title`.
---@param title string
---@return void
function Lime.Window.setTitle(title) end

--- Clears the `Image` in this `Material`.
---@param layer number?
---@return void
function Material:clearImage(layer) end

--- Loads an `Image` into this `Material`.
---@overload fun(layer:number, img:Image): void
---@param img Image
---@return void
function Material:loadImage(img) end

--- Sets the scale of the mapping of an `Image`.
---@overload fun(layer:number, scale:Vec2): void
---@param scale Vec2
---@return void
function Material:setImageScale(scale) end

--- Changes the method for `Image` UV wrapping.
---@overload fun(layer:number, uMethod:Lime.Enum.ImageWrapType, vMethod:Lime.Enum.ImageWrapType): void
---@param uMethod Lime.Enum.ImageWrapType
---@param vMethod Lime.Enum.ImageWrapType
---@return void
function Material:setImageWrapMethod(uMethod, vMethod) end

--- Loads a new Material into this Skydome
---@param material Material
---@return void
function Skydome:loadMaterial(material) end

--- Parents this object to another 3D object.
---@param child any
---@return void
function Skydome:parentTo(child) end

--- Measures the angle between vectors in degrees
---@param other Vec2
---@return number
function Vec2:angle(other) end

--- Returns a clamped vector to vectors min and max.
---@param min Vec2
---@param max Vec2
---@return Vec2
function Vec2:clamp(min, max) end

--- Measures signed scalar area, indicating clockwise versus counter-clockwise orientation.
---@param other Vec2
---@return number
function Vec2:cross(other) end

--- Returns the distance between two vectors.
---@param other Vec2
---@return number
function Vec2:distance(other) end

--- Returns the distance between two vectors squared.
---@param other Vec2
---@return number
function Vec2:distanceSqr(other) end

--- Measures alignment of two vectors; >0 - same direction, 0 - perpendicular, <0 - opposite.
---@param other Vec2
---@return number
function Vec2:dot(other) end

--- Returns true if the vector is effectively zero.
---@param epsilon number?
---@return boolean
function Vec2:isNearlyZero(epsilon) end

--- Returns the length of the vector.
---@return number
function Vec2:length() end

--- Returns the length of the vector save the square root operation.
---@return number
function Vec2:lengthSqr() end

--- Returns a normalized unit vector.
---@return Vec2
function Vec2:normalize() end

--- Returns a normalied vector scaled to clamp between numbers min and max.
---@param min number
---@param max number
---@return Vec2
function Vec2:normalizeRng(min, max) end

--- Measures the angle between vectors in degrees
---@param other Vec3
---@return number
function Vec3:angle(other) end

--- Returns a clamped vector to vectors min and max.
---@param min Vec3
---@param max Vec3
---@return Vec3
function Vec3:clamp(min, max) end

--- Measures signed scalar area, indicating clockwise versus counter-clockwise orientation.
---@param other Vec3
---@return Vec3
function Vec3:cross(other) end

--- Returns the distance between two vectors.
---@param other Vec3
---@return number
function Vec3:distance(other) end

--- Returns the distance between two vectors squared.
---@param other Vec3
---@return number
function Vec3:distanceSqr(other) end

--- Measures alignment of two vectors; >0 - same direction, 0 - perpendicular, <0 - opposite.
---@param other Vec3
---@return number
function Vec3:dot(other) end

--- Returns true if the vector is effectively zero.
---@param epsilon number?
---@return boolean
function Vec3:isNearlyZero(epsilon) end

--- Returns the length of the vector.
---@return number
function Vec3:length() end

--- Returns the length of the vector save the square root operation.
---@return number
function Vec3:lengthSqr() end

--- Returns a normalized unit vector.
---@return Vec3
function Vec3:normalize() end

--- Returns a normalied vector scaled to clamp between numbers min and max.
---@param min number
---@param max number
---@return Vec3
function Vec3:normalizeRng(min, max) end
