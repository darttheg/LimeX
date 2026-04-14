#include "LuaBinder.h"
#include <sol/sol.hpp>
#include "Application.h"

// INTERFACE INCLUDES
#include "Interfaces/Object2D.h"
#include "Interfaces/Object3D.h"
#include "Interfaces/PhysicsObject.h"
//

// MODULE INLCUDES
#include "Modules/ModuleLime.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleWindow.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleGUI.h"
#include "Modules/ModuleSound.h"
#include "Modules/ModuleFile.h"
#include "Modules/ModulePhysics.h"
#include "Modules/ModuleNetwork.h"
#include "Modules/ModuleWeb.h"
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
#include "Objects/SoundSource.h"
#include "Objects/Noise.h"
#include "Objects/EditBox.h"
#include "Objects/RigidBody.h"
#include "Objects/Constraints.h"
#include "Objects/ParticleSystem.h"
#include "Objects/Packet.h"
//

void LuaBinder::BindAll(Application* app) {
	lua_State* ls = app->GetLuaState();

	// Interfaces
	Interface::Object2DBind::bind(ls, app->GetRenderer());
	Interface::Object3DBind::bind(ls, app->GetRenderHelper());
	Interface::PhysicsObjectBind::bind(ls, app->GetPhysicsManager());
	Interface::ConstraintBind::bind(ls, app->GetPhysicsManager());

	// Modules
	Module::Lime::bind(app);
	Module::Scene::bind(app);
	Module::Window::bind(app);
	Module::Input::bind(app);
	Module::GUI::bind(app);
	Module::Sound::bind(app);
	Module::File::bind(app);
	Module::Physics::bind(app);
	Module::Network::bind(app);
	Module::Web::bind(app);

	// Objects
	Object::EventBind::bind(ls);
	Object::Vec2Bind::bind(ls);
	Object::Vec3Bind::bind(ls);
	Object::Vec4Bind::bind(ls);
	Object::TextureBind::bind(ls, app->GetRenderer());
	Object::MaterialBind::bind(ls);
	Object::SkydomeBind::bind(ls, app->GetRenderHelper());
	Object::CameraBind::bind(ls, app->GetRenderHelper());
	Object::Text2DBind::bind(ls, app->GetRenderHelper(), app->GetGUIManager());
	Object::Image2DBind::bind(ls, app->GetRenderHelper());
	Object::BillboardBind::bind(ls, app->GetRenderHelper());
	Object::Text3DBind::bind(ls, app->GetRenderHelper(), app->GetGUIManager());
	Object::EmptyBind::bind(ls, app->GetRenderHelper());
	Object::MeshBind::bind(ls, app->GetDebugConsole(), app->GetRenderer(), app->GetRenderHelper());
	Object::MeshBufferBind::bind(ls, app->GetRenderer());
	Object::LightBind::bind(ls, app->GetRenderHelper());
	Object::ShaderMaterialBind::bind(ls, app->GetRenderer());
	Object::SoundSourceBind::bind(ls, app->GetSoundManager(), app->GetRenderHelper());
	Object::NoiseBind::bind(ls);
	Object::EditBoxBind::bind(ls, app->GetRenderHelper(), app->GetGUIManager());
	Object::RigidBodyBind::bind(ls, app->GetPhysicsManager(), app->GetRenderer());
	Object::HingeConstraintBind::bind(ls);
	Object::ConeTwistConstraintBind::bind(ls);
	Object::ParticleSystemBind::bind(ls, app->GetRenderHelper());
	Object::PacketBind::bind(ls);
}