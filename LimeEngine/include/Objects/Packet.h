#pragma once
#include <vector>
#include <string>
#include <optional>
#include <cstdint>

#include <sol/sol.hpp>

extern "C" { struct lua_State; }

class Packet {
public:
	Packet() : pos(0) {}
	~Packet() = default;
	explicit Packet(std::vector<uint8_t> dat) : data(std::move(dat)), pos(0) {}

	void pushUnsigned8(uint8_t v); // Unsigned, 8 bits (0-255)
	void pushUnsigned16(uint16_t v); // Unsigned, 16 bits (0-65535)
	void pushUnsignedInt(uint32_t v); // Unsigned, 32 bits (0-big)
	void pushInt(int32_t v); // Signed, 32 bits
	void pushFloat(float v); // Float
	void pushString(const std::string& s); // String
	bool pushFile(const std::string& path);

	sol::object readUnsigned8();
	sol::object readUnsigned16();
	sol::object readUnsignedInt();
	sol::object readInt();
	sol::object readFloat();
	sol::object readString();
	bool readFile(const std::string outPath);

	int getPos() const { return pos; }
	void setPos(int p);

	void clear() { data.clear(); }
	int getSize() const { return (int)data.size(); }
	const std::vector<uint8_t>& getData() const { return data; }

private:
	std::vector<uint8_t> data;
	int pos = 0;

	void write(const void* src, size_t n);
	template<typename T>
	sol::object read(lua_State* l) {
		if (pos + (int)sizeof(T) > (int)data.size()) return sol::nil;
		T v;
		std::memcpy(&v, data.data() + pos, sizeof(T));
		pos += sizeof(T);
		return sol::make_object(sol::state_view(l), v);
	}
};

namespace Object::PacketBind {
	void bind(lua_State* ls);
}