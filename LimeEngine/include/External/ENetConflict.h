#pragma once

// ENET has a conflict with windows.h and its winsock usage.

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>