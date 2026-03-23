#include "LuaBinder.h"
#include <sol/sol.hpp>
#include "Application.h"

// INTERFACE INCLUDES
#include "Interfaces/Object2D.h"
#include "Interfaces/Object3D.h"
//

// MODULE INLCUDES
#include "Modules/ModuleLime.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleWindow.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleGUI.h"
//

// OBJECT INCLUDES
#include "Objects/Event.h"
#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"
#include "Objects/Skydome.h"
#include "Objects/Camera.h"
#include "Objects/Text2D.h"
#include "Objects/Image2D.h"
#include "Objects/Billboard.h"
#include "Objects/Text3D.h"
#include "Objects/Empty.h"
#include "Objects/Mesh.h"
#include "Objects/MeshBuffer.h"
#include "Objects/Light.h"
#include "Objects/ShaderMaterial.h"
//

void LuaBinder::BindAll(Application* app) {
	// Interfaces
	Interface::Object2DBind::bind(app);
	Interface::Object3DBind::bind(app);

	// Modules
	Module::Lime::bind(app);
	Module::Scene::bind(app);
	Module::Window::bind(app);
	Module::Input::bind(app);
	Module::GUI::bind(app);

	// Objects
	Object::EventBind::bind(app);
	Object::Vec2Bind::bind(app);
	Object::Vec3Bind::bind(app);
	Object::Vec4Bind::bind(app);
	Object::TextureBind::bind(app);
	Object::MaterialBind::bind(app);
	Object::SkydomeBind::bind(app);
	Object::CameraBind::bind(app);
	Object::Text2DBind::bind(app);
	Object::Image2DBind::bind(app);
	Object::BillboardBind::bind(app);
	Object::Text3DBind::bind(app);
	Object::EmptyBind::bind(app);
	Object::MeshBind::bind(app);
	Object::MeshBufferBind::bind(app);
	Object::LightBind::bind(app);
	Object::ShaderMaterialBind::bind(app);
}