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

---@class Billboard
---@field size Vec2 @The width and height of this Billboard.
---@field position Vec3 @The 3D position of this object in the scene.
---@field rotation Vec3 @The 3D rotation of this object in the scene in degrees.
---@field scale Vec3 @The 3D scale of this object in the scene.
---@field visible boolean @Determines the visibility of this object and its children.
---@field id number @The identifier for this object to be used in raycasts and object selection.
---@field debug boolean @Show debug information about this object in the scene.
Billboard = Billboard or {}
--- A plane that faces the active camera.
---@overload fun(material:Material): Billboard
---@return Billboard
function Billboard.new() end

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
---@field debug boolean @Show debug information about this object in the scene.
Camera = Camera or {}
--- A viewpoint in the 3D world.
---@overload fun(pos:Vec3): Camera
---@overload fun(pos:Vec3, rot:Vec3): Camera
---@return Camera
function Camera.new() end

---@class Empty
---@field position Vec3 @The 3D position of this object in the scene.
---@field rotation Vec3 @The 3D rotation of this object in the scene in degrees.
---@field scale Vec3 @The 3D scale of this object in the scene.
---@field visible boolean @Determines the visibility of this object and its children.
---@field id number @The identifier for this object to be used in raycasts and object selection.
---@field debug boolean @Show debug information about this object in the scene.
Empty = Empty or {}
--- An invisible object used to mark locations, parent objects, and more.
---@overload fun(pos:Vec3): Empty
---@return Empty
function Empty.new() end

---@class Event
Event = Event or {}
---@class HitResult
---@field startPos Vec3 @The starting position of this raycast.
---@field endPos Vec3 @The ending position of this raycast. If an object was hit, this will be the hit position.
---@field objID number @If hit, this will be the hit object's ID. Else, 0.
---@field matID number @If hit, this will be the hit material's ID. Else, 0.
---@field hit boolean @True if the raycast hit a collidable object.
HitResult = HitResult or {}
---@class Hook
Hook = Hook or {}
---@class Image2D
---@field scaleToFit boolean @Determines whether or not this `Image2D` scales to its size boundaries or not.
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

---@class Mesh
---@field vertexColor Vec4 @Sets the color of all vertexes in this `Mesh`.
---@field collision boolean @Allows response to raypicks and other simple collision methods. (NOTE: This flag does not affect this `Mesh` when wrapped by a physics object.)
---@field shadows boolean @Enables shadows.
---@field position Vec3 @The 3D position of this object in the scene.
---@field rotation Vec3 @The 3D rotation of this object in the scene in degrees.
---@field scale Vec3 @The 3D scale of this object in the scene.
---@field visible boolean @Determines the visibility of this object and its children.
---@field id number @The identifier for this object to be used in raycasts and object selection.
---@field debug boolean @Show debug information about this object in the scene.
Mesh = Mesh or {}
--- A scene object capable of displaying a mesh.
---@overload fun(path:string): Mesh
---@overload fun(buffer:MeshBuffer): Mesh
---@return Mesh
function Mesh.new() end

---@class MeshBuffer
MeshBuffer = MeshBuffer or {}
--- A container for vertices.
---@return MeshBuffer
function MeshBuffer.new() end

---@class Skydome
---@field position Vec3 @The 3D position of this object in the scene.
---@field rotation Vec3 @The 3D rotation of this object in the scene in degrees.
---@field scale Vec3 @The 3D scale of this object in the scene.
---@field visible boolean @Determines the visibility of this object and its children.
---@field id number @The identifier for this object to be used in raycasts and object selection.
---@field debug boolean @Show debug information about this object in the scene.
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

---@class Text3D
---@field text string @The text content of this object.
---@field opacity number @The opacity of the text, from 0 to 255. For individual characters being not fully opaque, use color tags with an alpha value.
---@field size Vec2 @The size of the 2D text box.
---@field position Vec3 @The 3D position of this object in the scene.
---@field rotation Vec3 @The 3D rotation of this object in the scene in degrees.
---@field scale Vec3 @The 3D scale of this object in the scene.
---@field visible boolean @Determines the visibility of this object and its children.
---@field id number @The identifier for this object to be used in raycasts and object selection.
---@field debug boolean @Show debug information about this object in the scene.
Text3D = Text3D or {}
--- A basic 3D object to display text. This object is essentially a `Text2D` tied to a 3D scene node. Text objects support colors and basic styling. Use tags `<#HEX>` for color, `<s>` for strike, `<d>` for drop shadow, `<u>` for underline, `<b>` for bold, and `<r>` to reset styles. Example: `<#6ABE30>This is green! <b>Now, it's green and bold! <r>Now, it's back to normal.`
---@overload fun(text:string): Text3D
---@return Text3D
function Text3D.new() end

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

--- Destroys this object.
---@return nil
function Billboard:destroy() end

--- Returns the bounding box of this object, following: (MinEdgeX, MinEdgeY, MaxEdgeX, MaxEdgeY).
---@return Vec4
function Billboard:getBoundingBox() end

--- Returns the reference count for this object.
---@return number
function Billboard:getReferenceCount() end

