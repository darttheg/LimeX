#include "Objects/RigidBody.h"
#include "PhysicsManager.h"
#include "Interfaces/PhysicsObject.h"
#include "Objects/Mesh.h"
#include "irrBullet.h"
#include "Objects/Vec3.h"
#include "Interfaces/Object3D.h"

#include <sol/sol.hpp>

static lua_State* l;
static PhysicsManager* p;

btCollisionObject* RigidBody::getCollisionObject() const {
	return rb ? rb->getPointer() : nullptr;
}

irr::scene::ISceneNode* RigidBody::getNode() const {
    return src;
}

RigidBody::RigidBody(const Mesh& m) : RigidBody(m, m) {}

RigidBody::RigidBody(const Object3D& m, const Mesh& c) {
    rb = p->createRigidBody(m, c);
    if (!rb) return;
    src = m.getNode(); col = c.getNode();

    irr::scene::IAnimatedMeshSceneNode* s = dynamic_cast<irr::scene::IAnimatedMeshSceneNode*>(m.getNode());
    if (s)
        p->appendToMatchedRBSrc(s->getMesh(), this);
    irr::scene::IAnimatedMeshSceneNode* cm = dynamic_cast<irr::scene::IAnimatedMeshSceneNode*>(c.getNode());
    if (cm)
        p->appendToMatchedRBSrc(cm->getMesh(), this);

    PhysicsObject::createCallbacks();
    PhysicsObject::insertIntoCallbacks();
}

bool RigidBody::loadMesh(const Mesh& m) {
    if (!rb || !m.getNode()) return false;
    src = m.getNode();
    return src;
}

Vec3 RigidBody::getPosition() {
    if (!rb) return Vec3();
    btVector3 pos = rb->getPointer()->getWorldTransform().getOrigin();
    return Vec3(pos.getX(), pos.getY(), pos.getZ());
}

void RigidBody::setPosition(const Vec3& pos) {
    if (!rb) return;

    btTransform transform = rb->getPointer()->getWorldTransform();
    transform.getIdentity();
    transform.setOrigin(btVector3(pos.getX(), pos.getY(), pos.getZ()));

    rb->getPointer()->setWorldTransform(transform);
    rb->getMotionState()->setWorldTransform(transform);
}

Vec3 RigidBody::getRotation() {
    if (!rb) return Vec3();

    btTransform transform = rb->getPointer()->getWorldTransform();
    irr::core::matrix4 outMatrix;
    transform.getOpenGLMatrix(outMatrix.pointer());

    irr::core::vector3df rot = outMatrix.getRotationDegrees();
    return Vec3(rot.X, rot.Y, rot.Z);
}

void RigidBody::setRotation(const Vec3& rot) {
    if (!rb) return;

    btTransform transform = rb->getPointer()->getWorldTransform();
    irr::core::matrix4 inMatrix;
    inMatrix.setRotationDegrees(irr::core::vector3df(rot.getX(), rot.getY(), rot.getZ()));

    const btVector3 ogPos = transform.getOrigin();

    irr::core::quaternion irrQuat;
    irrQuat.getMatrix(inMatrix);
    btQuaternion newRot(irrQuat.X, irrQuat.Y, irrQuat.Z, irrQuat.W);

    transform.setOrigin(ogPos);
    transform.setRotation(newRot);

    rb->getPointer()->setWorldTransform(transform);
    if (btMotionState* ms = rb->getMotionState())
        ms->setWorldTransform(transform);

    rb->activate(true);
}

void RigidBody::setFriction(float f) {
    if (!rb) return;
    rb->setFriction(f);
}

float RigidBody::getFriction() {
    return rb ? rb->getFriction() : 0.0f;
}

void RigidBody::setAnisotropicFriction(const Vec3& f) {
    if (!rb) return;
    rb->setAnisotropicFriction(irr::core::vector3df(f.getX(), f.getY(), f.getZ()));
}

Vec3 RigidBody::getAnisotropicFriction() {
    return rb ? Vec3(rb->getAnisotropicFriction().X, rb->getAnisotropicFriction().Y, rb->getAnisotropicFriction().Z) : Vec3();
}

