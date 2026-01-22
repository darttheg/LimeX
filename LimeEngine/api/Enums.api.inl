LIME_MODULE(Lime)

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