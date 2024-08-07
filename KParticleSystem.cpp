#include "KParticleSystem.h"
#include "KVectorUtil.h"
#include <algorithm>
#include "KRgb.h"

void KParticleSystem::Initialize(KParticleSystemInitParam param)
{
	param.gravity;
	param.wind;
	param.initialNumParticle;
	param.maximumNumParticle;
	param.color;
	param.defaultLifetime;
	param.isRegenerate;
	param.initCallback;
	param.afterUpdateCallback;
	param.generateParticleCallback;

	m_Regenerate = param.isRegenerate;
	m_Position = param.position;
	m_maximumNumParticle = param.maximumNumParticle;;
	m_Color = param.color;;
	m_defaultLifetimeOfParticle = param.defaultLifetime;;
	m_initCallback = param.initCallback;
	m_afterUpdateCallback = param.afterUpdateCallback;
	m_generateParticleCallback = param.generateParticleCallback;

	m_spParticleSystemData.reset(new KParticleSystemData());
	m_particles.reserve(m_maximumNumParticle);
	m_spParticleSystemData->SetGravity(param.gravity);
	m_spParticleSystemData->SetWind(param.wind);
	m_spParticleSystemData->SetPosition(param.position);

	if (m_initCallback) {
		m_initCallback(__super::shared_from_this());
	}
}

void KParticleSystem::AddParticle()
{
	if (m_generateParticleCallback) {
		KParticlePtr particle = m_generateParticleCallback(shared_from_this());
		m_particles.push_back(particle);
	}
}

bool KParticleSystem::Update(float fElapsedTime)
{
	std::vector<KParticlePtr> particles;
	for (size_t i = 0; i < m_particles.size(); ++i)
	{
		KParticlePtr particle = m_particles[i];
		particle->Update(fElapsedTime);
		if (particle->IsAlive()) {
			particles.push_back(particle);
		}
	}
	m_particles = particles;
	bool ret = false;
	if (m_afterUpdateCallback) {
		ret = m_afterUpdateCallback(shared_from_this());
	}

	return false;
}

void KParticleSystem::Draw(HDC hdc)
{
	const int numSegments = 10;
	// For each particle in the system
	for (size_t i = 0; i < m_particles.size(); i++)
	{
		// Get the current particle
		KParticle& part = *m_particles[i];
		// Calculate particle intensity
		const double intense = (double)part.GetAge()/ part.GetLifetime();
		const double radius = __max(0.01, intense * 0.10);
		KRgb rgb(0, 0, 1);
		COLORREF color = rgb.GetColor();
		KVectorUtil::DrawCircle(hdc, part.GetPosition(), (float)radius, numSegments, 1, 0, color);
	}
}