void RigidBody::setMass(float f) {
    if (!rb) return;

    btVector3 inertia(0,0,0);
    btCollisionShape* shape = rb->getPointer()->getCollisionShape();

    if (f > 0.0f)
        shape->calculateLocalInertia(f, inertia);

    rb->getPointer()->setMassProps(f, inertia);
    rb->getPointer()->updateInertiaTensor();
    rb->getPointer()->activate(true);
}

float RigidBody::getMass() {
    return rb ? 1.0 / rb->getPointer()->getInvMass() : 0.0f;
}

float RigidBody::getLinearDamping() {
    return rb ? rb->getLinearDamping() : 0.0f;
}

void RigidBody::setLinearDamping(float f) {
    if (!rb) return;
    rb->setDamping(f, rb->getAngularDamping());
}

float RigidBody::getAngularDamping() {
    return rb ? rb->getAngularDamping() : 0.0f;
}

void RigidBody::setAngularDamping(float f) {
    if (!rb) return;
    rb->setDamping(rb->getLinearDamping(), f);
}

float RigidBody::getElasticity() {
    return rb ? rb->getRestitution() : 0.0f;
}

void RigidBody::setElasticity(float f) {
    if (!rb) return;
    rb->setRestitution(f);
}

Vec3 RigidBody::getCenterOfMass() {
    if (!rb) return Vec3();

    btVector3 out = rb->getPointer()->getCenterOfMassTransform().getOrigin();
    return Vec3(out.getX(), out.getY(), out.getZ());
}

void RigidBody::applyForce(const Vec3& force, const Vec3& relPos) {
    if (!rb) return;
    rb->applyForce(irr::core::vector3df(force.getX(), force.getY(), force.getZ()), irr::core::vector3df(relPos.getX(), relPos.getY(), relPos.getZ()));
}

void RigidBody::applyImpulse(const Vec3& force, const Vec3& relPos) {
    if (!rb) return;
    rb->applyImpulse(irr::core::vector3df(force.getX(), force.getY(), force.getZ()), irr::core::vector3df(relPos.getX(), relPos.getY(), relPos.getZ()));
}

void RigidBody::applyTorque(const Vec3& force, bool impulsive) {
    if (!rb) return;
    if (impulsive)
        rb->applyTorqueImpulse(irr::core::vector3df(force.getX(), force.getY(), force.getZ()));
    else
        rb->applyTorque(irr::core::vector3df(force.getX(), force.getY(), force.getZ()));
}

void RigidBody::faceTarget(const Vec3& pos) {
    if (!rb) return;
    rb->faceTarget(irr::core::vector3df(pos.getX(), pos.getY(), pos.getZ()));
}

void RigidBody::wakeUp() {
    if (!rb) return;
    rb->activate(true);
}

bool RigidBody::isSleeping() {
    return rb ? rb->isActive() : false;
}

void RigidBody::setSleeping(bool v) {
    if (!rb) return;
    rb->forceActivationState(v ? EActivationState::EAS_SLEEPING : EActivationState::EAS_ACTIVE);
}

void RigidBody::clearForces() {
    if (!rb) return;
    rb->getPointer()->clearForces();
}

void RigidBody::applySimpleImpulse(float forceMagnitude, const Vec3& worldPos) {
    if (!rb) return;

    btVector3 centerMass = rb->getPointer()->getWorldTransform().getOrigin();
    btVector3 dir = btVector3(worldPos.getX(), worldPos.getY(), worldPos.getZ()) - centerMass;
    dir.normalize();

    btVector3 imp = dir * -1.0 * forceMagnitude;
    btVector3 rel = btVector3(worldPos.getX(), worldPos.getY(), worldPos.getZ()) - centerMass;

    rb->getPointer()->applyImpulse(imp, rel);
    rb->activate(true);
}

void RigidBody::applySimpleForce(float forceMagnitude, const Vec3& worldPos) {
    if (!rb) return;

    btVector3 centerMass = rb->getPointer()->getWorldTransform().getOrigin();
    btVector3 dir = btVector3(worldPos.getX(), worldPos.getY(), worldPos.getZ()) - centerMass;
    dir.normalize();

    btVector3 imp = dir * -1.0 * forceMagnitude;
    btVector3 rel = btVector3(worldPos.getX(), worldPos.getY(), worldPos.getZ()) - centerMass;

    rb->getPointer()->applyForce(imp, rel);
    rb->activate(true);
}

Vec3 RigidBody::getLinearVelocity() {
    return rb ? Vec3(rb->getLinearVelocity().X, rb->getLinearVelocity().Y, rb->getLinearVelocity().Z) : Vec3();
}

