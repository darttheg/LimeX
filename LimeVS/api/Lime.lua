---@alias OnInitCallback fun()
---@class OnInitEvent : Event
--- Event called by Lime prior to initializing the window.
OnInitEvent = OnInitEvent or {}
---@param callback OnInitCallback
function OnInitEvent:hook(callback) end

---@alias OnStartCallback fun()
---@class OnStartEvent : Event
--- Event called by Lime following window creation and rendering services are available.
OnStartEvent = OnStartEvent or {}
---@param callback OnStartCallback
function OnStartEvent:hook(callback) end

---@alias OnUpdateCallback fun(dt: number)
---@class OnUpdateEvent : Event
--- Event called by Lime every rendering frame. This Event is run with a number `dt` argument.
OnUpdateEvent = OnUpdateEvent or {}
---@param callback OnUpdateCallback
function OnUpdateEvent:hook(callback) end

---@alias OnCloseCallback fun()
---@class OnCloseEvent : Event
--- Event called by Lime once the application closes in any way.
OnCloseEvent = OnCloseEvent or {}
---@param callback OnCloseCallback
function OnCloseEvent:hook(callback) end

---@alias OnKeyPressedCallback fun(key: Lime.Enum.Key)
---@class OnKeyPressedEvent : Event
--- Event called by Lime when a key is pressed.
OnKeyPressedEvent = OnKeyPressedEvent or {}
---@param callback OnKeyPressedCallback
function OnKeyPressedEvent:hook(callback) end

---@alias OnKeyReleasedCallback fun(key: Lime.Enum.Key)
---@class OnKeyReleasedEvent : Event
--- Event called by Lime when a key is released.
OnKeyReleasedEvent = OnKeyReleasedEvent or {}
---@param callback OnKeyReleasedCallback
function OnKeyReleasedEvent:hook(callback) end

---@alias OnMouseButtonPressedCallback fun(button: Lime.Enum.Mouse)
---@class OnMouseButtonPressedEvent : Event
--- Event called by Lime when a mouse button is pressed.
OnMouseButtonPressedEvent = OnMouseButtonPressedEvent or {}
---@param callback OnMouseButtonPressedCallback
function OnMouseButtonPressedEvent:hook(callback) end

---@alias OnMouseButtonReleasedCallback fun(button: Lime.Enum.Mouse)
---@class OnMouseButtonReleasedEvent : Event
--- Event called by Lime when a mouse button is released.
OnMouseButtonReleasedEvent = OnMouseButtonReleasedEvent or {}
---@param callback OnMouseButtonReleasedCallback
function OnMouseButtonReleasedEvent:hook(callback) end

---@alias OnMouseMovedCallback fun(delta: Vec2)
---@class OnMouseMovedEvent : Event
--- Event called by Lime when a mouse is moved.
OnMouseMovedEvent = OnMouseMovedEvent or {}
---@param callback OnMouseMovedCallback
function OnMouseMovedEvent:hook(callback) end

---@alias OnMouseScrollCallback fun(delta: number)
---@class OnMouseScrollEvent : Event
--- Event called by Lime when the mouse scroll wheel is moved.
OnMouseScrollEvent = OnMouseScrollEvent or {}
---@param callback OnMouseScrollCallback
function OnMouseScrollEvent:hook(callback) end

---@alias OnControllerConnectedCallback fun(id: number)
---@class OnControllerConnectedEvent : Event
--- Event called by Lime when a controller is connected.
OnControllerConnectedEvent = OnControllerConnectedEvent or {}
---@param callback OnControllerConnectedCallback
function OnControllerConnectedEvent:hook(callback) end

---@alias OnControllerDisconnectedCallback fun(id: number)
---@class OnControllerDisconnectedEvent : Event
--- Event called by Lime when a controller is disconnected.
OnControllerDisconnectedEvent = OnControllerDisconnectedEvent or {}
---@param callback OnControllerDisconnectedCallback
function OnControllerDisconnectedEvent:hook(callback) end

---@alias OnControllerButtonPressedCallback fun(id: number, button: Lime.Enum.Controller)
---@class OnControllerButtonPressedEvent : Event
--- Event called by Lime when a controller button is pressed.
OnControllerButtonPressedEvent = OnControllerButtonPressedEvent or {}
---@param callback OnControllerButtonPressedCallback
function OnControllerButtonPressedEvent:hook(callback) end

---@alias OnControllerButtonReleasedCallback fun(id: number, button: Lime.Enum.Controller)
---@class OnControllerButtonReleasedEvent : Event
--- Event called by Lime when a controller button is released.
OnControllerButtonReleasedEvent = OnControllerButtonReleasedEvent or {}
---@param callback OnControllerButtonReleasedCallback
function OnControllerButtonReleasedEvent:hook(callback) end

---@alias OnResizeCallback fun()
---@class OnResizeEvent : Event
--- Event called by Lime once the window is resized in any way.
OnResizeEvent = OnResizeEvent or {}
---@param callback OnResizeCallback
function OnResizeEvent:hook(callback) end

---@alias OnHoveredCallback fun()
---@class OnHoveredEvent : Event
--- Event called by Lime when this object is hovered.
OnHoveredEvent = OnHoveredEvent or {}
---@param callback OnHoveredCallback
function OnHoveredEvent:hook(callback) end

---@alias OnPressedCallback fun()
---@class OnPressedEvent : Event
--- Event called by Lime when this object is pressed.
OnPressedEvent = OnPressedEvent or {}
---@param callback OnPressedCallback
function OnPressedEvent:hook(callback) end

---@alias OnEnterCallback fun(result: CollisionResult)
---@class OnEnterEvent : Event
--- Event called when another physics object collides with this object for the first time.
OnEnterEvent = OnEnterEvent or {}
---@param callback OnEnterCallback
function OnEnterEvent:hook(callback) end

---@alias OnInsideCallback fun()
---@class OnInsideEvent : Event
--- Event called when another physics object is inside this physics object.
OnInsideEvent = OnInsideEvent or {}
---@param callback OnInsideCallback
function OnInsideEvent:hook(callback) end

---@alias OnExitCallback fun()
---@class OnExitEvent : Event
--- Event called when another physics object exits this physics object.
OnExitEvent = OnExitEvent or {}
---@param callback OnExitCallback
function OnExitEvent:hook(callback) end

---@class Lime
---@field onInit OnInitEvent @Event called by Lime prior to initializing the window.
---@field onStart OnStartEvent @Event called by Lime following window creation and rendering services are available.
---@field onUpdate OnUpdateEvent @Event called by Lime every rendering frame. This Event is run with a number `dt` argument.
---@field onClose OnCloseEvent @Event called by Lime once the application closes in any way.
Lime = Lime or {}

---@class Lime.File
Lime.File = Lime.File or {}

---@class Lime.GUI
Lime.GUI = Lime.GUI or {}

