#include "KParticle.h"

void KParticle::Update(float fElapsedTime)
{
	// Update particle's movement according to environment
	m_Velocity = m_Velocity - m_spParticleSystemData->GetGravity() * fElapsedTime
		+ m_spParticleSystemData->GetWind()*fElapsedTime;
	// Update particle's position according to movement
	m_Position = m_Position + m_Velocity*fElapsedTime;
	// Update particle's age
	m_age += fElapsedTime;
}