void RigidBody::setLinearVelocity(const Vec3& vel) {
    if (!rb) return;
    rb->setLinearVelocity(irr::core::vector3df(vel.getX(), vel.getY(), vel.getZ()));
}

Vec3 RigidBody::getAngularVelocity() {
    return rb ? Vec3(rb->getAngularVelocity().X, rb->getAngularVelocity().Y, rb->getAngularVelocity().Z) : Vec3();
}

void RigidBody::setAngularVelocity(const Vec3& vel) {
    if (!rb) return;
    rb->setAngularVelocity(irr::core::vector3df(vel.getX(), vel.getY(), vel.getZ()));
}

Vec3 RigidBody::getLinearFactor() {
    return rb ? Vec3(rb->getLinearFactor().X, rb->getLinearFactor().Y, rb->getLinearFactor().Z) : Vec3();
}

void RigidBody::setLinearFactor(const Vec3& f) {
    if (!rb) return;
    rb->setLinearFactor(irr::core::vector3df(f.getX(), f.getY(), f.getZ()));
}

Vec3 RigidBody::getAngularFactor() {
    return rb ? Vec3(rb->getAngularFactor().X, rb->getAngularFactor().Y, rb->getAngularFactor().Z) : Vec3();
}

void RigidBody::setAngularFactor(const Vec3& f) {
    if (!rb) return;
    rb->setAngularFactor(irr::core::vector3df(f.getX(), f.getY(), f.getZ()));
}

Vec3 RigidBody::getForward() {
    if (!rb) return Vec3(0, 0, 1);

    btVector3 f = btVector3(0, 0, 1);
    f = rb->getPointer()->getWorldTransform().getBasis() * f;
    return Vec3(f.getX(), f.getY(), f.getZ());
}

float RigidBody::getSleepingThreshold() {
    return rb ? rb->getLinearSleepingThreshold() : 0.0f;
}

void RigidBody::setSleepingThreshold(float f) { // Velocity vector must be <= f to sleep (linear and angular)
    if (!rb) return;

    if (f <= 0)
        rb->setActivationState(EActivationState::EAS_DISABLE_DEACTIVATION);
    else
        rb->setActivationState(EActivationState::EAS_WANTS_DEACTIVATION);

    rb->setSleepingThresholds(f, f);
}

bool RigidBody::getGhost() {
    return rb ? isGhost : false;
}

void RigidBody::setGhost(bool v) {
    if (!rb) return;
    rb->setCollisionFlags(v ? ECollisionFlag::ECF_NO_CONTACT_RESPONSE : (ECollisionFlag)0);
    isGhost = v;
}

Vec3 RigidBody::getGravity() {
    return rb ? Vec3(rb->getGravity().X, rb->getGravity().Y, rb->getGravity().Z) : Vec3();
}

void RigidBody::setGravity(const Vec3& f) {
    if (!rb) return;
    rb->setGravity(irr::core::vector3df(f.getX(), f.getY(), f.getZ()));
}

void RigidBody::destroy() {
    if (src) src->drop();
    if (col) col->drop();
    src = nullptr;
    col = nullptr;
    p->removeRigidBody(rb);
    isGhost = false;
}

