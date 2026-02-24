---@class Lime
---@field onInit Event @Event called by Lime prior to initializing the window.
---@field onStart Event @Event called by Lime following window creation and rendering services are available.
---@field onUpdate Event @Event called by Lime every rendering frame. This Event is run with a number `dt` argument.
---@field onClose Event @Event called by Lime once the application closes in any way.
Lime = Lime or {}

---@class Lime.GUI
Lime.GUI = Lime.GUI or {}

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
---@field onResize Event @Event called by Lime once the window is resized in any way.
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

---@class Image2D
---@field position Vec2 @The 2D position of this object on the screen.
---@field size Vec2 @The 2D size of this object.
---@field visible boolean @Determines the visibility of this object and its children.
---@field border boolean @Displays a border outlining this object's bounding box on the screen.
---@field backgroundColor Vec4 @The RGBA background color of this object.
---@field enabled boolean @Determines if this object is hoverable and clickable.
---@field onHovered Event @Event called by Lime when this object is hovered.
---@field onPressed Event @Event called by Lime when this object is pressed.
Image2D = Image2D or {}
--- A basic 2D object to display images. Without loading a Texture, this object can be used as a container for other GUI objects if parented together.
---@overload fun(tex:Texture): Image2D
---@overload fun(pos:Vec2, size:Vec2): Image2D
---@return Image2D
function Image2D.new() end

---@class Material
---@field ID number @An ID to identify this `Material` with, being useful for raycast hit results as those can contain a hit ID.
---@field type Lime.Enum.MaterialType @Sets the type of this `Material`, determing how the layers interact with themselves and the world.
---@field fog boolean @Enables fog for this `Material`.
---@field lighting boolean @Enables lighting for this `Material`.
---@field backfaceCulling boolean @Change backface culling behavior for this `Material`.
---@field frontfaceCulling boolean @Change frontface culling behavior for this `Material`.
---@field quality Lime.Enum.Quality @Sets the quality of this Material using `Lime.Enum.Quality` presets, where Low is unfiltered and High is smooth.
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
---@overload fun(img:Texture): Material
---@overload fun(other:Material): Material
---@overload fun(quality:Lime.Enum.Quality): Material
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

---@class Text2D
---@field text string @The text content of this object.
---@field opacity number @The opacity of the text, from 0 to 255. For individual characters being not fully opaque, use color tags with an alpha value.
---@field position Vec2 @The 2D position of this object on the screen.
---@field size Vec2 @The 2D size of this object.
---@field visible boolean @Determines the visibility of this object and its children.
---@field border boolean @Displays a border outlining this object's bounding box on the screen.
---@field backgroundColor Vec4 @The RGBA background color of this object.
---@field enabled boolean @Determines if this object is hoverable and clickable.
---@field onHovered Event @Event called by Lime when this object is hovered.
---@field onPressed Event @Event called by Lime when this object is pressed.
Text2D = Text2D or {}
--- A basic 2D object to display text. Text objects support colors and basic styling. Use tags `<#HEX>` for color, `<s>` for strike, `<d>` for drop shadow, `<u>` for underline, `<b>` for bold, and `<r>` to reset styles. Example: `<#6ABE30>This is green! <b>Now, it's green and bold! <r>Now, it's back to normal.`
---@overload fun(text:string): Text2D
---@overload fun(pos:Vec2, size:Vec2): Text2D
---@return Text2D
function Text2D.new() end

---@class Texture
Texture = Texture or {}
--- A texture that is the foundation for all images for 2D and 3D objects.
---@overload fun(w:number, h:number, name:string?): Texture
---@overload fun(path:string): Texture
---@return Texture
function Texture.new() end

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
---@overload fun(HEX:string): Vec4
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

--- Returns true if this object is currently hovered.
---@return boolean
function Image2D:isHovered() end

--- Loads `texture` into this object.
---@param texture Texture
---@return boolean
function Image2D:loadTexture(texture) end

--- Moves this object to the back in terms of z ordering. (Rendered first, all other objects then overlap)
---@return boolean
function Image2D:moveToBack() end

--- Moves this object to the front in terms of z ordering. (Rendered last, overlaps all other objects)
---@return boolean
function Image2D:moveToFront() end

--- Parents this object to another 2D object.
---@param child any
---@return void
function Image2D:parentTo(child) end

--- In the case that the z ordering of the background for this object is above its children, this will update its z ordering to be correct.
---@return void
function Image2D:updateBackgroundZ() end

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

--- Embeds a bitmap font from path `path`. Returns the name of this font, cut from `path`. (NOTE: `path` must be the path to a .xml file. The .xml files must be paired by an image file.)
---@param path string
---@return string
function Lime.GUI.embedFont(path) end

--- Returns true if the font `name` is embedded.
---@param name string
---@return boolean
function Lime.GUI.isFontEmbedded(name) end

--- Renders all GUI elements to the screen. Returns true on success. (NOTE: Manual rendering must be on, otherwise this function has no effect. See `Lime.setManualRendering`.)
---@return boolean
function Lime.GUI.render() end

--- Sets the default font for new GUI elements to font `name`.
---@param name string
---@return void
function Lime.GUI.setDefaultFont(name) end

--- Sets the quality of all GUI elements using `Lime.Enum.Quality` presets, where Low is unfiltered and High is smooth.
---@param quality Lime.Enum.Quality
---@return void
function Lime.GUI.setQuality(quality) end

