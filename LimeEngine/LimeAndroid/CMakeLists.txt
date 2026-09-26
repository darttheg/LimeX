# Builds liblime.so for one ABI. Run through buildNative.ps1.
cmake_minimum_required(VERSION 3.22)
project(LimeAndroid C CXX)

if(NOT ANDROID)
	message(FATAL_ERROR "Configure LimeAndroid with the Android NDK toolchain (see buildNative.ps1).")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

get_filename_component(ENGINE ${CMAKE_CURRENT_LIST_DIR}/.. ABSOLUTE)
get_filename_component(LIME_ROOT ${ENGINE}/.. ABSOLUTE)
get_filename_component(DEFAULT_DEPS ${LIME_ROOT}/.. ABSOLUTE)
set(LIME_DEPS_DIR ${DEFAULT_DEPS} CACHE PATH "Folder with the third-party SDKs (same as LimeDepsDir)")
set(DEPS ${LIME_DEPS_DIR})
set(GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)

set(QUIET_WARNINGS -w)

# Lua
file(GLOB LUA_SOURCES ${DEPS}/lua-5.4.8/src/*.c)
list(REMOVE_ITEM LUA_SOURCES ${DEPS}/lua-5.4.8/src/lua.c ${DEPS}/lua-5.4.8/src/luac.c)
add_library(lua STATIC ${LUA_SOURCES})
target_include_directories(lua PUBLIC ${DEPS}/lua-5.4.8/src)
target_compile_definitions(lua PUBLIC LUA_USE_LINUX)
target_compile_options(lua PRIVATE ${QUIET_WARNINGS})
target_link_libraries(lua PUBLIC m dl)

# Bullet
file(GLOB_RECURSE BULLET_SOURCES
	${DEPS}/bullet3-2.83.7/src/BulletCollision/*.cpp
	${DEPS}/bullet3-2.83.7/src/BulletDynamics/*.cpp
	${DEPS}/bullet3-2.83.7/src/BulletSoftBody/*.cpp
	${DEPS}/bullet3-2.83.7/src/LinearMath/*.cpp)
add_library(bullet STATIC ${BULLET_SOURCES})
target_include_directories(bullet PUBLIC ${DEPS}/bullet3-2.83.7/src)
target_compile_options(bullet PRIVATE ${QUIET_WARNINGS})

# FreeType
foreach(opt FT_DISABLE_ZLIB FT_DISABLE_BZIP2 FT_DISABLE_PNG FT_DISABLE_HARFBUZZ FT_DISABLE_BROTLI)
	set(${opt} ON CACHE BOOL "" FORCE)
endforeach()
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(${DEPS}/freetype-master ${CMAKE_BINARY_DIR}/freetype EXCLUDE_FROM_ALL)

# ENet
file(GLOB ENET_SOURCES ${DEPS}/enet-1.3.18/*.c)
add_library(enet STATIC ${ENET_SOURCES})
target_include_directories(enet PUBLIC ${DEPS}/enet-1.3.18/include)
target_compile_definitions(enet PRIVATE HAS_FCNTL=1 HAS_POLL=1 HAS_GETADDRINFO=1 HAS_GETNAMEINFO=1
	HAS_GETHOSTBYNAME_R=1 HAS_GETHOSTBYADDR_R=1 HAS_INET_PTON=1 HAS_INET_NTOP=1 HAS_MSGHDR_FLAGS=1 HAS_SOCKLEN_T=1)
target_compile_options(enet PRIVATE ${QUIET_WARNINGS})

# gl4es (OpenGL on GLES 2)
file(GLOB GL4ES_SOURCES ${DEPS}/gl4es/src/gl/*.c ${DEPS}/gl4es/src/gl/wrap/*.c ${DEPS}/gl4es/src/gl/math/*.c)
list(APPEND GL4ES_SOURCES
	${DEPS}/gl4es/src/glx/hardext.c ${DEPS}/gl4es/src/glx/glx.c ${DEPS}/gl4es/src/glx/lookup.c
	${DEPS}/gl4es/src/glx/gbm.c ${DEPS}/gl4es/src/glx/streaming.c)
add_library(gl4es STATIC ${GL4ES_SOURCES})
target_include_directories(gl4es PUBLIC ${DEPS}/gl4es/include)
target_compile_definitions(gl4es PRIVATE ANDROID NOX11 NO_GBM DEFAULT_ES=2 NOEGL NO_INIT_CONSTRUCTOR STATICLIB USE_ANDROID_LOG)
target_compile_options(gl4es PRIVATE -std=gnu99 -funwind-tables -fvisibility=hidden
	-include ${DEPS}/gl4es/include/android_debug.h ${QUIET_WARNINGS})
target_link_libraries(gl4es PUBLIC log dl)

# IrrLime + Android device
set(IRR ${DEPS}/irrlicht-1.8.5/source/Irrlicht)
file(GLOB IRR_SOURCES ${IRR}/*.cpp)
foreach(f adler32 compress crc32 deflate inffast inflate inftrees trees uncompr zutil)
	list(APPEND IRR_SOURCES ${IRR}/zlib/${f}.c)
endforeach()
foreach(f jcapimin jcapistd jccoefct jccolor jcdctmgr jchuff jcinit jcmainct jcmarker jcmaster jcomapi jcparam
		jcprepct jcsample jctrans jdapimin jdapistd jdatadst jdatasrc jdcoefct jdcolor jddctmgr jdhuff jdinput
		jdmainct jdmarker jdmaster jdmerge jdpostct jdsample jdtrans jerror jfdctflt jfdctfst jfdctint jidctflt
		jidctfst jidctint jmemmgr jmemnobs jquant1 jquant2 jutils jcarith jdarith jaricom)
	list(APPEND IRR_SOURCES ${IRR}/jpeglib/${f}.c)
endforeach()
foreach(f png pngerror pngget pngmem pngpread pngread pngrio pngrtran pngrutil pngset pngtrans pngwio pngwrite
		pngwtran pngwutil)
	list(APPEND IRR_SOURCES ${IRR}/libpng/${f}.c)
endforeach()
foreach(f aescrypt aeskey aestab fileenc hmac prng pwd2key sha1 sha2)
	list(APPEND IRR_SOURCES ${IRR}/aesGladman/${f}.cpp)
endforeach()
foreach(f blocksort huffman crctable randtable bzcompress decompress bzlib)
	list(APPEND IRR_SOURCES ${IRR}/bzip2/${f}.c)
endforeach()
list(APPEND IRR_SOURCES ${IRR}/lzma/LzmaDec.c
	${CMAKE_CURRENT_LIST_DIR}/irrlicht/CIrrDeviceAndroid.cpp ${CMAKE_CURRENT_LIST_DIR}/irrlicht/GLExtensionStubs.c)

add_library(irrlicht STATIC ${IRR_SOURCES})
target_include_directories(irrlicht
	PUBLIC ${DEPS}/irrlicht-1.8.5/include
	PRIVATE ${IRR} ${IRR}/zlib ${IRR}/jpeglib ${IRR}/libpng ${DEPS}/irrlicht-1.8.5/source/lib
		${CMAKE_CURRENT_LIST_DIR}/irrlicht ${CMAKE_CURRENT_LIST_DIR}/include ${GLUE_DIR})
target_compile_definitions(irrlicht
	PUBLIC _IRR_STATIC_LIB_ NO_IRR_COMPILE_WITH_X11_ NO_IRR_COMPILE_WITH_CONSOLE_DEVICE_
		NO_IRR_COMPILE_WITH_SOFTWARE_ NO_IRR_COMPILE_WITH_BURNINGSVIDEO_ NO_IRR_COMPILE_WITH_JOYSTICK_EVENTS_
	# The bundled libpng has no NEON sources
	PRIVATE IRRLICHT_EXPORTS=1 PNG_THREAD_UNSAFE_OK PNG_NO_MMX_CODE PNG_NO_MNG_FEATURES PNG_ARM_NEON_OPT=0
		TINYGLTF_NO_EXTERNAL_IMAGE TINYGLTF_NO_STB_IMAGE TINYGLTF_NO_STB_IMAGE_WRITE)
# Irrlicht 1.8 still uses 'register'
target_compile_options(irrlicht PRIVATE ${QUIET_WARNINGS} $<$<COMPILE_LANGUAGE:CXX>:-Wno-register>)
target_link_libraries(irrlicht PUBLIC gl4es EGL android log)

# irrBullet
file(GLOB IRRBULLET_SOURCES ${DEPS}/irrBullet/*.cpp)
add_library(irrbullet STATIC ${IRRBULLET_SOURCES})
target_include_directories(irrbullet PUBLIC ${DEPS}/irrBullet)
target_compile_options(irrbullet PRIVATE ${QUIET_WARNINGS})
target_link_libraries(irrbullet PUBLIC bullet irrlicht)

# BoringSSL (httplib TLS)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(${DEPS}/boringssl ${CMAKE_BINARY_DIR}/boringssl EXCLUDE_FROM_ALL)
# BoringSSL turns on -Werror
target_compile_options(crypto PRIVATE -Wno-error)
target_compile_options(ssl PRIVATE -Wno-error)

# Engine resources zipped into a C array
file(GLOB_RECURSE RESOURCE_FILES RELATIVE ${ENGINE}/resources ${ENGINE}/resources/*)
list(TRANSFORM RESOURCE_FILES PREPEND ${ENGINE}/resources/ OUTPUT_VARIABLE RESOURCE_PATHS)
set(RESOURCES_ZIP ${CMAKE_BINARY_DIR}/resources.zip)
set(RESOURCES_C ${CMAKE_BINARY_DIR}/resources_zip.c)
add_custom_command(OUTPUT ${RESOURCES_C}
	COMMAND ${CMAKE_COMMAND} -E tar cf ${RESOURCES_ZIP} --format=zip ${RESOURCE_FILES}
	COMMAND ${CMAKE_COMMAND} -DINPUT=${RESOURCES_ZIP} -DOUTPUT=${RESOURCES_C} -DNAME=lime_resources_zip
		-P ${CMAKE_CURRENT_LIST_DIR}/cmake/EmbedFile.cmake
	WORKING_DIRECTORY ${ENGINE}/resources
	DEPENDS ${RESOURCE_PATHS} ${CMAKE_CURRENT_LIST_DIR}/cmake/EmbedFile.cmake
	VERBATIM)

# liblime.so
file(GLOB_RECURSE ENGINE_SOURCES ${ENGINE}/src/*.cpp ${ENGINE}/src/*.c)
# WindowAndroid.cpp replaces the GLFW Window.cpp
list(FILTER ENGINE_SOURCES EXCLUDE REGEX ".*/LimeEngine/src/Window\\.cpp$")
file(GLOB ANDROID_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

add_library(lime SHARED ${ENGINE_SOURCES} ${ANDROID_SOURCES} ${RESOURCES_C} ${GLUE_DIR}/android_native_app_glue.c)
target_include_directories(lime PRIVATE
	${ENGINE}/include ${ENGINE}/api ${DEPS}/sol2/include ${DEPS}/stb ${DEPS}/miniaudio
	${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/irrlicht ${GLUE_DIR})
target_compile_definitions(lime PRIVATE _USE_MATH_DEFINES MINIZ_NO_ZLIB_COMPATIBLE_NAMES LIME_WITH_OPENSSL)
target_link_libraries(lime PRIVATE irrbullet irrlicht bullet freetype enet lua ssl crypto gl4es EGL android log m dl)
# Keep the glue's entry point
target_link_options(lime PRIVATE -u ANativeActivity_onCreate -Wl,--no-undefined)
