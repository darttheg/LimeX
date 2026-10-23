#pragma once
#include <type_traits>

#define VEC_LINK(Type, Prop) Type::link(&c, \
	[](void* o) { return static_cast<std::remove_reference_t<decltype(c)>*>(o)->get##Prop(); }, \
	[](void* o, const Type& v) { static_cast<std::remove_reference_t<decltype(c)>*>(o)->set##Prop(v); } \
)