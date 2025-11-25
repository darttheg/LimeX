#pragma once
#include <string>
#include <utility>

extern "C" {
	#include "lua.h"
	#include "luacode.h"
	#include <lualib.h>
}

extern "C" {
	int luaL_ref(lua_State* L, int tbl) {
		if (!lua_istable(L, tbl)) {
			luaL_error(L, "bad argument to luaL_ref: table expected");
		}
		
		lua_pushvalue(L, -1);
		int ref = lua_objlen(L, tbl) + 1;
		lua_rawseti(L, tbl, ref);
		lua_pop(L, 1);
		return ref;
	}
	void luaL_unref(lua_State* L, int tbl, int ref) {
		if (!lua_istable(L, tbl)) {
			luaL_error(L, "bad argument to luaL_ref: table expected");
		}

		lua_pushnil(L);
		lua_rawseti(L, tbl, ref);
	}
}

namespace lx {
	inline void push(lua_State* L, bool v) { lua_pushboolean(L, v); }
	inline void push(lua_State* L, int v) { lua_pushinteger(L, v); }
	inline void push(lua_State* L, double v) { lua_pushnumber(L, v); }
	inline void push(lua_State* L, float v) { lua_pushnumber(L, v); }
	inline void push(lua_State* L, const char* v) { lua_pushstring(L, v); }
	inline void push(lua_State* L, const std::string& v) { lua_pushlstring(L, v.c_str(), v.size()); }

	template<typename T>
	inline void push(lua_State*, const T&) {
		static_assert(sizeof(T) == 0, "lx::push: unsupported type");
	}

	class LuaTable {
	public:
		LuaTable() = default;
		LuaTable(lua_State* L, int ref) : L(L), ref(ref) {}
		LuaTable(const LuaTable&) = delete;
		LuaTable& operator=(const LuaTable&) = delete;
		~LuaTable() { release(); }

		LuaTable(LuaTable&& other) noexcept {
			L = other.L;
			ref = other.ref;
			other.ref = LUA_NOREF;
		}

		LuaTable& operator=(LuaTable&& other) noexcept {
			if (this != &other) {
				release();
				L = other.L;
				ref = other.ref;
				other.ref = LUA_NOREF;
			}
			return *this;
		}

		bool isValid() const { return L != nullptr && ref != LUA_NOREF; }

		lua_State* state() const { return L; }

		LuaTable operator[](const std::string& key) const {
			if (!isValid()) return LuaTable();

			lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
			lua_getfield(L, -1, key.c_str());
			if (!lua_istable(L, -1)) {
				lua_pop(L, 1);
				lua_newtable(L);
				lua_pushvalue(L, -1);
				lua_setfield(L, -3, key.c_str());
			}

			int newRef = luaL_ref(L, LUA_REGISTRYINDEX);
			lua_pop(L, 1);

			return LuaTable(L, newRef);
		}

		template<typename V>
		void set(const std::string& key, const V& value) const {
			if (!isValid()) return;

			lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
			lx::push(L, value);
			lua_setfield(L, -2, key.c_str());
			lua_pop(L, 1);
		}

		void setFunc(const std::string& key, lua_CFunction func) const {
			if (!isValid()) return;

			lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
			lua_pushcfunction(L, func, 0);
			lua_setfield(L, -2, key.c_str());
			lua_pop(L, -1);
		}

		struct Setter {
			const LuaTable& tbl;
			std::string key;

			Setter(const LuaTable& t, const std::string& k) : tbl(t), key(k) {}

			template<typename V>
			Setter& operator=(const V& value) {
				tbl.set(key, value);
				return *this;
			}

			Setter& operator=(lua_CFunction func) {
				tbl.setFunc(key, func);
				return *this;
			}

			Setter operator[](const std::string& key) const {
				return Setter(this->tbl, key);
			}
		};

		class LuaState {
		public:
			LuaState() = default;
			explicit LuaState(lua_State* L) : L(L) {}

			void attach(lua_State* Lb) { L = Lb; }
			lua_State* state() const { return L; }

			LuaTable operator[](const std::string& name) const {
				if (!L) return LuaTable();

				lua_getglobal(L, name.c_str());
				if (!lua_istable(L, -1)) {
					lua_pop(L, 1);
					lua_newtable(L);
					lua_pushvalue(L, -1);
					lua_setglobal(L, name.c_str());
				}

				int ref = luaL_ref(L, LUA_REGISTRYINDEX);
				return LuaTable(L, ref);
			}

			void setGlobal(const std::string& name, lua_CFunction func) const {
				if (!L) return;
				lua_pushcfunction(L, func, 0);
				lua_setglobal(L, name.c_str());
			}

		private:
			lua_State* L = nullptr;
		};

	private:
		lua_State* L = nullptr;
		int ref = LUA_NOREF;

		void release() {
			if (L && ref != LUA_NOREF) {
				luaL_unref(L, LUA_REGISTRYINDEX, ref);
				ref = LUA_NOREF;
			}
		}
	};
}