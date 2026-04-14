#include "Objects/Packet.h"

#pragma warning(disable: 4996) // fopen warning

static lua_State* l;

void Packet::pushUnsigned8(uint8_t v) { write(&v, 1); }

void Packet::pushUnsigned16(uint16_t v) { write(&v, 2); }

void Packet::pushUnsignedInt(uint32_t v) { write(&v, 4); }

void Packet::pushInt(int32_t v) { write(&v, 4); }

void Packet::pushFloat(float v) { write(&v, 4); }

void Packet::pushString(const std::string& s) {
	pushUnsigned16(static_cast<uint16_t>(s.size()));
	data.insert(data.end(), s.begin(), s.end());
}

bool Packet::pushFile(const std::string& path) {
	FILE* f = fopen(path.c_str(), "rb");
	if (!f) return false;
	fseek(f, 0, SEEK_END);
	uint32_t size = (uint32_t)ftell(f);
	rewind(f);
	pushUnsignedInt(size);
	size_t offset = data.size();
	data.resize(offset + size);
	fread(data.data() + offset, 1, size, f);
	fclose(f);
	return true;
}

sol::object Packet::readUnsigned8() { return read<uint8_t>(l); }

sol::object Packet::readUnsigned16() { return read<uint16_t>(l); }

sol::object Packet::readUnsignedInt() { return read<uint32_t>(l); }

sol::object Packet::readInt() { return read<int32_t>(l); }

sol::object Packet::readFloat() { return read<float>(l); }

sol::object Packet::readString() {
	if (pos + 2 > (int)data.size()) return sol::nil;

	uint16_t len;
	std::memcpy(&len, data.data() + pos, 2);
	pos += 2;

	if (pos + (int)len > (int)data.size()) return sol::nil;

	std::string s(reinterpret_cast<const char*>(data.data() + pos), len);
	pos += len;

	return sol::make_object(sol::state_view(l), s);
}

bool Packet::readFile(const std::string outPath) {
	if (pos + 4 > (int)data.size()) return false;
	uint32_t size;
	std::memcpy(&size, data.data() + pos, 4);
	pos += 4;
	if (pos + (int)size > (int)data.size()) return false;
	FILE* f = fopen(outPath.c_str(), "wb");
	if (!f) return false;
	fwrite(data.data() + pos, 1, size, f);
	fclose(f);
	pos += size;
	return true;
}

void Packet::write(const void* src, size_t n) {
	const uint8_t* out = reinterpret_cast<const uint8_t*>(src);
	data.insert(data.end(), out, out + n);
}

void Packet::setPos(int v) {
	if (v < 0) v = 0;
	if (v > data.size()) v = (int)data.size();
	pos = v;
}

void Object::PacketBind::bind(lua_State* ls) {
	l = ls;

	// Object Packet, An object used to store data to send across a network.

	// Constructor

	sol::state_view view(ls);
	sol::usertype<Packet> obj = view.new_usertype<Packet>(
		"Packet",
		sol::constructors<Packet()>(),
		sol::meta_function::type, [](const Packet&) { return "Packet"; },

		// Field number position, The read position.
		"position", sol::property(&Packet::getPos, &Packet::setPos)
	);

	obj[sol::meta_function::to_string] = [](const Packet& v) {
		return "Packet";
		};

	// Writes an unsigned 8 bit number to this `Packet`.
	// Params number v
	// Returns void
	obj.set_function("writeUint8", &Packet::pushUnsigned8);

	// Writes an unsigned 16 bit number to this `Packet`.
	// Params number v
	// Returns void
	obj.set_function("writeUint16", &Packet::pushUnsigned16);

	// Writes an unsigned integer to this `Packet`.
	// Params number v
	// Returns void
	obj.set_function("writeUint32", &Packet::pushUnsignedInt);

	// Writes an integer to this `Packet`.
	// Params number v
	// Returns void
	obj.set_function("writeInt", &Packet::pushInt);

	// Writes a float point number to this `Packet`.
	// Params number v
	// Returns void
	obj.set_function("writeFloat", &Packet::pushFloat);

	// Writes a string to this `Packet`.
	// Params string v
	// Returns void
	obj.set_function("writeString", &Packet::pushString);

	// Writes a file from `path` to this `Packet`. Returns true if the file was found and written successfully.
	// Params string path
	// Returns boolean
	obj.set_function("writeFile", &Packet::pushFile);

	// Returns an unsigned 8 bit number or nil on fail.
	// Returns number
	obj.set_function("readUint8", &Packet::readUnsigned8);

	// Returns an unsigned 16 bit number or nil on fail.
	// Returns number
	obj.set_function("readUint16", &Packet::readUnsigned16);

	// Returns an unsigned integer or nil on fail.
	// Returns number
	obj.set_function("readUint32", &Packet::readUnsignedInt);

	// Returns an integer or nil on fail.
	// Returns number
	obj.set_function("readInt", &Packet::readInt);

	// Returns a floating point number or nil on fail.
	// Returns number
	obj.set_function("readFloat", &Packet::readFloat);

	// Returns a string or nil on fail.
	// Returns string
	obj.set_function("readString", &Packet::readString);

	// Saves a file to `path` from this `Packet`. Returns true on success.
	// Params string path
	// Returns boolean
	obj.set_function("readFile", &Packet::readFile);

	// Clears the data of this `Packet`.
	// Returns void
	obj.set_function("clear", &Packet::clear);

	// Returns the size of this `Packet` in bytes.
	// Returns void
	obj.set_function("getSize", &Packet::getSize);

	// End Object
}