---@class Lime.Input
---@field onKeyPressed OnKeyPressedEvent @Event called by Lime when a key is pressed.
---@field onKeyReleased OnKeyReleasedEvent @Event called by Lime when a key is released.
---@field onMouseButtonPressed OnMouseButtonPressedEvent @Event called by Lime when a mouse button is pressed.
---@field onMouseButtonReleased OnMouseButtonReleasedEvent @Event called by Lime when a mouse button is released.
---@field onMouseMoved OnMouseMovedEvent @Event called by Lime when a mouse is moved.
---@field onMouseScroll OnMouseScrollEvent @Event called by Lime when the mouse scroll wheel is moved.
---@field onControllerConnected OnControllerConnectedEvent @Event called by Lime when a controller is connected.
---@field onControllerDisconnected OnControllerDisconnectedEvent @Event called by Lime when a controller is disconnected.
---@field onControllerButtonPressed OnControllerButtonPressedEvent @Event called by Lime when a controller button is pressed.
---@field onControllerButtonReleased OnControllerButtonReleasedEvent @Event called by Lime when a controller button is released.
Lime.Input = Lime.Input or {}

---@class Lime.Physics
Lime.Physics = Lime.Physics or {}

---@class Lime.Scene
Lime.Scene = Lime.Scene or {}

---@class Lime.Sound
Lime.Sound = Lime.Sound or {}

---@class Lime.Window
---@field onResize OnResizeEvent @Event called by Lime once the window is resized in any way.
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

---@class CollisionResult
---@field depth number @Penetration depth
---@field posA Vec3 @Contact point on physics object A
---@field posB Vec3 @Contact point on physics object B
---@field normal Vec3 @Collision normal from physics object A to B
---@field linearVelocityA Vec3 @Linear velocity of physics object A
---@field linearVelocityB Vec3 @Linear velocity of physics object B
---@field angularVelocityA Vec3 @Angular velocity of physics object A
---@field angularVelocityB Vec3 @Angular velocity of physics object B
---@field velocityAtPointA Vec3 @Actual velocity at physics object A contact point
---@field velocityAtPointB Vec3 @Actual velocity at physics object B contact point
---@field relativeVelocity Vec3 @velocityAtPointB - velocityAtPointA
---@field impactSpeed number @Impact speed
---@field attributesB table @Attributes of physics object B
CollisionResult = CollisionResult or {}
---@class ConeTwistConstraint
---@field swingLimits Vec2 @The angular swing limits of this `Constraint`, where `x` is sideways and `y` is forward and backward.
---@field twistLimit number @The angular twist limit.
---@field active boolean @Whether or not this `Constraint` is active.
---@field ignoreCollision boolean @Whether or not the `RigidBody` objects of this `Constraint` should ignore collision between one another. This value should be altered prior to activating this `Constraint` to take effect in the scene.
---@field breakThreshold number @The impulse threshold this `Constraint` can endure before it breaks, deactivating itself. Physics objects default to unbreakable, but altering this value will enable this object to be prone to breaking.
ConeTwistConstraint = ConeTwistConstraint or {}
--- A physics `Constraint` that twists two `RigidBody` objects together like a limited socket joint. Pivots and axis parameters are in local space. Axis vector values range from 0 to 1, where 1 allows rotation around said axis and vice versa.
---@param rbA RigidBody
---@param rbB RigidBody
---@param localPivotA Vec3
---@param localPivotB Vec3
---@param localAxisA Vec3
---@param localAxisB Vec3
---@return ConeTwistConstraint
function ConeTwistConstraint.new(rbA, rbB, localPivotA, localPivotB, localAxisA, localAxisB) end

---@class EditBox
---@field text string @The text content of this object.
---@field password boolean @Determines if the text content of this `EditBox` is obfuscated.
---@field enabled boolean @Determines if this `EditBox` is enabled.
---@field cursorPosition number @The position of the cursor in this `EditBox`.
---@field multiLine boolean @Determines if this `EditBox` supports multiple lines.
---@field background boolean @Determines if this `EditBox` has a visible background.
---@field autoscroll boolean @Determines if this `EditBox` will autoscroll.
---@field focused boolean @Determines if this `EditBox` is focused.
---@field wordWrap boolean @Determines if the text wraps when touching the border of its text box.
---@field position Vec2 @The 2D position of this object on the screen.
---@field size Vec2 @The 2D size of this object.
---@field visible boolean @Determines the visibility of this object and its children.
---@field border boolean @Displays a border outlining this object's bounding box on the screen.
---@field backgroundColor Vec4 @The RGBA background color of this object.
---@field onHovered OnHoveredEvent @Event called by Lime when this object is hovered.
---@field onPressed OnPressedEvent @Event called by Lime when this object is pressed.
EditBox = EditBox or {}
--- A basic 2D object that allows user input. `EditBox` objects can be used for getting user input and can even be made to look like passwords.
---@overload fun(text:string): EditBox
---@overload fun(pos:Vec2, size:Vec2): EditBox
---@return EditBox
function EditBox.new() end

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
---@class HingeConstraint
---@field limits Vec2 @The angular limits of the hinge, where `x` is lower limit and `y` is upper limit.
---@field motor boolean @Whether or not this `Constraint` applies angular velocity every physics step, rotating itself like a motor.
---@field motorVelocity number @If this `Constraint` is a motor, this determines the target rotational velocity.
---@field maxMotorImpulse number @If this `Constraint` is a motor, this determines the maximum force allowed to spin to reach the target motorVelocity.
---@field active boolean @Whether or not this `Constraint` is active.
---@field ignoreCollision boolean @Whether or not the `RigidBody` objects of this `Constraint` should ignore collision between one another. This value should be altered prior to activating this `Constraint` to take effect in the scene.
---@field breakThreshold number @The impulse threshold this `Constraint` can endure before it breaks, deactivating itself. Physics objects default to unbreakable, but altering this value will enable this object to be prone to breaking.
HingeConstraint = HingeConstraint or {}
--- A physics `Constraint` that hinges two `RigidBody` objects together. Pivots and axis parameters are in local space. Axis vector values range from 0 to 1, where 1 allows rotation around said axis and vice versa.
---@param rbA RigidBody
---@param rbB RigidBody
---@param localPivotA Vec3
---@param localPivotB Vec3
---@param localAxisA Vec3
---@param localAxisB Vec3
---@return HingeConstraint
function HingeConstraint.new(rbA, rbB, localPivotA, localPivotB, localAxisA, localAxisB) end

---@class HitResult
---@field startPos Vec3 @The starting position of this raycast.
---@field endPos Vec3 @The ending position of this raycast. If an object was hit, this will be the hit position.
---@field normal Vec3 @The normal vector of the hit triangle, if any.
---@field objectID number @If hit, this will be the hit object's ID. Else, 0.
---@field materialID number @If hit, this will be the hit material's ID. Else, 0.
---@field hit boolean @True if the raycast hit a collidable object.
---@field attributes table @Attributes of the hit object, if any.
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
---@field onHovered OnHoveredEvent @Event called by Lime when this object is hovered.
---@field onPressed OnPressedEvent @Event called by Lime when this object is pressed.
Image2D = Image2D or {}
--- A basic 2D object to display images. Without loading a Texture, this object can be used as a container for other GUI objects if parented together.
---@overload fun(tex:Texture): Image2D
---@overload fun(pos:Vec2, size:Vec2): Image2D
---@return Image2D
function Image2D.new() end

