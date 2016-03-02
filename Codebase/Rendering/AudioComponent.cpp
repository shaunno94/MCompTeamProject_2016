#include "AudioComponent.h"
#include "GameObject.h"

// Base Audio

void AudioComponent::SetObject(GameObject* obj)
{
	m_Object = obj;
}

void AudioComponent::AddEmitters()
{
	for (auto emt : m_AudioList) {
		SoundSystem::Instance()->AddSoundEmitter(emt);
	}
}

void AudioComponent::SetPositions(const Vec3& pos)
{
	for (auto emt : m_AudioList) {
		emt->SetPosition(pos);
	}
}

void AudioComponent::SetVelocities(const Vec3& vel)
{
	for (auto emt : m_AudioList) {
		emt->SetVelocity(vel);
	}
}

void AudioComponent::SetRadius(float rad)
{
	for (auto emt : m_AudioList) {
		emt->SetRadius(rad);
	}
}

AudioCompCar::AudioCompCar(bool global) : AudioComponent()
{
	// idle noise
	SoundEmitter* emt = new SoundEmitter(SoundManager::GetSound(ENGINE_IDLE));
	emt->SetIsGlobal(global);

	m_AudioList.push_back(emt);

	// engine noise
	emt = new SoundEmitter(SoundManager::GetSound(ENGINE_REV));
	emt->SetIsGlobal(global);

	m_AudioList.push_back(emt);

	AddEmitters();

	SetRadius(350.0f);
}

// Car Audio

#define MAX_REV_VOL 0.5f
#define MIN_REV_VOL 0.2f

#define MAX_IDLE_VOL 0.15f
#define MIN_IDLE_VOL 0.0f

#define REV_SPEED_REL 50.0f // degree of interpolation
#define IDLE_SPEED_REL 20.0f 

enum SoundHolder {
	IDLE,
	REV
};

void AudioCompCar::Update()
{
	// move sound
	SetPositions(m_Object->GetWorldTransform().GetTranslation());

	std::cout << "pos: " << m_Object->GetWorldTransform().GetTranslation() << std::endl;

	// set velocity
	btVector3 vel = m_Object->GetPhysicsComponent()->GetPhysicsBody()->getInterpolationLinearVelocity();
	Vec3 velProper = Vec3();
	velProper.x = vel.x();
	velProper.y = vel.y();
	velProper.z = vel.z();
	//SetVelocities(velProper);

	// Handle volume
	float speed = vel.length();

	float revVol = fmin(MAX_REV_VOL, (speed / REV_SPEED_REL)); // max vol
	revVol = fmax(revVol, MIN_REV_VOL); // min vol

	float idleVol = fmin(MAX_IDLE_VOL, (1 - (speed / IDLE_SPEED_REL)) ); 
	idleVol = fmax(idleVol, MIN_IDLE_VOL); 

	m_AudioList[REV]->SetVolume(revVol);
	m_AudioList[IDLE]->SetVolume(idleVol);

	// Handle pitch
	float speedNorm = fmax(0.1f, (speed / 100.0f)); // min pitch
	speedNorm = fmin(speedNorm, 1.3f); // max pitch

	// gear shift
	if (speedNorm < 0.6f) // boost pitch when going slow
		speedNorm += 0.2f;
	
	m_AudioList[REV]->SetPitch(speedNorm);
}
