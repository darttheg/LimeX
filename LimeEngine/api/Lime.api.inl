LIME_MODULE(Lime)

LIME_DOC("Prints a message to console.");
LIME_FUNC(void, Log,
    string msg,
    PrintColor? color
)

LIME_DOC("Displays a message above the application.");
LIME_FUNC(void, DisplayMessage,
    string msg,
    string? title,
    PopUpIcon? icon
)

LIME_DOC("End application if an error occurs?");
LIME_FUNC(void, SetEndOnError,
    bool endOnError
)

LIME_DOC("Enables the debugger console.");
LIME_FUNC(void, SetShowDebugger,
    bool show
)

LIME_DOC("Write the debugger output to the project directory.");
LIME_FUNC(void, SetWriteDebugOutput,
    bool output
)

LIME_DOC("Closes the application.");
LIME_FUNC(void, Close
)

LIME_DOC("Retrieves the running Lime version.");
LIME_FUNC(string, GetVersion
)

LIME_DOC("Get the elapsed time the application has been running for.");
LIME_FUNC(int, GetTime
)

LIME_DOC("Returns the frame rate.");
LIME_FUNC(int, GetFrameRate)

LIME_DOC("Returns the current delta time.");
LIME_FUNC(float, GetDeltaTime
)

LIME_DOC("Sets the frame limit.");
LIME_FUNC(void, SetFrameLimit,
    int limit
)

LIME_DOC("Sets vertical syncronization.");
LIME_FUNC(void, SetVSync,
    bool enable
)

LIME_DOC("Returns the amount of memory being used, in megabytes.");
LIME_FUNC(int, GetMemoryUsage)