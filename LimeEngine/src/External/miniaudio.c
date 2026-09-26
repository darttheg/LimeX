// stb_vorbis gives miniaudio OGG support
#define STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#undef STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"

#include "extras/nodes/ma_reverb_node/ma_reverb_node.c"