--- Returns true if `pos` is inside this object's bounding box.
---@param pos Vec3
---@return boolean
function Billboard:isPointInside(pos) end

--- Loads a `Material` into this `Billboard`.
---@param material Material
---@return void
function Billboard:loadMaterial(material) end

--- Parents this object to another 3D object.
---@param parent any
---@return void
function Billboard:parentTo(parent) end

--- Destroys this object.
---@return nil
function Camera:destroy() end

--- Returns the bounding box of this object, following: (MinEdgeX, MinEdgeY, MaxEdgeX, MaxEdgeY).
---@return Vec4
function Camera:getBoundingBox() end

--- Returns the forward vector.
---@return Vec3
function Camera:getForward() end

--- Returns the left vector.
---@return Vec3
function Camera:getLeft() end

--- Returns the reference count for this object.
---@return number
function Camera:getReferenceCount() end

--- Returns true if `pos` is inside this object's bounding box.
---@param pos Vec3
---@return boolean
function Camera:isPointInside(pos) end

--- Parents this object to another 3D object.
---@param parent any
---@return void
function Camera:parentTo(parent) end

--- When rendering, this `Camera` will take over as the active rendering viewpoint.
---@return void
function Camera:setActive() end

--- Destroys this object.
---@return nil
function Empty:destroy() end

--- Returns the bounding box of this object, following: (MinEdgeX, MinEdgeY, MaxEdgeX, MaxEdgeY).
---@return Vec4
function Empty:getBoundingBox() end

--- Returns the reference count for this object.
---@return number
function Empty:getReferenceCount() end

--- Returns true if `pos` is inside this object's bounding box.
---@param pos Vec3
---@return boolean
function Empty:isPointInside(pos) end

--- Parents this object to another 3D object.
---@param parent any
---@return void
function Empty:parentTo(parent) end

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

--- Destroys this object.
---@return nil
function Image2D:destroy() end

--- Returns the reference count for this object.
---@return number
function Image2D:getReferenceCount() end

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
---@param parent any
---@return void
function Image2D:parentTo(parent) end

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

--- Sets debug console configuration. If `enable` is true, the debug console will appear alongside the application. If `writeOutput` is true, the console's output will be written to a output.log file in the application directory. (NOTE: Enabling the debug console may cause minor hiccups at runtime.)
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

--- Returns a `Mesh` containing a cube.
---@param size Vec3
---@return Mesh
function Lime.Scene.createCubeMesh(size) end

--- Returns a `Mesh` containing a cylinder.
---@overload fun(radius:number, length:number, polyCount:number, closed:boolean): Mesh
---@param radius number
---@param length number
---@return Mesh
function Lime.Scene.createCylinderMesh(radius, length) end

--- Returns a `Mesh` containing a plane. Parameter `repeatCount` controls how much an applied `Texture` will repeat within one tile.
---@overload fun(tileSize:Vec2, tileCount:Vec2, repeatCount:Vec2): Mesh
---@param tileSize Vec2
---@param tileCount Vec2
---@return Mesh
function Lime.Scene.createPlaneMesh(tileSize, tileCount) end

--- Returns a `Mesh` containing a sphere.
---@overload fun(radius:number, polyCount:number): Mesh
---@param radius number
---@return Mesh
function Lime.Scene.createSphereMesh(radius) end

--- Fires a raycast out into the scene from `startPos` to `endPos`. Only objects with collision enabled will be tested.
---@param startPos Vec3
---@param endPos Vec3
---@param rayLifeMs number?
---@return HitResult
function Lime.Scene.fireRaycast(startPos, endPos, rayLifeMs) end

--- Returns an `Texture` of a lime and white checkerboard pattern, 2x2. Useful for missing Textures and the like.
---@return Texture
function Lime.Scene.getErrorTexture() end

--- Returns the amount of 3D objects in the scene.
---@return number
function Lime.Scene.getObjectCount() end

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

--- Sets the render quality of the scene using `Lime.Enum.Quality`.
---@param quality Lime.Enum.Quality
---@return void
function Lime.Scene.setRenderQuality(quality) end

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

--- Loads a `Texture` into this `Material`.
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

--- Clears the `MeshBuffer` from within this `Mesh`. This will not remove its `MeshBuffer` from memory.
---@return void
function Mesh:clear() end

--- Destroys this object.
---@return nil
function Mesh:destroy() end

--- Returns the bounding box of this object, following: (MinEdgeX, MinEdgeY, MaxEdgeX, MaxEdgeY).
---@return Vec4
function Mesh:getBoundingBox() end

--- Returns the material count of this `Mesh`.
---@return number
function Mesh:getMaterialCount() end

--- Returns the reference count for this object.
---@return number
function Mesh:getReferenceCount() end

--- Returns the vertex count of this `Mesh`.
---@return number
function Mesh:getVertexCount() end

--- Returns true if `pos` is inside this object's bounding box.
---@param pos Vec3
---@return boolean
function Mesh:isPointInside(pos) end

--- Loads a `Material` into this `Mesh`.
---@overload fun(layer:number, material:Material): boolean
---@param material Material
---@return boolean
function Mesh:loadMaterial(material) end