---@class Light
---@field type Lime.Enum.LightType @Sets the type of this `Light`.
---@field intensity number @Scales the intensity of luminosity from this `Light`.
---@field radius number @The cut-off distance for light reach around its center. Not effective for directional light sources.
---@field falloff number @Dictates the blend from inner to outer cones for spotlights. For example, <1.0 is soft, 1.0 is linear, 2.0 is a bit harsher, 10.0+ is a very harsh cut-off.
---@field diffuseColor Vec4 @Sets the diffuse color for this `Light`, the main light color.
---@field ambientColor Vec4 @Sets the ambient color for this `Light`, the atmospheric color applied to all objects.
---@field specularColor Vec4 @Sets the specular color for this `Light`, the color that appears on shiny objects.
---@field attenuation Vec3 @Sets the attenuation, or spread behavior, of this `Light`. Format is `(Constant, Linear, Quadratic)`, all ranging from 0.0 to 1.0. Not effective for directional light sources.
---@field cones Vec2 @Sets the inner and outer cones of this `Light`. This is only used for spotlights.
---@field position Vec3 @The 3D position of this object in the scene.
---@field rotation Vec3 @The 3D rotation of this object in the scene in degrees.
---@field scale Vec3 @The 3D scale of this object in the scene.
---@field visible boolean @Determines the visibility of this object and its children.
---@field id number @The identifier for this object to be used in raycasts and object selection.
---@field debug boolean @Show debug information about this object in the scene.
Light = Light or {}
--- A source of light.
---@overload fun(type:Lime.Enum.LightType): Light
---@return Light
function Light.new() end

---@class Material
---@field ID number @An ID to identify this `Material` with, being useful for raycast hit results as those can contain a hit ID.
---@field type Lime.Enum.MaterialType @Sets the type of this `Material`, determing how the layers interact with themselves and the world.
---@field fog boolean @Enables fog for this `Material`.
---@field lighting boolean @Enables lighting for this `Material`.
---@field backfaceCulling boolean @Change backface culling behavior for this `Material`.
---@field frontfaceCulling boolean @Change frontface culling behavior for this `Material`.
---@field quality Lime.Enum.Quality @Sets the quality of this `Material` using `Lime.Enum.Quality` presets, where Low is unfiltered and High is smooth.
---@field wireframe boolean @Enables wireframe view for this `Material`.
---@field zMethod Lime.Enum.ZOrderMethod @Sets Z ordering method for this `Material` using `Lime.Enum.ZOrderMethod`.
---@field opacity number @Sets the opacity of this `Material` from 0.0 (invisible) to 1.0 (visible), affecting the transparency of objects with this `Material` applied. (NOTE: Will not affect solid types)
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
---@field frame number @Controls the current frame of animation.
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

---@class Noise
---@field seed number @Sets the seed of this `Noise`.
---@field octaves number @Sets the number of octaves in this `Noise`.
Noise = Noise or {}
--- An object that holds gradient noise. Useful for random terrain, clouds, and more.
---@overload fun(seed:number): Noise
---@overload fun(seed:number, octaves:number): Noise
---@return Noise
function Noise.new() end

---@class RigidBody
---@field position Vec3 @The 3D position of this object in the scene.
---@field rotation Vec3 @The 3D rotation of this object in the scene in degrees.
---@field friction number @Sets the friction coefficient.
---@field anisotropicFriction Vec3 @Sets per-axis friction scaling.
---@field mass number @Sets the mass and recalculates inertia.
---@field sleeping boolean @Whether or not this physics object is sleeping.
---@field ghost boolean @Sets whether or not other physics objects can pass through this object.
---@field linearDamping number @Sets the linear damping, reducing linear velocity over time.
---@field angularDamping number @Sets the angular damping, reducing rotational velocity over time.
---@field elasticity number @Sets the elasticity, controlling bounciness on collision.
---@field sleepThreshold number @Sets the velocity threshold this `RigidBody` must be below in order to trigger sleeping.
---@field linearVelocity Vec3 @Sets the linear velocity.
---@field angularVelocity Vec3 @Sets the angular velocity.
---@field linearFactor Vec3 @Sets the linear factor, controlling which axes linear motion is allowed on. (0 locks, 1 allows)
---@field angularFactor Vec3 @Sets the angular factor, controlling which axes rotation is allowed on. (0 locks, 1 allows)
---@field onEnter OnEnterEvent @Event called when another physics object collides with this object for the first time.
---@field onInside OnInsideEvent @Event called when another physics object is inside this physics object.
---@field onExit OnExitEvent @Event called when another physics object exits this physics object.
RigidBody = RigidBody or {}
--- A wrapper to `Mesh` objects that allows for them to react to physics. It can be created with a `Mesh` as its visual and collision shape, or with a custom collision shape independent of any 3D object.
---@overload fun(root:any, collision:Mesh): RigidBody
---@param base Mesh
---@return RigidBody
function RigidBody.new(base) end

---@class Shader
Shader = Shader or {}
--- A special material that can produce custom effects. Apply `Shader` objects to `Material` objects or to the screen with `Lime.Scene.setPostProcessingShader`. By default, all `Shader` objects set internal parameters `uWorldTransformed` to the current world-view projection matrix, `uWorld` to just the current world matrix, and `uTime` to the elapsed time in seconds. (decimal)
---@param vertexShaderPath string
---@param pixelShaderPath string
---@param type Lime.Enum.MaterialType?
---@return Shader
function Shader.new(vertexShaderPath, pixelShaderPath, type) end

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

---@class SoundSource
---@field paused boolean @Whether or not this `SoundSource` is paused.
---@field looping boolean @Whether or not this `SoundSource` loops on playback.
---@field volume number @The volume of this `SoundSource`.
---@field speed number @The playback speed of this `SoundSource`.
---@field pan number @The pan of this `SoundSource`, where -1.0 is left and 1.0 is right.
---@field minimumDistance number @Sets the minimum listening distance for this `SoundSource`. Only applicable if this object is played in 3D.
---@field maximumDistance number @Sets the maximum listening distance for this `SoundSource`. Only applicable if this object is played in 3D.
---@field playbackPosition number @The current playback position of this `SoundSource`.
---@field velocity Vec3 @The velocity of this `SoundSource`. Only applicable if this object is played in 3D.
---@field position Vec3 @The position of this `SoundSource` in the scene. Only applicable if this `SoundSource` is played in 3D.
---@field effects boolean @Whether or not sound effects are enabled on playback. This flag must first be enabled to apply effects, as it is false by default. Sound effects are more resource-intensive.
---@field debug boolean @Show debug information about this object in the scene.
SoundSource = SoundSource or {}
--- A source of sound, whether that be for sound effects or music.
---@overload fun(path:string, type:Lime.Enum.SoundType?): SoundSource
---@return SoundSource
function SoundSource.new() end

