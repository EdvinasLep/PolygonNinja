#pragma once
#include <memory>
#include "KVector2.h"

class KParticleSystemData;
typedef std::shared_ptr<KParticleSystemData>    KParticleSystemDataPtr;
typedef std::weak_ptr<KParticleSystemData>    KParticleSystemDataWeakPtr;
class KParticleSystemData
{
private:
	KVector2 m_gravity;
	KVector2 m_wind;
	KVector2 m_position;

public:
	const KVector2 GetGravity() const { return m_gravity; }
	const KVector2 GetWind() const { return m_wind; }
	const KVector2 GetPosition() const { return m_position; }
	void SetGravity(const KVector2& gravity) { m_gravity = gravity; }
	void SetWind(const KVector2& wind) { m_wind = wind; }
	void SetPosition(const KVector2& wind) { m_position = wind; }
};

