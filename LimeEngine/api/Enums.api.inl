LIME_MODULE(Lime.Enum)

LIME_ENUM(PrintColor,
    LIME_ENUM_VALUE(White, 0)
    LIME_ENUM_VALUE(Red, 1)
    LIME_ENUM_VALUE(Green, 2)
    LIME_ENUM_VALUE(Blue, 3)
    LIME_ENUM_VALUE(Yellow, 4)
);

LIME_ENUM(PopUpIcon,
    LIME_ENUM_VALUE(None, 0)
    LIME_ENUM_VALUE(Warning, 1)
    LIME_ENUM_VALUE(Message, 2)
    LIME_ENUM_VALUE(Information, 3)
);

LIME_ENUM(DriverType,
    LIME_ENUM_VALUE(None, 0)
    LIME_ENUM_VALUE(Software, 1)
    LIME_ENUM_VALUE(SoftwarePlus, 2)
    LIME_ENUM_VALUE(Direct3D8, 3)
    LIME_ENUM_VALUE(Direct3D9, 4)
    LIME_ENUM_VALUE(OpenGL, 5)
);

LIME_ENUM(MaterialType,
    LIME_ENUM_VALUE(Solid, 0)
    LIME_ENUM_VALUE(SolidMultiplyLayers, 1)
    LIME_ENUM_VALUE(Add, 12)
    LIME_ENUM_VALUE(AlphaSoft, 13)
    LIME_ENUM_VALUE(AlphaHarsh, 14)
    LIME_ENUM_VALUE(SphereMap, 10)
    LIME_ENUM_VALUE(ReflectMap, 11)
);

LIME_ENUM(ImageWrapType,
    LIME_ENUM_VALUE(Repeat, 0)
    LIME_ENUM_VALUE(Clamp, 1)
    LIME_ENUM_VALUE(ClampToEdge, 2)
    LIME_ENUM_VALUE(Mirror, 4)
    LIME_ENUM_VALUE(MirrorClamp, 5)
    LIME_ENUM_VALUE(MirrorClampToEdge, 6)
);

LIME_ENUM(MaterialQuality,
    LIME_ENUM_VALUE(Low, 0)
    LIME_ENUM_VALUE(Medium, 1)
    LIME_ENUM_VALUE(High, 2)
    LIME_ENUM_VALUE(Ultra, 3)
);

LIME_ENUM(ZOrderMethod,
    LIME_ENUM_VALUE(BehindAll, 0)
    LIME_ENUM_VALUE(Default, 1)
    LIME_ENUM_VALUE(Decal, 2)
    LIME_ENUM_VALUE(ExceptDecal, 3)
    LIME_ENUM_VALUE(XRay, 4)
    LIME_ENUM_VALUE(StrictXRay, 5)
    LIME_ENUM_VALUE(OnTop, 6)
);