---@class Text2D
---@field text string @The text content of this object.
---@field wordWrap boolean @Determines if the text wraps when touching the border of its text box.
---@field opacity number @The opacity of the text, from 0 to 255. For individual characters being not fully opaque, use color tags with an alpha value.
---@field position Vec2 @The 2D position of this object on the screen.
---@field size Vec2 @The 2D size of this object.
---@field visible boolean @Determines the visibility of this object and its children.
---@field border boolean @Displays a border outlining this object's bounding box on the screen.
---@field backgroundColor Vec4 @The RGBA background color of this object.
---@field enabled boolean @Determines if this object is hoverable and clickable.
---@field onHovered OnHoveredEvent @Event called by Lime when this object is hovered.
---@field onPressed OnPressedEvent @Event called by Lime when this object is pressed.
Text2D = Text2D or {}
--- A basic 2D object to display text. Text objects support colors and basic styling. Use tags `<#HEX>` for color, `<s>` for strike, `<d>` for drop shadow, `<u>` for underline, `<b>` for bold, and `<r>` to reset styles. Example: `<#6ABE30>This is green! <b>Now, it's green and bold! <r>Now, it's back to normal.`
---@overload fun(text:string): Text2D
---@overload fun(pos:Vec2, size:Vec2): Text2D
---@return Text2D
function Text2D.new() end

---@class Text3D
---@field text string @The text content of this object.
---@field opacity number @The opacity of the text, from 0 to 255. For individual characters being not fully opaque, use color tags with an alpha value.
---@field wordWrap boolean @Determines if the text wraps when touching the border of its text box.
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
---@operator add(Vec2): Vec2
---@operator sub(Vec2): Vec2
---@operator mul(number): Vec2
---@operator div(number): Vec2
---@operator eq(Vec2): boolean
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
---@operator add(Vec3): Vec3
---@operator sub(Vec3): Vec3
---@operator mul(number): Vec3
---@operator div(number): Vec3
---@operator eq(Vec3): boolean
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
---@operator add(Vec4): Vec4
---@operator sub(Vec4): Vec4
---@operator mul(number): Vec4
---@operator div(number): Vec4
---@operator eq(Vec4): boolean
Vec4 = Vec4 or {}
--- A four-dimensional vector object.
---@overload fun(x:number, y:number, z:number, w:number): Vec4
---@overload fun(all:number): Vec4
---@overload fun(HEX:string): Vec4
---@return Vec4
function Vec4.new() end

--- Clears this object's attributes.
---@return void
function Billboard:clearAttributes() end

--- Destroys this object.
---@return nil
function Billboard:destroy() end

--- Returns the content of attribute `key` from this object's attributes.
---@param key any
---@return any
function Billboard:getAttribute(key) end

--- Returns all attributes bundled in a table object.
---@return table
function Billboard:getAttributes() end

--- Returns the bounding box of this object, following: (MinEdgeX, MinEdgeY, MaxEdgeX, MaxEdgeY).
---@return Vec4
function Billboard:getBoundingBox() end

--- Returns the reference count for this object.
---@return number
function Billboard:getReferenceCount() end

--- Returns true if this object is parented to another 3D object.
---@return boolean
function Billboard:hasParent() end

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
---@return boolean
function Billboard:parentTo(parent) end

--- Sets `key` to `value` within this object's attributes.
---@param key any
---@param value any
---@return void
function Billboard:setAttribute(key, value) end

--- Clears this object's attributes.
---@return void
function Camera:clearAttributes() end

--- Destroys this object.
---@return nil
function Camera:destroy() end

--- Returns the content of attribute `key` from this object's attributes.
---@param key any
---@return any
function Camera:getAttribute(key) end

--- Returns all attributes bundled in a table object.
---@return table
function Camera:getAttributes() end

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

--- Returns true if this object is parented to another 3D object.
---@return boolean
function Camera:hasParent() end

--- Returns true if `pos` is inside this object's bounding box.
---@param pos Vec3
---@return boolean
function Camera:isPointInside(pos) end

--- Parents this object to another 3D object.
---@param parent any
---@return boolean
function Camera:parentTo(parent) end

--- When rendering, this `Camera` will take over as the active rendering viewpoint.
---@return void
function Camera:setActive() end

--- Sets `key` to `value` within this object's attributes.
---@param key any
---@param value any
---@return void
function Camera:setAttribute(key, value) end

--- Destroys this object.
---@return nil
function EditBox:destroy() end

--- Returns the reference count for this object.
---@return number
function EditBox:getReferenceCount() end

--- Returns true if this object is parented to another 2D object.
---@return boolean
function EditBox:hasParent() end

--- Returns true if this object is currently hovered.
---@return boolean
function EditBox:isHovered() end

--- Moves this object to the back in terms of z ordering. (Rendered first, all other objects then overlap)
---@return boolean
function EditBox:moveToBack() end

--- Moves this object to the front in terms of z ordering. (Rendered last, overlaps all other objects)
---@return boolean
function EditBox:moveToFront() end

--- Parents this object to another 2D object.
---@param parent any
---@return boolean
function EditBox:parentTo(parent) end

--- Sets the text's alignment within its bounding box.
---@overload fun(x:Lime.Enum.TextAlign, y:Lime.Enum.TextAlign): void
---@param all Lime.Enum.TextAlign
---@return void
function EditBox:setAlignment(all) end

--- Sets the font to use for this object. Fonts must first be loaded. See `Lime.GUI.loadXML` and `Lime.GUI.loadTTF`.
---@param name string
---@return boolean
function EditBox:setFont(name) end

--- Clears this object's attributes.
---@return void
function Empty:clearAttributes() end

--- Destroys this object.
---@return nil
function Empty:destroy() end

--- Returns the content of attribute `key` from this object's attributes.
---@param key any
---@return any
function Empty:getAttribute(key) end

--- Returns all attributes bundled in a table object.
---@return table
function Empty:getAttributes() end

--- Returns the bounding box of this object, following: (MinEdgeX, MinEdgeY, MaxEdgeX, MaxEdgeY).
---@return Vec4
function Empty:getBoundingBox() end

--- Returns the reference count for this object.
---@return number
function Empty:getReferenceCount() end

--- Returns true if this object is parented to another 3D object.
---@return boolean
function Empty:hasParent() end

--- Returns true if `pos` is inside this object's bounding box.
---@param pos Vec3
---@return boolean
function Empty:isPointInside(pos) end

--- Parents this object to another 3D object.
---@param parent any
---@return boolean
function Empty:parentTo(parent) end

--- Sets `key` to `value` within this object's attributes.
---@param key any
---@param value any
---@return void
function Empty:setAttribute(key, value) end

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

--- Returns true if this object is parented to another 2D object.
---@return boolean
function Image2D:hasParent() end

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
---@return boolean
function Image2D:parentTo(parent) end

--- Clears this object's attributes.
---@return void
function Light:clearAttributes() end

--- Destroys this object.
---@return nil
function Light:destroy() end

--- Returns the content of attribute `key` from this object's attributes.
---@param key any
---@return any
function Light:getAttribute(key) end

--- Returns all attributes bundled in a table object.
---@return table
function Light:getAttributes() end

--- Returns the bounding box of this object, following: (MinEdgeX, MinEdgeY, MaxEdgeX, MaxEdgeY).
---@return Vec4
function Light:getBoundingBox() end

--- Returns the reference count for this object.
---@return number
function Light:getReferenceCount() end

--- Returns true if this object is parented to another 3D object.
---@return boolean
function Light:hasParent() end

--- Returns true if `pos` is inside this object's bounding box.
---@param pos Vec3
---@return boolean
function Light:isPointInside(pos) end