--- Loads a 3D model into this `Mesh`. If importing from file, acceptable formats include `.obj`, `.fbx`, and `.x`.
---@overload fun(buffer:MeshBuffer): boolean
---@param path string
---@return boolean
function Mesh:loadMesh(path) end

--- Parents this object to another 3D object.
---@param parent any
---@return void
function Mesh:parentTo(parent) end

--- Purges this `Mesh`, effectively removing its mesh buffer from memory. Objects using this `Mesh`'s `MeshBuffer` will use an engine-defined `Mesh` instead, but it is recommended to remove references to this `Mesh` first.
---@return nil
function Mesh:purge() end

--- Tells the graphics system how to store this `Mesh`. By default, `Mesh` objects use Static. Use Dynamic (or more intensely, Stream) if the `Mesh` is updated frequently.
---@param hint Lime.Enum.StorageHint
---@return void
function Mesh:setStorageHint(hint) end

--- If shadows are enabled for this `Mesh`, this will update the projection of the shadow. Use this if light sources have moved.
---@return void
function Mesh:updateShadow() end

--- Clears this `MeshBuffer`. This will not remove itself from memory.
---@return void
function MeshBuffer:clear() end

--- Returns the reference count for this `MeshBuffer`.
---@return number
function MeshBuffer:getReferenceCount() end

--- Returns the vertex count of this `MeshBuffer`.
---@return number
function MeshBuffer:getVertexCount() end

--- Purges this `MeshBuffer`, effectively removing it from memory. Objects using this `MeshBuffer` will use an engine-defined `MeshBuffer` instead, but it is recommended to remove references to this `MeshBuffer` first.
---@return nil
function MeshBuffer:purge() end

--- Pushes a face to this `MeshBuffer`.
---@param pos1 Vec3
---@param pos2 Vec3
---@param pos3 Vec3
---@param normal1 Vec3
---@param normal2 Vec3
---@param normal3 Vec3
---@param uvw1 Vec3
---@param uvw2 Vec3
---@param uvw3 Vec3
---@param color1 Vec4
---@param color2 Vec4
---@param color3 Vec4
---@return void
function MeshBuffer:pushFace(pos1, pos2, pos3, normal1, normal2, normal3, uvw1, uvw2, uvw3, color1, color2, color3) end

--- Recalculates the bounding box of this `MeshBuffer`. This should be called after modifying this `MeshBuffer`.
---@return void
function MeshBuffer:recalculateBoundingBox() end

--- Destroys this object.
---@return nil
function Skydome:destroy() end

--- Returns the bounding box of this object, following: (MinEdgeX, MinEdgeY, MaxEdgeX, MaxEdgeY).
---@return Vec4
function Skydome:getBoundingBox() end

--- Returns the reference count for this object.
---@return number
function Skydome:getReferenceCount() end

--- Returns true if `pos` is inside this object's bounding box.
---@param pos Vec3
---@return boolean
function Skydome:isPointInside(pos) end

--- Loads a new Material into this Skydome
---@param material Material
---@return void
function Skydome:loadMaterial(material) end

--- Parents this object to another 3D object.
---@param parent any
---@return void
function Skydome:parentTo(parent) end

--- Destroys this object.
---@return nil
function Text2D:destroy() end

--- Returns the reference count for this object.
---@return number
function Text2D:getReferenceCount() end

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
---@param parent any
---@return void
function Text2D:parentTo(parent) end

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

--- Destroys this object.
---@return nil
function Text3D:destroy() end

--- Returns the bounding box of this object, following: (MinEdgeX, MinEdgeY, MaxEdgeX, MaxEdgeY).
---@return Vec4
function Text3D:getBoundingBox() end

--- Returns the reference count for this object.
---@return number
function Text3D:getReferenceCount() end

--- Returns true if `pos` is inside this object's bounding box.
---@param pos Vec3
---@return boolean
function Text3D:isPointInside(pos) end

--- Parents this object to another 3D object.
---@param parent any
---@return void
function Text3D:parentTo(parent) end

--- Sets the text's alignment within its bounding box.
---@overload fun(x:Lime.Enum.TextAlign, y:Lime.Enum.TextAlign): void
---@param all Lime.Enum.TextAlign
---@return void
function Text3D:setAlignment(all) end

--- Sets the font to use for this object. Fonts must first be embedded. See `Lime.GUI.embedFont`.
---@param name string
---@return boolean
function Text3D:setFont(name) end

--- Enables word wrap.
---@param wrap boolean
---@return void
function Text3D:setWordWrap(wrap) end

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

--- Returns the reference count for this `Texture`.
---@return number
function Texture:getReferenceCount() end

--- Returns the dimensions of this `Texture`.
---@return Vec2
function Texture:getSize() end

--- Removes the color `keyColor` from anywhere in this `Texture`.
---@param keyColor Vec4
---@return void
function Texture:keyColor(keyColor) end

--- Purges this `Texture`, effectively removing it from memory. Objects using this `Texture` will use an engine-defined `Texture` instead, but it is recommended to remove references to this `Texture` first.
---@return nil
function Texture:purge() end

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
