LIME_MODULE(Lime)

LIME_DOC("Prints a message to console.");
LIME_FUNC(void, Log,
    string msg,
    PrintColor? color
)

LIME_DOC("End application if an error occurs?");
LIME_FUNC(void, SetEndOnError,
    bool endOnError
)

LIME_DOC("Closes the application.");
LIME_FUNC(void, Close
)

LIME_DOC("Retrieves the running Lime version.");
LIME_FUNC(string, GetVersion
)