void RigidBody::applyPreset(int v) {
    if (!rb) return;

    btRigidBody* bt = rb->getPointer();
    Vec3 gr = p->getGravity();
    btVector3 grav = btVector3(gr.getX(), gr.getY(), gr.getZ());

    switch (v) {
    case 1: // Static
        bt->setFriction(0.5f);
        bt->setRestitution(0.0f);
        bt->setDamping(0.0f, 0.0f);
        bt->setLinearFactor(btVector3());
        bt->setAngularFactor(btVector3());
        break;
    case 2: // Bouncy
        bt->setFriction(0.3f);
        bt->setRestitution(0.7f);
        bt->setDamping(0.0f, 0.0f);
        bt->setLinearFactor(btVector3(1,1,1));
        bt->setAngularFactor(btVector3(1,1,1));
        break;
    case 3: // Icy
        bt->setFriction(0.02f);
        bt->setRestitution(0.1f);
        bt->setDamping(0.0f, 0.0f);
        bt->setLinearFactor(btVector3(1, 1, 1));
        bt->setAngularFactor(btVector3(1, 1, 1));
        break;
    case 4: // Heavy
        bt->setFriction(0.8f);
        bt->setRestitution(0.0f);
        bt->setDamping(0.2f, 0.2f);
        bt->setGravity(btVector3(0, -30, 0));
        bt->setLinearFactor(btVector3(1, 1, 1));
        bt->setAngularFactor(btVector3(1, 1, 1));
        break;
    case 5: // Floaty
        bt->setFriction(0.3f);
        bt->setRestitution(0.1f);
        bt->setDamping(0.5f, 0.5f);
        bt->setGravity(btVector3(0, -2, 0));
        bt->setLinearFactor(btVector3(1, 1, 1));
        bt->setAngularFactor(btVector3(1, 1, 1));
        break;
    default: // Normal
        bt->setFriction(0.5f);
        bt->setRestitution(0.0f);
        bt->setDamping(0.0f, 0.0f);
        bt->setGravity(grav);
        bt->setLinearFactor(btVector3(1, 1, 1));
        bt->setAngularFactor(btVector3(1, 1, 1));
        break;
    }
}

void RigidBody::loadVisual(irr::scene::ISceneNode* visual) {
    if (!rb) return;
    src = visual;
}

