#pragma once
#include <Windows.h>
#include "KVector2.h"
#include "KParticleSystemData.h"

class KParticle;
typedef std::shared_ptr<KParticle>	KParticlePtr;
typedef std::weak_ptr<KParticle>	KParticleWeakPtr;
class KParticle
{
private:
	KVector2 m_Position;
	KVector2 m_Velocity;
	double m_age;
	double m_lifetime;
	COLORREF m_Color;
	KParticleSystemDataPtr	m_spParticleSystemData;
#ifdef _UNDEFINED
	KParticleSystemData* _data;
#endif

public:
	KParticle(KVector2 pos, KVector2 velocity, COLORREF col, double lifetime) {
		m_Position = pos;
		m_Velocity = velocity;
		m_age = 0.0;
		m_lifetime = lifetime;
		m_Color = col;
	}
	virtual void Update(float fElapsedTime);

	void SetParticleSystemData(KParticleSystemDataPtr sp) {
		m_spParticleSystemData = sp;
	}
	const KVector2& GetPosition() const { return m_Position; }
	const KVector2& GetVelocity() const { return m_Velocity; }
	const COLORREF GetColor() const { return m_Color; }
	double GetAge() const { return m_age; }
	double GetLifetime() const { return m_lifetime; }
	bool IsAlive() const { return m_age <= m_lifetime; }
};