--- Returns controller with id `id`'s `axis` axis value from -1.0 to 1.0. Triggers are ranged 0.0 to 1.0.
---@param id number
---@param axis Lime.Enum.ControllerAxis
---@return number
function Lime.Input.getControllerAxis(id, axis) end

--- Returns the name of the controller with id `id`.
---@param id number
---@return string
function Lime.Input.getControllerName(id) end

--- Returns the change in mouse position within the last frame.
---@return Vec2
function Lime.Input.getMouseDelta() end

--- Returns the mouse's position.
---@return Vec2
function Lime.Input.getMousePosition() end

--- Returns true if controller with id `id` has button `button` pressed down.
---@param id number
---@param button Lime.Enum.Controller
---@return boolean
function Lime.Input.isButtonDown(id, button) end

--- Returns true if controller with id `id` is connected.
---@param id number
---@return boolean
function Lime.Input.isControllerConnected(id) end

--- Returns true if `key` is currently pressed.
---@param key Lime.Enum.Key
---@return boolean
function Lime.Input.isKeyDown(key) end

--- Returns true if `button` is currently pressed.
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

--- Returns an `Texture` of a lime and white checkerboard pattern, 2x2. Useful for missing Textures and the like.
---@return Texture
function Lime.Scene.getErrorTexture() end

--- Returns the amount of 3D objects in the scene.
---@return number
function Lime.Scene.getObjectCount() end

--- Renders the scene using the active `Camera` and current rendering parameters. Returns true on success. Clearing the back buffer will replace previously rendered items with the background color. Clearing the Z buffer ensures the previous depth pass is not used. (NOTE: Manual rendering must be on, otherwise this function has no effect. See `Lime.setManualRendering`.)
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

--- Sets the default `Texture` creation quality using `Lime.Enum.TextureCreationQuality`, where Low is optimized for speed and High is optimized for quality.
---@return void
function Lime.Scene.setTextureCreationQuality() end

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

--- Clears the `Texture` in this `Material`.
---@param layer number?
---@return void
function Material:clearTexture(layer) end

--- Loads an `Texture` into this `Material`.
---@overload fun(layer:number, img:Texture): void
---@param img Texture
---@return void
function Material:loadTexture(img) end

--- Sets the scale of the mapping of an `Texture`.
---@overload fun(layer:number, scale:Vec2): void
---@param scale Vec2
---@return void
function Material:setTextureScale(scale) end

--- Changes the method for `Texture` UV wrapping.
---@overload fun(layer:number, uMethod:Lime.Enum.TextureWrapType, vMethod:Lime.Enum.TextureWrapType): void
---@param uMethod Lime.Enum.TextureWrapType
---@param vMethod Lime.Enum.TextureWrapType
---@return void
function Material:setTextureWrapMethod(uMethod, vMethod) end

--- Loads a new Material into this Skydome
---@param material Material
---@return void
function Skydome:loadMaterial(material) end

--- Parents this object to another 3D object.
---@param child any
---@return void
function Skydome:parentTo(child) end

--- Returns true if this object is currently hovered.
---@return boolean
function Text2D:isHovered() end

--- Moves this object to the back in terms of z ordering. (Rendered first, all other objects then overlap)
---@return boolean
function Text2D:moveToBack() end

--- Moves this object to the front in terms of z ordering. (Rendered last, overlaps all other objects)
---@return boolean
function Text2D:moveToFront() end

--- Parents this object to another 2D object.
---@param child any
---@return void
function Text2D:parentTo(child) end

--- Sets the text's alignment within its bounding box.
---@overload fun(x:Lime.Enum.TextAlign, y:Lime.Enum.TextAlign): void
---@param all Lime.Enum.TextAlign
---@return void
function Text2D:setAlignment(all) end

--- Sets the font to use for this object. Fonts must first be embedded. See `Lime.GUI.embedFont`.
---@param name string
---@return boolean
function Text2D:setFont(name) end

--- Enables word wrap.
---@param wrap boolean
---@return void
function Text2D:setWordWrap(wrap) end

--- In the case that the z ordering of the background for this object is above its children, this will update its z ordering to be correct.
---@return void
function Text2D:updateBackgroundZ() end

--- Appends another `Texture` onto this `Texture`.
---@param toAppend Texture
---@param pos Vec2
---@return void
function Texture:append(toAppend, pos) end

--- Crops the `Texture` to the dimensions provided. Be mindful as cropping creates a new `Texture` in the renderer.
---@param topLeft Vec2
---@param bottomRight Vec2
---@return void
function Texture:crop(topLeft, bottomRight) end

--- Returns the color of the pixel at `pos` in this `Texture`.
---@param pos Vec2
---@return Vec4
function Texture:getColor(pos) end

--- Returns the path of this `Texture`.
---@return string
function Texture:getPath() end

--- Returns the dimensions of this `Texture`.
---@return Vec2
function Texture:getSize() end

--- Removes the color `keyColor` from anywhere in this `Texture`.
---@param keyColor Vec4
---@return void
function Texture:keyColor(keyColor) end

--- Replaces the pixel at `pos` with a pixel of color `color`.
---@param pos Vec2
---@param color Vec4
---@return void
function Texture:setColor(pos, color) end

--- Writes the `Texture` to path `path`.
---@param path string
---@return void
function Texture:write(path) end

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

--- Returns the HEX code for this object. This is useful for converting RGBA to HEX color.
---@return string
function Vec4:getHEX() end
