---@class Lime
Lime = Lime or {}
---@class Lime.Enum
Lime.Enum = Lime.Enum or {}
---@enum Lime.Enum.PrintColor
Lime.Enum.PrintColor = {
White = 0,
Red = 1,
Green = 2,
Blue = 3,
Yellow = 4,
}
---@enum Lime.Enum.PopUpIcon
Lime.Enum.PopUpIcon = {
None = 0,
Warning = 1,
Message = 2,
Information = 3,
}
---@enum Lime.Enum.MouseType
Lime.Enum.MouseType = {
Normal = 0,
Confined = 1,
Raw = 2,
}
---@enum Lime.Enum.PeerState
Lime.Enum.PeerState = {
Disconnected = 0,
Connecting = 1,
AcknowledgingConnect = 2,
ConnectionPending = 3,
ConnectionSucceeded = 4,
Connected = 5,
DisconnectLater = 6,
Disconnecting = 7,
AcknowledgingDisconnect = 8,
Zombie = 9,
}
---@enum Lime.Enum.PhysicalPreset
Lime.Enum.PhysicalPreset = {
Default = 0,
Static = 1,
Bouncy = 2,
Icy = 3,
Heavy = 4,
Floaty = 5,
}
---@enum Lime.Enum.Cursor
Lime.Enum.Cursor = {
Normal = 0,
Cross = 1,
Hand = 2,
Help = 3,
Highlighter = 4,
Forbidden = 5,
Hourglass = 6,
ResizeAll = 7,
ResizeNS = 8,
ResizeWE = 9,
ResizeNESW = 10,
ResizeNWSE = 11,
Up = 12,
}
---@enum Lime.Enum.DriverType
Lime.Enum.DriverType = {
None = 0,
Software = 1,
SoftwarePlus = 2,
Direct3D8 = 3,
Direct3D9 = 4,
OpenGL = 5,
}
---@enum Lime.Enum.PhysicsDebugType
Lime.Enum.PhysicsDebugType = {
None = 0,
BoundingBoxes = 1,
ContactPoints = 2,
Wireframe = 3,
Constraints = 4,
All = 5,
}
---@enum Lime.Enum.MaterialType
Lime.Enum.MaterialType = {
Solid = 0,
SolidMultiply = 1,
Add = 12,
AlphaSoft = 13,
AlphaHarsh = 14,
VertexAlpha = 15,
SphereMap = 10,
ReflectMap = 11,
LightMap = 2,
LightMapAdd = 3,
LightMapBright = 4,
LightMapBrightest = 5,
DynamicLightMap = 6,
DynamicLightMapBright = 7,
DynamicLightMapBrightest = 8,
DetailMap = 9,
}
---@enum Lime.Enum.EmitterType
Lime.Enum.EmitterType = {
Point = 0,
Box = 1,
Sphere = 2,
Ring = 3,
Cylinder = 4,
}
---@enum Lime.Enum.LightType
Lime.Enum.LightType = {
Point = 0,
Spotlight = 1,
Directional = 2,
}
---@enum Lime.Enum.TextureWrapType
Lime.Enum.TextureWrapType = {
Repeat = 0,
Clamp = 1,
ClampToEdge = 2,
Mirror = 4,
MirrorClamp = 5,
MirrorClampToEdge = 6,
}
---@enum Lime.Enum.Quality
Lime.Enum.Quality = {
Low = 0,
Medium = 1,
High = 2,
}
---@enum Lime.Enum.TextureCreationQuality
Lime.Enum.TextureCreationQuality = {
Low = 0,
Medium = 1,
High = 2,
}
---@enum Lime.Enum.LightManagementType
Lime.Enum.LightManagementType = {
EightNearestToCamera = 0,
ThreeClosestToObjects = 1,
EmptyAsActivationZone = 2,
}
---@enum Lime.Enum.ZOrderMethod
Lime.Enum.ZOrderMethod = {
BehindAll = 6,
Default = 1,
Decal = 2,
ExceptDecal = 3,
XRay = 4,
StrictXRay = 5,
OnTop = 0,
}
---@enum Lime.Enum.TextAlign
Lime.Enum.TextAlign = {
Left = 0,
Right = 1,
Center = 2,
Top = 0,
Bottom = 1,
}
---@enum Lime.Enum.StorageHint
Lime.Enum.StorageHint = {
Never = 0,
Static = 1,
Dynamic = 2,
Stream = 3,
}
---@enum Lime.Enum.SoundType
Lime.Enum.SoundType = {
Auto = 0,
Stream = 1,
Static = 2,
}
---@enum Lime.Enum.Mouse
Lime.Enum.Mouse = {
Left = 0,
Right = 1,
Middle = 2,
}
---@enum Lime.Enum.ControllerButton
Lime.Enum.ControllerButton = {
A = 0,
B = 1,
X = 2,
Y = 3,
RightBumper = 4,
LeftBumper = 5,
Back = 6,
Start = 7,
RightStick = 8,
LeftStick = 9,
DPadUp = 32,
DPadRight = 33,
DPadDown = 34,
DPadLeft = 35,
}
---@enum Lime.Enum.ControllerAxis
Lime.Enum.ControllerAxis = {
LeftStickX = 0,
LeftStickY = 1,
RightStickX = 3,
RightStickY = 4,
LeftTrigger = 5,
RightTrigger = 6,
}
---@enum Lime.Enum.Key
Lime.Enum.Key = {
Back = 8,
Tab = 9,
Return = 13,
Shift = 16,
Control = 17,
Alt = 18,
Pause = 19,
CapsLock = 20,
Escape = 27,
Space = 32,
PageUp = 33,
PageDown = 34,
End = 35,
Home = 36,
Left = 37,
Up = 38,
Right = 39,
Down = 40,
PrintScreen = 44,
Insert = 45,
Delete = 46,
Num0 = 48,
Num1 = 49,
Num2 = 50,
Num3 = 51,
Num4 = 52,
Num5 = 53,
Num6 = 54,
Num7 = 55,
Num8 = 56,
Num9 = 57,
A = 65,
B = 66,
C = 67,
D = 68,
E = 69,
F = 70,
G = 71,
H = 72,
I = 73,
J = 74,
K = 75,
L = 76,
M = 77,
N = 78,
O = 79,
P = 80,
Q = 81,
R = 82,
S = 83,
T = 84,
U = 85,
V = 86,
W = 87,
X = 88,
Y = 89,
Z = 90,
LWin = 91,
RWin = 92,
Numpad0 = 96,
Numpad1 = 97,
Numpad2 = 98,
Numpad3 = 99,
Numpad4 = 100,
Numpad5 = 101,
Numpad6 = 102,
Numpad7 = 103,
Numpad8 = 104,
Numpad9 = 105,
NumpadMultiply = 106,
NumpadAdd = 107,
NumpadSeparator = 108,
NumpadSubtract = 109,
NumpadDecimal = 110,
NumpadDivide = 111,
F1 = 112,
F2 = 113,
F3 = 114,
F4 = 115,
F5 = 116,
F6 = 117,
F7 = 118,
F8 = 119,
F9 = 120,
F10 = 121,
F11 = 122,
F12 = 123,
NumLock = 144,
ScrollLock = 145,
LShift = 160,
RShift = 161,
LControl = 162,
RControl = 163,
LAlt = 164,
RAlt = 165,
Semicolon = 186,
Equals = 187,
Comma = 188,
Minus = 189,
Period = 190,
Slash = 191,
Backtick = 192,
LeftBracket = 219,
Backslash = 220,
RightBracket = 221,
Apostrophe = 222,
Oem8 = 223,
Oem102 = 226,
OemClear = 254,
}