--- Parents this object to another 3D object.
---@param parent any
---@return boolean
function Light:parentTo(parent) end

--- Sets `key` to `value` within this object's attributes.
---@param key any
---@param value any
---@return void
function Light:setAttribute(key, value) end

--- Clears the debug console's lines, only visually.
function Lime.clearDebugConsole() end

--- Closes the Lime application.
function Lime.close() end

--- Displays a pop-up message.
---@param title string
---@param message string
---@param icon Lime.Enum.PopUpIcon?
---@return void
function Lime.displayMessage(title, message, icon) end

--- Executes `cmd` in the system's command line.
---@param cmd string
---@return number
function Lime.executeCommandLine(cmd) end

--- Returns the value of `arg` from the commmand line, if any. Returns `nil` if no such argument exists.
---@param arg string
---@return string
function Lime.getCommandLineArg(arg) end

--- Returns the elapsed time the application has been running in milliseconds.
---@return number
function Lime.getElapsedTime() end

--- Returns the elapsed time the application has been running in seconds.
---@return number
function Lime.getElapsedTimeSeconds() end

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

--- Loads an archive of assets to the application. Content is accessed as if it were at the application's root. For example: If the archive contains folder/image.png, the path ./folder/image.png is valid for loading `Texture` objects.
---@param path string
---@return boolean
function Lime.loadArchive(path) end

--- Prints a message to console.
---@param msg any
---@param color Lime.Enum.PrintColor?
---@return void
function Lime.log(msg, color) end

--- **This function can only be run before window creation.**  
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

--- **This function can only be run before window creation.**  
--- This function sets initial parameters for the Lime application.
---@overload fun(driver:Lime.Enum.DriverType, windowSize:Vec2): boolean
---@overload fun(driver:Lime.Enum.DriverType, windowSize:Vec2, renderSize:Vec2): boolean
---@param driver Lime.Enum.DriverType
---@return boolean
function Lime.setInitConfig(driver) end

--- Sets vertical syncronization, matching the frame rate to the current monitor's refresh rate.
---@param vSyncOn boolean
---@return void
function Lime.setVSync(vSyncOn) end

--- Creates a directory.
---@param path string
---@return boolean
function Lime.File.createDirectory(path) end

--- Returns the extension of the file at `path`.
---@param path string
---@return string
function Lime.File.getExtension(path) end

--- Returns the name of the file at `path`.
---@param path string
---@return string
function Lime.File.getFileName(path) end

--- Returns a table of file paths from directory `path`.
---@param path string
---@return table
function Lime.File.getFilesInDirectory(path) end

--- Returns true if a directory exists at `path`.
---@param path string
---@return boolean
function Lime.File.isDirectory(path) end

--- Returns true if `path` leads to a file.
---@param path string
---@return boolean
function Lime.File.isFile(path) end

--- Returns contents of file at `path`.
---@param path string
---@return string
function Lime.File.readFile(path) end

--- Writes content to file at `path`. The file will be created if it does not exist.
---@param path string
---@return boolean
function Lime.File.writeFile(path) end

--- **This function cannot be run until window creation.**  
--- Returns true if the font `name` is loaded.
---@param name string
---@return boolean
function Lime.GUI.isFontLoaded(name) end

--- **This function cannot be run until window creation.**  
--- Loads a Truetype font. Provide `name` to set the name manually, otherwise Lime will register the font as fontname_size. Returns the output font name.
---@overload fun(path:string, fontSize:number, name:string, aa:boolean?): string
---@param path string
---@param fontSize number
---@param aa boolean?
---@return string
function Lime.GUI.loadTTF(path, fontSize, aa) end

--- **This function cannot be run until window creation.**  
--- Loads a bitmap font. Returns the name of this font, cut from `path`. (NOTE: `path` must be the path to a .xml file. The .xml files must be paired by an image file.)
---@param path string
---@return string
function Lime.GUI.loadXML(path) end

--- **This function cannot be run until window creation.**  
--- Sets the default font for new GUI elements to font `name`.
---@param name string
---@return void
function Lime.GUI.setDefaultFont(name) end

--- **This function cannot be run until window creation.**  
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

--- Determines if the mouse is confined to the window or not.
---@param confined boolean
---@return void
function Lime.Input.setMouseConfined(confined) end

--- Sets the mouse's position.
---@param pos Vec2
---@return void
function Lime.Input.setMousePosition(pos) end

--- Set the visibility of the mouse cursor.
---@param visible boolean
---@return void
function Lime.Input.setMouseVisible(visible) end

--- **This function cannot be run until window creation.**  
--- Returns the direction and magnitude of global gravity.
---@return Vec3
function Lime.Physics.getGravity() end

--- **This function cannot be run until window creation.**  
--- Returns the physics simulation step factor.
---@return number
function Lime.Physics.getStepFactor() end

--- **This function cannot be run until window creation.**  
--- Returns whether or not the scene's physics simulation is paused.
---@return boolean
function Lime.Physics.isPaused() end

--- **This function cannot be run until window creation.**  
--- Sets the physics simulation debug mode.
---@param type Lime.Enum.PhysicsDebugType
---@return void
function Lime.Physics.setDebug(type) end

--- **This function cannot be run until window creation.**  
--- Sets the direction and magnitude of global gravity.
---@param gravity Vec3
---@return void
function Lime.Physics.setGravity(gravity) end

--- **This function cannot be run until window creation.**  
--- Sets whether or not collisions with objects sharing the same ID should ignore one another.
---@param ignore boolean
---@return void
function Lime.Physics.setIgnoreIdenticalID(ignore) end

--- **This function cannot be run until window creation.**  
--- Sets whether or not the scene's physics simulation is paused.
---@param paused boolean
---@return void
function Lime.Physics.setPaused(paused) end

--- **This function cannot be run until window creation.**  
--- Sets the physics simulation step factor.
---@param factor number
---@return void
function Lime.Physics.setStepFactor(factor) end

--- **This function cannot be run until window creation.**  
--- Clears the `Shader` applied to the screen, if any.
---@return void
function Lime.Scene.clearPostProcessingShader() end

--- **This function cannot be run until window creation.**  
--- Returns a `Mesh` containing a cube.
---@param size Vec3
---@return Mesh
function Lime.Scene.createCubeMesh(size) end

--- **This function cannot be run until window creation.**  
--- Returns a `Mesh` containing a cylinder.
---@overload fun(radius:number, length:number, polyCount:number, closed:boolean): Mesh
---@param radius number
---@param length number
---@return Mesh
function Lime.Scene.createCylinderMesh(radius, length) end

--- **This function cannot be run until window creation.**  
--- Returns a `Mesh` containing a plane. Parameter `repeatCount` controls how much an applied `Texture` will repeat within one tile.
---@overload fun(tileSize:Vec2, tileCount:Vec2, repeatCount:Vec2): Mesh
---@param tileSize Vec2
---@param tileCount Vec2
---@return Mesh
function Lime.Scene.createPlaneMesh(tileSize, tileCount) end

--- **This function cannot be run until window creation.**  
--- Returns a `Mesh` containing a sphere.
---@overload fun(radius:number, polyCount:number): Mesh
---@param radius number
---@return Mesh
function Lime.Scene.createSphereMesh(radius) end

