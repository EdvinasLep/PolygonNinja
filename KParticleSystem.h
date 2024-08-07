#pragma once
#include <memory>
#include <Windows.h>
#include "KParticle.h"
#include "KParticleSystemData.h"
#include <vector>
#include <functional>

class KParticleSystem;
typedef std::shared_ptr<KParticleSystem>    KParticleSystemPtr;
typedef std::weak_ptr<KParticleSystem>		KParticleSystemWeakPtr;
class KParticleSystem : public std::enable_shared_from_this<KParticleSystem>
{
public:
	struct KParticleSystemInitParam
	{
		KVector2	gravity;
		KVector2	wind;
		int			initialNumParticle;
		int			maximumNumParticle;
		COLORREF	color;
		double		defaultLifetime;
		bool		isRegenerate;
		KVector2	position;
		std::function<void(KParticleSystemPtr)>				initCallback;
		std::function<bool(KParticleSystemPtr)>				afterUpdateCallback;
		std::function<KParticlePtr (KParticleSystemPtr)>	generateParticleCallback;
	};
protected:
	KParticleSystemDataPtr		m_spParticleSystemData;
#ifdef _UNDEFINED
	KParticleSystemData* _data;
#endif
	std::vector<KParticlePtr>	m_particles;
#ifdef _UNDEFINED
	KParticle*			_particles;
#endif
	bool						m_Regenerate;
	KVector2					m_Position;
	int							m_maximumNumParticle;
	COLORREF					m_Color;
	double						m_defaultLifetimeOfParticle;
	std::function<void(KParticleSystemPtr)>			m_initCallback;
	std::function<bool(KParticleSystemPtr)>			m_afterUpdateCallback;
	std::function<KParticlePtr(KParticleSystemPtr)>	m_generateParticleCallback;

public:
	KParticleSystem() {}
	void Initialize( KParticleSystemInitParam param);
	void AddParticle();
	virtual bool Update(float fElapsedTime);
	virtual void Draw(HDC hdc);

	KVector2 GetPosition() const { return m_Position; }
	void SetPosition(KVector2 pos) {
		m_Position = pos; 
		m_spParticleSystemData->SetPosition(pos);
	}
	KParticlePtr operator[](int index) { return m_particles[index]; }
	KParticleSystemDataPtr GetParticleSystemData() const {
		return m_spParticleSystemData;
	}
	int GetParticles() const { return m_particles.size(); }
	int	GetMaximumNumParticle() const { return m_maximumNumParticle; }
	double GetDefaultLifetimeOfParticle() const { return m_defaultLifetimeOfParticle; }
};
