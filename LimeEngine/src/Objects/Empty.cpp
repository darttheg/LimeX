#include "Objects/Empty.h"

#include "Application.h"
#include "Renderer.h"
#include "DebugConsole.h"

#include "Objects/Vec3.h"

#include "irrlicht.h"

static DebugConsole* d;
static Renderer* r;

Empty::Empty() {
	emp = r->createEmptyNode();
}

Empty::Empty(const Vec3& pos) {
	emp = r->createEmptyNode();
	setPosition(pos);
}

void Empty::destroy() {
	if (emp) emp->remove();
	emp = nullptr;
}

irr::scene::ISceneNode* Empty::getNode() const { return emp; }

void Object::EmptyBind::bind(Application* a) {
	r = a->GetRenderer();
	d = a->GetDebugConsole();

	// Object Empty, An invisible object used to mark locations, parent objects, and more.
	// Inherits Object3D

	sol::state_view view(a->GetLuaState());
	sol::usertype<Empty> obj = view.new_usertype<Empty>(
		"Empty",
		sol::constructors<Empty(), Empty(const Vec3& pos)>(),

		sol::base_classes, sol::bases<Object3D>(),
		sol::meta_function::type, [](const Empty&) { return "Empty"; }
	);

	obj[sol::meta_function::to_string] = [](const Empty& v) {
		return "Empty";
		};

	// Constructor
	// Constructor Vec3 pos

	// End Object
}