--- **This function cannot be run until window creation.**  
--- Fires a raycast out into the scene from `startPos` to `endPos`. Only objects with collision enabled will be tested.
---@param startPos Vec3
---@param endPos Vec3
---@param rayLifeMs number?
---@return HitResult
function Lime.Scene.fireRaycast(startPos, endPos, rayLifeMs) end

--- **This function cannot be run until window creation.**  
--- Fires a raycast out from a screenspace position `Vec2` of length `length`. Only objects with collision enabled will be tested.
---@param startPos Vec2
---@param length number
---@param rayLifeMs number?
---@return HitResult
function Lime.Scene.fireScreenspaceRaycast(startPos, length, rayLifeMs) end

--- **This function cannot be run until window creation.**  
--- Returns an `Texture` of a lime and white checkerboard pattern, 2x2. Useful for missing Textures and the like.
---@return Texture
function Lime.Scene.getErrorTexture() end

--- Returns the amount of 3D objects in the scene.
---@return number
function Lime.Scene.getObjectCount() end

--- Returns whether or not the application is actively rendering new output from the scene.
---@return boolean
function Lime.Scene.isRenderingActive() end

--- **This function cannot be run until window creation.**  
--- Preloads a mesh by `path` into the scene for later use. Returns true on success or if the mesh at `path` has already been preloaded.
---@param path string
---@return boolean
function Lime.Scene.preloadMesh(path) end

--- **This function cannot be run until window creation.**  
--- Preloads a texture by `path` into the scene for later use. Returns true on success or if the texture at `path` has already been preloaded.
---@param path string
---@return boolean
function Lime.Scene.preloadTexture(path) end

--- **This function cannot be run until window creation.**  
--- Purges a mesh by `path` from the scene. Returns true on success or if the mesh at `path` has already been purged. `Mesh` objects referencing this mesh will use an engine-defined `Mesh` instead.
---@param path string
---@return boolean
function Lime.Scene.purgeMesh(path) end

--- **This function cannot be run until window creation.**  
--- Purges a texture by `path` from the scene. Returns true on success or if the texture at `path` has already been purged. `Texture` objects referencing this texture will use an engine-defined `Texture` instead.
---@param path string
---@return boolean
function Lime.Scene.purgeTexture(path) end

--- **This function cannot be run until window creation.**  
--- Sets the ambient color of the scene to `rgba`.
---@param rgba Vec4
---@return void
function Lime.Scene.setAmbientColor(rgba) end

--- **This function cannot be run until window creation.**  
--- Sets the background color of the scene to `rgba`. This color is generally only visible when there is no `Skydome`.
---@param rgba Vec4
---@return void
function Lime.Scene.setBackgroundColor(rgba) end

--- **This function cannot be run until window creation.**  
--- Sets the color of the scene's fog to `rgba`.
---@param rgba Vec4
---@return void
function Lime.Scene.setFogColor(rgba) end

--- **This function cannot be run until window creation.**  
--- Sets where the scene's fog starts and ends.
---@param planes Vec2
---@return void
function Lime.Scene.setFogPlanes(planes) end

--- **This function cannot be run until window creation.**  
--- Sets the light management behavior using `Lime.Enum.LightManagementType`.
---@param type Lime.Enum.LightManagementType
---@return void
function Lime.Scene.setLightManagementType(type) end

--- **This function cannot be run until window creation.**  
--- Passes a `Shader` to the renderer be used for special effects on the scene output.
---@param shader Shader
---@return void
function Lime.Scene.setPostProcessingShader(shader) end

--- Sets the render quality of the scene using `Lime.Enum.Quality`.
---@param quality Lime.Enum.Quality
---@return void
function Lime.Scene.setRenderQuality(quality) end

--- If set to false, the application will not update the scene output. A use case is a pseudo-pause your game to save on resources while loading assets.
---@param active boolean
---@return void
function Lime.Scene.setRenderingActive(active) end

--- If set to true, the rendering resolution will be that of the window resolution, updating on any window size changes. If false, the rendering resolution will remain the same size but upscale without anti-aliasing to the window resolution.
---@param doRescale boolean
---@return void
function Lime.Scene.setRescaleRenderToWindowSize(doRescale) end

--- **This function cannot be run until window creation.**  
--- Sets the color of shadows in the scene to `rgba`.
---@param rgba Vec4
---@return void
function Lime.Scene.setShadowColor(rgba) end

--- Sets the render size to `size`. If the render size is set to rescale to the window size then this change will not take effect. See `Lime.Scene.setRescaleRenderToWindowSize`.
---@param size Vec2
---@return void
function Lime.Scene.setSize(size) end

--- **This function cannot be run until window creation.**  
--- Sets the default `Texture` creation quality using `Lime.Enum.TextureCreationQuality`, where Low is optimized for speed and High is optimized for quality.
---@param quality Lime.Enum.TextureCreationQuality
---@return void
function Lime.Scene.setTextureCreationQuality(quality) end

--- **This function cannot be run until window creation.**  
--- Converts a 3D position to `Vec2` on the screen.
---@param pos Vec3
---@return Vec2
function Lime.Scene.toScreenPosition(pos) end

--- Returns the application's main volume.
---@return number
function Lime.Sound.getMainVolume() end

--- Returns the listener velocity factor.
---@return number
function Lime.Sound.getVelocityFactor() end

--- Sets all `Sound` objects to paused or unpaused.
---@param paused boolean
---@return void
function Lime.Sound.setAllSoundsPaused(paused) end

--- Sets the default maximum listening distance for new sounds.
---@param distance number
---@return void
function Lime.Sound.setDefaultMaximumDistance(distance) end

--- Sets the default minimum listening distance for new sounds.
---@param distance number
---@return void
function Lime.Sound.setDefaultMinimumDistance(distance) end

--- Sets the application's main volume.
---@param volume number
---@return void
function Lime.Sound.setMainVolume(volume) end

--- Determines unfocused window behavior in regards to application volume.
---@param mute boolean
---@return void
function Lime.Sound.setMuteWhileUnfocused(mute) end

--- Sets the factor to scale listener velocity by. Listener velocity is calculated with the last and current active `Camera` positions. In the event that the active `Camera` is swapped, listener velocity will be 0 on the first frame automatically.
---@param factor number
---@return void
function Lime.Sound.setVelocityFactor(factor) end

--- **This function cannot be run until window creation.**  
--- Returns the size of the monitor the window is running on.
---@return Vec2
function Lime.Window.getMonitorSize() end

--- **This function cannot be run until window creation.**  
--- Returns the window's position.
---@return Vec2
function Lime.Window.getPosition() end

--- **This function cannot be run until window creation.**  
--- Returns the size of the window.
---@return Vec2
function Lime.Window.getSize() end

--- **This function cannot be run until window creation.**  
--- Returns true if the window is focused.
---@return boolean
function Lime.Window.isFocused() end

--- **This function cannot be run until window creation.**  
--- Toggles fullscreen mode.
---@param fullscreen boolean
---@return void
function Lime.Window.setFullscreen(fullscreen) end

