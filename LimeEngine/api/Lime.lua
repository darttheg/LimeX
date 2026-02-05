---@class Lime
---@field onInit Event @Event called by Lime prior to initializing the window.
---@field onStart Event @Event called by Lime following window creation and rendering services are available.
---@field onUpdate Event @Event called by Lime every rendering frame. This Event is run with a number delta time argument.
---@field onClose Event @Event called by Lime once the application closes in any way.
Lime = Lime or {}

---@class Lime.Scene
Lime.Scene = Lime.Scene or {}

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
---@field ID number @An ID to identify this Material with, being useful for raycast hit results as those can contain a hit Material ID.
---@field type Lime.MaterialType @Sets the type of this Material, determing how the layers interact with themselves and the world
---@field fog boolean @Enables fog for this Material
---@field lighting boolean @Enables lighting for this Material
---@field backfaceCulling boolean @Change backface culling behavior for this Material
---@field frontfaceCulling boolean @Change frontface culling behavior for this Material
---@field quality Lime.MaterialQuality @Sets the quality of this Material using Lime.MaterialQuality presets, where Low is retro/old-school and Ultra is smooth and high quality
---@field wireframe boolean @Enables wireframe view for this Material
---@field zMethod Lime.ZOrderMethod @Sets Z ordering method for this Material using Lime.ZOrderMethod
---@field opacity number @Sets the opacity of this Material from 0.0 (invisible) to 1.0 (visible), affecting the transparency of objects with this Material applied (NOTE: Will not affect solid Materials)
---@field mipmaps boolean @Enables the generation of mipmaps
---@field shine number @Sets the shine for this Material, ranging from 0 (soft and wide shine) to 1 (harsh and small shine)
---@field writeToDepth boolean @Enables this material writing to the depth buffer on render, where false is common for transparent objects
---@field ambientColor Vec4 @Sets the ambient color for this Material, the base color
---@field diffuseColor Vec4 @Sets the diffuse color for this Material, the light-affected base color
---@field specularColor Vec4 @Sets the specular color for this Material, the shine color
---@field emissiveColor Vec4 @Sets the emissive color for this Material, the color that is seen through shadows, lighting, and fog
Material = Material or {}
--- An object used to hold material parameters for 3D objects. A Material has at most two layers, with Lime.MaterialType allowing for different combinations of said layers.
---@overload fun(img:Image): Material
---@overload fun(other:Material): Material
---@overload fun(quality:Lime.MaterialQuality): Material
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

--- Appends another Image onto this Image.
---@param toAppend Image
---@param pos Vec2
---@return void
function Image:append(toAppend, pos) end

--- Crops the Image to the dimensions provided. Cropping creates a new Image in the renderer, so be mindful and free unused and uncropped Images.
---@param topLeft Vec2
---@param bottomRight Vec2
---@return void
function Image:crop(topLeft, bottomRight) end

--- Returns the color of the pixel at a position in this Image.
---@param pos Vec2
---@return Vec4
function Image:getColor(pos) end

--- Returns the path of this Image.
---@return string
function Image:getPath() end

--- Returns the dimensions of this Image.
---@return Vec2
function Image:getSize() end

--- Removes the color keyColor from anywhere in the Image.
---@param keyColor Vec4
---@return void
function Image:keyColor(keyColor) end

--- Replaces the pixel at the provided position with the color provided.
---@param pos Vec2
---@param color Vec4
---@return void
function Image:setColor(pos, color) end

--- Writes the Image to a path.
---@param path string
---@return void
function Image:write(path) end

--- Closes the Lime application.
function Lime.close() end

--- Returns the Lime version running.
---@return string
function Lime.getVersion() end

--- Prints a message to console.
---@param msg any
---@param color Lime.PrintColor?
---@return void
function Lime.log(msg, color) end

--- If set to true, Lime will close on any error. A pop-up will be disclosed prior with error details.
---@param doEnd boolean
function Lime.setEndOnError(doEnd) end

--- IMPORTANT: This function should always be run prior to window creation (pre-Lime.Update Event) as only here can the driver type be changed. This function sets initial parameters for the Lime application.
---@param driver Lime.DriverType
---@param vSync boolean?
---@param frameRate number?
---@param windowSize Vec2?
---@param renderSize Vec2?
---@param scaleRenderToWindow boolean?
---@param fullscreen boolean?
---@return boolean
function Lime.setInitConfig(driver, vSync, frameRate, windowSize, renderSize, scaleRenderToWindow, fullscreen) end

--- Clears the Image in this Material
---@param layer number?
---@return void
function Material:clearImage(layer) end

--- Loads an Image into this Material
---@overload fun(layer:number, img:Image): void
---@param img Image
---@return void
function Material:loadImage(img) end

--- Sets the scale of an Image's mapping
---@overload fun(layer:number, scale:Vec2): void
---@param scale Vec2
---@return void
function Material:setImageScale(scale) end

--- Changes the method for Image UV wrapping
---@overload fun(layer:number, uMethod:Lime.ImageWrapType, vMethod:Lime.ImageWrapType): void
---@param uMethod Lime.ImageWrapType
---@param vMethod Lime.ImageWrapType
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
