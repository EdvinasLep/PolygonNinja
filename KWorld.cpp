/*
	Windows porting in 2021: jintaeks@dongseo.ac.kr
*/

#include "KPhysicsEngine.h"

const float			KWorld::gravityScale = 3.0f; // original 5.0f. 20210428_jintaeks
//const float		KWorld::gravityScale = 0.0f; // test
const KVector2		KWorld::gravity(0, 9.8f * gravityScale);
const float			KWorld::dt = 1.0f / 60.0f;
bool				KWorld::frameStepping = false;
bool				KWorld::canStep = false;

void IntegrateForces(std::shared_ptr<KRigidbody> b, float dt)
{
	if (b->m_invMass == 0.0f)
		return;

	b->velocity += (b->force * b->m_invMass + KWorld::gravity) * dt;
	b->angularVelocity += b->torque * b->m_invI * dt;

	// Apply damping.
	// ODE: dv/dt + c * v = 0
	// Solution: v(t) = v0 * exp(-c * t)
	// Time step: v(t + dt) = v0 * exp(-c * (t + dt)) 
	//                      = v0 * exp(-c * t) * exp(-c * dt) = v * exp(-c * dt)
	// v2 = exp(-c * dt) * v1
	b->velocity *= std::exp(-(b->m_linearDamping)*dt);
	b->angularVelocity *= std::exp(-(b->m_linearDamping)*dt);

	//// Pade approximation:
	//// v2 = v1 * 1 / (1 + c * dt)
	//// https://en.wikipedia.org/wiki/Pad%C3%A9_approximant
	//b->velocity *= 1.0f / (1.0f + dt * b->m_linearDamping);
	//b->angularVelocity *= 1.0f / (1.0f + dt * b->m_angularDamping);
}

void IntegrateVelocity(std::shared_ptr<KRigidbody> b, float dt)
{
	if (b->m_invMass == 0.0f)
		return;

	b->position += b->velocity * dt;
	b->rotation += b->angularVelocity * dt;
}

/*static*/ KWorld& KWorld::Singleton()
{
	static KWorld instance(KWorld::dt, 20);
	return instance;
}

// constructor
KWorld::KWorld(float dt, uint32 iterations) 
	: m_dt(dt), m_iterations(iterations)
{
}

void KWorld::GenerateCollisionInfo()
{
	// Generate new collision info
	m_contacts.clear();
	for (uint32 i = 0; i < m_bodies.size(); ++i)
	{
		std::shared_ptr<KRigidbody> A = m_bodies[i];

		for (uint32 j = i + 1; j < m_bodies.size(); ++j)
		{
			std::shared_ptr<KRigidbody> B = m_bodies[j];
			if (A->m_invMass == 0 && B->m_invMass == 0)
				continue;
			KManifold m(A,B);
			m.Solve();
			if (m.contact_count)
				m_contacts.emplace_back(m);
		}
	}
}

bool KWorld::_IsBodyInRemoveCandidate(std::shared_ptr<KRigidbody> body_)
{
	for (uint32 i = 0; i < m_removeCandidates.size(); ++i) {
		std::shared_ptr<KRigidbody> body = m_removeCandidates[i];
		if (body == body_)
			return true;
	}
	return false;
}

void KWorld::_RemoveRigidbody()
{
	for (uint32 i = 0; i < m_removeCandidates.size(); ++i) {
		std::shared_ptr<KRigidbody> body = m_removeCandidates[i];
		m_bodies.erase(std::remove_if(m_bodies.begin(), m_bodies.end()
			,[body](std::shared_ptr<KRigidbody> body_) { return body == body_; })
			, m_bodies.end());
	}
}

void KWorld::Step()
{
	_RemoveRigidbody();
	// Generate new collision info
	GenerateCollisionInfo();

	// Integrate forces
	for (uint32 i = 0; i < m_bodies.size(); ++i)
		IntegrateForces(m_bodies[i], m_dt);

	// Initialize collision
	for (uint32 i = 0; i < m_contacts.size(); ++i)
		m_contacts[i].Initialize();

	// Solve collisions
	for (uint32 j = 0; j < m_iterations; ++j)
		for (uint32 i = 0; i < m_contacts.size(); ++i)
			m_contacts[i].ApplyImpulse();

	// Integrate velocities
	for (uint32 i = 0; i < m_bodies.size(); ++i)
		IntegrateVelocity(m_bodies[i], m_dt);

	// Correct positions
	for (uint32 i = 0; i < m_contacts.size(); ++i)
		m_contacts[i].PositionalCorrection();

	// Clear all forces
	for (uint32 i = 0; i < m_bodies.size(); ++i)
	{
		std::shared_ptr<KRigidbody> b = m_bodies[i];
		b->force.Set(0, 0);
		b->torque = 0;
	}

	// Update shape data from rigidbody
	for (std::shared_ptr<KRigidbody> body : m_bodies)
	{
		// qff
		body->BodyToShape();
	}
}

std::shared_ptr<KRigidbody> KWorld::CreateRigidbody(std::shared_ptr<KShape> shape, float x, float y)
{
	assert(shape);
	std::shared_ptr<KRigidbody> b;
	b.reset(new KRigidbody(shape, x, y));
	shape->body = b;
	shape->Initialize();
	b->restitution = 0.2f;
	b->dynamicFriction = 0.2f;
	b->staticFriction = 0.4f;
	return b;
}

std::shared_ptr<KRigidbody> KWorld::Add(std::shared_ptr<KShape> shape, float x, float y)
{
	std::shared_ptr<KRigidbody> b = CreateRigidbody(shape, x, y);
	m_bodies.push_back(b);
	return b;
}

bool KWorld::Remove(std::shared_ptr<KRigidbody> body)
{
	m_removeCandidates.push_back(body);
	return true;
}

void KWorld::Clear()
{
	m_bodies.clear();
	m_contacts.clear();
}

std::shared_ptr<KShape> KWorld::CreateCircle(float radius, float x, float y, bool isStatic)
{
	std::shared_ptr<KCircleShape> c;
	c.reset(new KCircleShape((float)radius));
	std::shared_ptr<KRigidbody> body = Add(c, x, y);
	body->BodyToShape();
	if (isStatic)
		body->SetStatic();

	return c;
}

std::shared_ptr<KShape> KWorld::CreatePolygon(KVector2* vertices, uint32 numVertices, float x, float y, bool isStatic)
{
	std::shared_ptr<KPolygonShape> polygon;
	polygon.reset(new KPolygonShape());
	{
		polygon->Set(vertices, numVertices);
		std::shared_ptr<KRigidbody> body = Add(polygon, 0, 0);
		body->shape->ComputeMass(1.0f);
		body->SetRotation(0);
		body->restitution = 0.2f;
		body->dynamicFriction = 0.2f;
		body->staticFriction = 0.4f;
		body->position = KVector2(x, y);
		body->BodyToShape();
		if (isStatic)
			body->SetStatic();
	}
	return polygon;
}

std::shared_ptr<KShape> KWorld::CreateBox(float width, float height, float x, float y, bool isStatic)
{
	std::shared_ptr<KPolygonShape> polygon;
	polygon.reset(new KPolygonShape());
	{
		polygon->SetBox(width, height);
		std::shared_ptr<KRigidbody> body = Add(polygon, x, y);
		body->SetRotation(0);
		body->BodyToShape();
		if (isStatic)
			body->SetStatic();
	}
	return polygon;
}