--- **This function cannot be run until window creation.**  
--- Locks the aspect ratio of the raw window size. It will not make the window immune to all resizing. Maximizing the window will not preserve the rendering aspect ratio.
---@param locked boolean
---@return void
function Lime.Window.setLockAspectRatio(locked) end

--- **This function cannot be run until window creation.**  
--- Sets the window's position to `pos`.
---@param pos Vec2
---@return void
function Lime.Window.setPosition(pos) end

--- **This function cannot be run until window creation.**  
--- Allows the window to be resizable or locked to its intended size. This disables maximizing the application as well.
---@param allow boolean
---@return void
function Lime.Window.setResizable(allow) end

--- **This function cannot be run until window creation.**  
--- Sets the window's size to `size`.
---@param size Vec2
---@return void
function Lime.Window.setSize(size) end

--- **This function cannot be run until window creation.**  
--- Sets the window's title to `title`.
---@param title string
---@return void
function Lime.Window.setTitle(title) end

--- Clears the `Shader` applied to this `Material`, if any.
---@return void
function Material:clearShader() end

--- Clears the `Texture` in this `Material`.
---@param layer number?
---@return void
function Material:clearTexture(layer) end

--- Loads a `Shader` into this `Material`.
---@param shader Shader
---@return void
function Material:loadShader(shader) end

--- Loads a `Texture` into this `Material`.
---@overload fun(texture:Texture, layer:number): void
---@param texture Texture
---@return void
function Material:loadTexture(texture) end

--- Sets the scale of the mapping of an `Texture`.
---@overload fun(scale:Vec2, layer:number): void
---@param scale Vec2
---@return void
function Material:setTextureScale(scale) end

--- Changes the method for `Texture` UV wrapping.
---@overload fun(uMethod:Lime.Enum.TextureWrapType, vMethod:Lime.Enum.TextureWrapType, layer:number): void
---@param uMethod Lime.Enum.TextureWrapType
---@param vMethod Lime.Enum.TextureWrapType
---@return void
function Material:setTextureWrapMethod(uMethod, vMethod) end

--- Clears the `MeshBuffer` from within this `Mesh`. This will not remove its `MeshBuffer` from memory.
---@return void
function Mesh:clear() end

--- Clears this object's attributes.
---@return void
function Mesh:clearAttributes() end

--- Destroys this object.
---@return nil
function Mesh:destroy() end

--- Returns the content of attribute `key` from this object's attributes.
---@param key any
---@return any
function Mesh:getAttribute(key) end

--- Returns all attributes bundled in a table object.
---@return table
function Mesh:getAttributes() end

--- Returns the bounding box of this object, following: (MinEdgeX, MinEdgeY, MaxEdgeX, MaxEdgeY).
---@return Vec4
function Mesh:getBoundingBox() end

--- Returns the number of animation frames.
---@return number
function Mesh:getFrameCount() end

--- Returns the number of `Material` slots in this `Mesh`.
---@return number
function Mesh:getMaterialCount() end

--- Returns the reference count for this object.
---@return number
function Mesh:getReferenceCount() end

--- Returns the vertex count of this `Mesh`.
---@return number
function Mesh:getVertexCount() end

--- Returns true if this object is parented to another 3D object.
---@return boolean
function Mesh:hasParent() end

--- Returns true if `pos` is inside this object's bounding box.
---@param pos Vec3
---@return boolean
function Mesh:isPointInside(pos) end

--- Loads a `Material` into this `Mesh`.
---@overload fun(material:Material, layer:number): boolean
---@param material Material
---@return boolean
function Mesh:loadMaterial(material) end

--- Loads a 3D model into this `Mesh`.
---@overload fun(buffer:MeshBuffer): boolean
---@param path string
---@return boolean
function Mesh:loadMesh(path) end

--- Parents this object to another 3D object.
---@param parent any
---@return boolean
function Mesh:parentTo(parent) end

--- Purges this `Mesh`, effectively removing its mesh buffer from memory. Objects using this `Mesh`'s `MeshBuffer` will use an engine-defined `Mesh` instead, but it is recommended to remove references to this `Mesh` first.
---@return nil
function Mesh:purge() end

--- Recalculates the bounding box of this `Mesh`. This is useful for potential loading errors, where this `Mesh` stops rendering even if it is within the active `Camera`'s view.
---@return void
function Mesh:recalculateBoundingBox() end

--- Sets `key` to `value` within this object's attributes.
---@param key any
---@param value any
---@return void
function Mesh:setAttribute(key, value) end

--- Informs the graphics system of how this `Mesh` should be stored. By default, `Mesh` objects use Static. Use Dynamic (or more intensely, Stream) if the `Mesh` is updated frequently.
---@param hint Lime.Enum.StorageHint
---@return void
function Mesh:setStorageHint(hint) end

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

--- Returns the value at `x`, `y`, `z` in this `Noise` object.
---@overload fun(x:number, y:number): number
---@overload fun(x:number, y:number, z:number): number
---@param x number
---@return number
function Noise:get(x) end

--- Applies a continous force to this `RigidBody`, where `pos` is in world space.
---@param force number
---@param pos Vec3
---@return void
function RigidBody:applyForce(force, pos) end

--- Applies a continous force to this `RigidBody`, where `pos` is relative to the center of this `RigidBody`.
---@param force Vec3
---@param pos Vec3
---@return void
function RigidBody:applyForceRelative(force, pos) end

--- Applies an impulse to this `RigidBody`, where `pos` is in world space.
---@param force number
---@param pos Vec3
---@return void
function RigidBody:applyImpulse(force, pos) end

--- Applies an impulse to this `RigidBody`, where `pos` is relative to the center of this `RigidBody`.
---@param impulse Vec3
---@param pos Vec3
---@return void
function RigidBody:applyImpulseRelative(impulse, pos) end

--- Applies a torque to this `RigidBody`, where `pos` is in world space.
---@param force Vec3
---@param impulse bool
---@return void
function RigidBody:applyTorque(force, impulse) end

--- Clears all forces applied to this `RigidBody`.
---@return void
function RigidBody:clearForces() end

--- Destroys this object.
---@return nil
function RigidBody:destroy() end

--- Returns the center of mass position of this `RigidBody` in world space.
---@return Vec3
function RigidBody:getCenterOfMass() end

--- Returns the forward vector of this `RigidBody`.
---@return Vec3
function RigidBody:getForward() end

--- Loads a new visual `Mesh` into this `RigidBody`.
---@return boolean
function RigidBody:loadMesh() end

--- Applies a preset to this `RigidBody` that alters its physical properties.
---@param preset Lime.Enum.PhysicalPreset
---@return void
function RigidBody:loadPreset(preset) end

--- Rotates this `RigidBody` to look at a position in world space.
---@param pos Vec3
---@return void
function RigidBody:lookAt(pos) end

--- Returns the path to the pixel shader file loaded in this `Shader`.
---@return string
function Shader:getPSPath() end

--- Returns the path to the vertex shader file loaded in this `Shader`.
---@return string
function Shader:getVSPath() end

--- Sets a uniform shader parameter within this `Shader`.
---@overload fun(name:string, value:Vec2): void
---@overload fun(name:string, value:Vec3): void
---@overload fun(name:string, value:Vec4): void
---@param name string
---@param value number
---@return void
function Shader:setParameter(name, value) end

