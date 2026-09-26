# cmake -DINPUT=file -DOUTPUT=file.c -DNAME=symbol -P EmbedFile.cmake
file(READ "${INPUT}" hex HEX)
string(LENGTH "${hex}" hexLength)
math(EXPR size "${hexLength} / 2")
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," bytes "${hex}")
file(WRITE "${OUTPUT}"
	"/* Generated from ${INPUT} by EmbedFile.cmake. */\n"
	"const unsigned char ${NAME}[] = {${bytes}};\n"
	"const unsigned long ${NAME}_size = ${size};\n")