void Object::RigidBodyBind::bind(lua_State* ls, PhysicsManager* ps) {
    l = ls;
	p = ps;

	// Object RigidBody, A wrapper to `Mesh` objects that allows for them to react to physics. It can be created with a `Mesh` as its visual and collision shape, or with a custom collision shape independent of any 3D object.
	// Inherits PhysicsObject

	// Constructor Mesh base
	// Constructor any root, Mesh collision

	sol::state_view view(ls);
	sol::usertype<RigidBody> obj = view.new_usertype<RigidBody>(
		"RigidBody",
		sol::constructors<RigidBody(const Mesh& m), RigidBody(const Object3D& m, const Mesh& c)>(),

		sol::base_classes, sol::bases<PhysicsObject>(),
		sol::meta_function::type, [](const RigidBody&) { return "RigidBody"; },

        // Field Vec3 position, The 3D position of this object in the scene.
        "position", sol::property(
            [](RigidBody& c) { return Vec3{ [&] { return c.getPosition(); }, [&](auto v) { c.setPosition(v); } }; },
            [](RigidBody& c, const Vec3& v) { c.setPosition(v); }
        ),
        // Field Vec3 gravity, The force of gravity applied to this object per physics step.
        "gravity", sol::property(
            [](RigidBody& c) { return Vec3{ [&] { return c.getGravity(); }, [&](auto v) { c.setGravity(v); } }; },
            [](RigidBody& c, const Vec3& v) { c.setGravity(v); }
        ),
        // Field Vec3 rotation, The 3D rotation of this object in the scene in degrees.
        "rotation", sol::property(
            [](RigidBody& c) { return Vec3{ [&] { return c.getRotation(); }, [&](auto v) { c.setRotation(v); } }; },
            [](RigidBody& c, const Vec3& v) { c.setRotation(v); }
        ),
        // Field number friction, Sets the friction coefficient.
        "friction", sol::property(&RigidBody::getFriction, &RigidBody::setFriction),
        // Field Vec3 anisotropicFriction, Sets per-axis friction scaling.
        "anisotropicFriction", sol::property(
            [](RigidBody& c) { return Vec3{ [&] { return c.getAnisotropicFriction(); }, [&](auto v) { c.setAnisotropicFriction(v); } }; },
            [](RigidBody& c, const Vec3& v) { c.setAnisotropicFriction(v); }
        ),
        // Field number mass, Sets the mass and recalculates inertia.
        "mass", sol::property(&RigidBody::getMass, &RigidBody::setMass),
        // Field boolean sleeping, Whether or not this physics object is sleeping.
        "sleeping", sol::property(&RigidBody::isSleeping, &RigidBody::setSleeping),
        // Field boolean ghost, Sets whether or not other physics objects can pass through this object.
        "ghost", sol::property(&RigidBody::getGhost, &RigidBody::setGhost),
        // Field number linearDamping, Sets the linear damping, reducing linear velocity over time.
        "linearDamping", sol::property(&RigidBody::getLinearDamping, &RigidBody::setLinearDamping),
        // Field number angularDamping, Sets the angular damping, reducing rotational velocity over time.
        "angularDamping", sol::property(&RigidBody::getAngularDamping, &RigidBody::setAngularDamping),
        // Field number elasticity, Sets the elasticity, controlling bounciness on collision.
        "elasticity", sol::property(&RigidBody::getElasticity, &RigidBody::setElasticity),
        // Field number sleepThreshold, Sets the velocity threshold this `RigidBody` must be below in order to trigger sleeping.
        "sleepThreshold", sol::property(&RigidBody::getSleepingThreshold, &RigidBody::setSleepingThreshold),
        // Field Vec3 linearVelocity, Sets the linear velocity.
        "linearVelocity", sol::property(
            [](RigidBody& c) { return Vec3{ [&] { return c.getLinearVelocity(); }, [&](auto v) { c.setLinearVelocity(v); } }; },
            [](RigidBody& c, const Vec3& v) { c.setLinearVelocity(v); }
        ),
        // Field Vec3 angularVelocity, Sets the angular velocity.
        "angularVelocity", sol::property(
            [](RigidBody& c) { return Vec3{ [&] { return c.getAngularVelocity(); }, [&](auto v) { c.setAngularVelocity(v); } }; },
            [](RigidBody& c, const Vec3& v) { c.setAngularVelocity(v); }
        ),
        // Field Vec3 linearFactor, Sets the linear factor, controlling which axes linear motion is allowed on. (0 locks, 1 allows)
        "linearFactor", sol::property(
            [](RigidBody& c) { return Vec3{ [&] { return c.getLinearFactor(); }, [&](auto v) { c.setLinearFactor(v); } }; },
            [](RigidBody& c, const Vec3& v) { c.setLinearFactor(v); }
        ),
        // Field Vec3 angularFactor, Sets the angular factor, controlling which axes rotation is allowed on. (0 locks, 1 allows)
        "angularFactor", sol::property(
            [](RigidBody& c) { return Vec3{ [&] { return c.getAngularFactor(); }, [&](auto v) { c.setAngularFactor(v); } }; },
            [](RigidBody& c, const Vec3& v) { c.setAngularFactor(v); }
        )
	);

	obj[sol::meta_function::to_string] = [](const RigidBody& v) {
		return "RigidBody";
		};

    // Applies a preset to this `RigidBody` that alters its physical properties.
    // Params Lime.Enum.PhysicalPreset preset
    // Returns void
    obj.set_function("loadPreset", &RigidBody::applyPreset);

    // Loads a new visual `Mesh` into this `RigidBody`.
    // Returns boolean
    obj.set_function("loadMesh", &RigidBody::loadMesh);

    // Clears all forces applied to this `RigidBody`.
    // Returns void
    obj.set_function("clearForces", &RigidBody::clearForces);

    // Returns the center of mass position of this `RigidBody` in world space.
    // Returns Vec3
    obj.set_function("getCenterOfMass", &RigidBody::getCenterOfMass);

    // Rotates this `RigidBody` to look at a position in world space.
    // Params Vec3 pos
    // Returns void
    obj.set_function("lookAt", &RigidBody::faceTarget);

    // Returns the forward vector of this `RigidBody`.
    // Returns Vec3
    obj.set_function("getForward", &RigidBody::getForward);

    // Applies a continous force to this `RigidBody`, where `pos` is relative to the center of this `RigidBody`.
    // Params Vec3 force, Vec3 pos
    // Returns void
    obj.set_function("applyForceRelative", &RigidBody::applyForce);

    // Applies an impulse to this `RigidBody`, where `pos` is relative to the center of this `RigidBody`.
    // Params Vec3 impulse, Vec3 pos
    // Returns void
    obj.set_function("applyImpulseRelative", &RigidBody::applyImpulse);

    // Applies a continous force to this `RigidBody`, where `pos` is in world space.
    // Params number force, Vec3 pos
    // Returns void
    obj.set_function("applyForce", &RigidBody::applySimpleForce);

    // Applies an impulse to this `RigidBody`, where `pos` is in world space.
    // Params number force, Vec3 pos
    // Returns void
    obj.set_function("applyImpulse", &RigidBody::applySimpleImpulse);

    // Applies a torque to this `RigidBody`, where `pos` is in world space.
    // Params Vec3 force, bool impulse
    // Returns void
    obj.set_function("applyTorque", &RigidBody::applyTorque);

	// End Object
}