--- Clears this object's attributes.
---@return void
function Skydome:clearAttributes() end

--- Destroys this object.
---@return nil
function Skydome:destroy() end

--- Returns the content of attribute `key` from this object's attributes.
---@param key any
---@return any
function Skydome:getAttribute(key) end

--- Returns all attributes bundled in a table object.
---@return table
function Skydome:getAttributes() end

--- Returns the bounding box of this object, following: (MinEdgeX, MinEdgeY, MaxEdgeX, MaxEdgeY).
---@return Vec4
function Skydome:getBoundingBox() end

--- Returns the reference count for this object.
---@return number
function Skydome:getReferenceCount() end

--- Returns true if this object is parented to another 3D object.
---@return boolean
function Skydome:hasParent() end

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
---@return boolean
function Skydome:parentTo(parent) end

--- Sets `key` to `value` within this object's attributes.
---@param key any
---@param value any
---@return void
function Skydome:setAttribute(key, value) end

--- Enables compression on this `SoundSource`. Only applicable if this `SoundSource` is playing. This effect reduces the dynamic range of the sound's waveform.
--- Returns bool
---@overload fun(threshold:number, ratio:number)
function SoundSource:addCompressionEffect() end

--- Enables distortion on this `SoundSource`. Only applicable if this `SoundSource` is playing. This effect messes with the sound's frequency and other attributes to produce an odd result.
--- Returns bool
---@overload fun(gain:number, edge:number)
function SoundSource:addDistortionEffect() end

--- Enables echoing on this `SoundSource`. Only applicable if this `SoundSource` is playing. This effect repeats the sound with decay over time.
--- Returns bool
---@overload fun(wetDry:number, feedback:number, delayMs:number)
function SoundSource:addEchoEffect() end

--- Enables parametric equilization on this `SoundSource`. Only applicable if this `SoundSource` is playing. This effect amplifies or attenuates signals at a given frequency.
--- Returns bool
---@overload fun(threshold:number, ratio:number)
function SoundSource:addParamEqEffect() end

--- Enables reverb on this `SoundSource`. Only applicable if this `SoundSource` is playing. This effect mixes the sound to bounce off surfaces in a room or a cave.
--- Returns bool
---@overload fun(inputGain:number, mix:number, timeMs:number, freqRatio:number)
function SoundSource:addReverbEffect() end

--- Clears all effects applied to this `SoundSource`. Stopping or destroying this `SoundSource` will clear its effects.
---@return void
function SoundSource:clearEffects() end

--- Destroys this `SoundSource`, which stops itself from playing in the scene as well as detaching from a parent 3D object. To free this sound from memory, see `SoundSource:purge`.
---@return nil
function SoundSource:destroy() end

--- Returns the playback length of this `SoundSource`.
---@return number
function SoundSource:getLength() end

--- Returns the file path of the sound loaded into this `SoundSource`.
---@return string
function SoundSource:getPath() end

--- Returns true if this `SoundSource` is parented to a 3D object.
---@return boolean
function SoundSource:hasParent() end

--- Returns true if this `SoundSource` is playing.
---@return boolean
function SoundSource:isPlaying() end

--- Loads a new sound into this `SoundSource`. (WARNING: Unused sounds should be purged to free up unused memory)
---@param path string
---@param type Lime.Enum.SoundType?
---@return boolean
function SoundSource:load(path, type) end

---@overload fun(parent:any): boolean
---@param this Parents
---@return boolean
function SoundSource:parentTo(this) end

--- Play this `SoundSource`.
---@param is3D boolean?
---@return boolean
function SoundSource:play(is3D) end

--- Purges this `SoundSource`, effectively removing it from memory. If other `SoundSource` objects use this sound, there may be issues.
---@return nil
function SoundSource:purge() end

--- Stop this `SoundSource`.
---@return void
function SoundSource:stop() end

--- Destroys this object.
---@return nil
function Text2D:destroy() end

--- Returns the reference count for this object.
---@return number
function Text2D:getReferenceCount() end

--- Returns true if this object is parented to another 2D object.
---@return boolean
function Text2D:hasParent() end

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
---@return boolean
function Text2D:parentTo(parent) end

--- Sets the text's alignment within its bounding box.
---@overload fun(x:Lime.Enum.TextAlign, y:Lime.Enum.TextAlign): void
---@param all Lime.Enum.TextAlign
---@return void
function Text2D:setAlignment(all) end

--- Sets the font to use for this object. Fonts must first be loaded. See `Lime.GUI.loadXML` and `Lime.GUI.loadTTF`.
---@param name string
---@return boolean
function Text2D:setFont(name) end

--- Enables word wrap.
---@param wrap boolean
---@return void
function Text2D:setWordWrap(wrap) end

--- Clears this object's attributes.
---@return void
function Text3D:clearAttributes() end

--- Destroys this object.
---@return nil
function Text3D:destroy() end

--- Returns the content of attribute `key` from this object's attributes.
---@param key any
---@return any
function Text3D:getAttribute(key) end

--- Returns all attributes bundled in a table object.
---@return table
function Text3D:getAttributes() end

--- Returns the bounding box of this object, following: (MinEdgeX, MinEdgeY, MaxEdgeX, MaxEdgeY).
---@return Vec4
function Text3D:getBoundingBox() end

--- Returns the reference count for this object.
---@return number
function Text3D:getReferenceCount() end

--- Returns true if this object is parented to another 3D object.
---@return boolean
function Text3D:hasParent() end

--- Returns true if `pos` is inside this object's bounding box.
---@param pos Vec3
---@return boolean
function Text3D:isPointInside(pos) end

--- Parents this object to another 3D object.
---@param parent any
---@return boolean
function Text3D:parentTo(parent) end

--- Sets the text's alignment within its bounding box.
---@overload fun(x:Lime.Enum.TextAlign, y:Lime.Enum.TextAlign): void
---@param all Lime.Enum.TextAlign
---@return void
function Text3D:setAlignment(all) end

--- Sets `key` to `value` within this object's attributes.
---@param key any
---@param value any
---@return void
function Text3D:setAttribute(key, value) end

--- Sets the font to use for this object. Fonts must first be loaded. See `Lime.GUI.loadXML` and `Lime.GUI.loadTTF`.
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
---@return boolean
function Texture:append(toAppend, pos) end

--- Crops the `Texture` to the dimensions provided. Be mindful as cropping creates a new `Texture` in the renderer.
---@param topLeft Vec2
---@param bottomRight Vec2
---@return boolean
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
---@return boolean
function Texture:keyColor(keyColor) end

--- Purges this `Texture`, effectively removing it from memory. Objects using this `Texture` will use an engine-defined `Texture` instead, but it is recommended to remove references to this `Texture` first.
---@return nil
function Texture:purge() end

--- Renders the scene to this `Texture`.
---@overload fun(size:Vec2, viewpoint:Camera): void
---@param size Vec2
---@return void
function Texture:renderToTexture(size) end

--- Replaces the pixel at `pos` with a pixel of color `color`.
---@param pos Vec2
---@param color Vec4
---@return boolean
function Texture:setColor(pos, color) end

--- Writes the `Texture` to path `path`.
---@param path string
---@return boolean
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
