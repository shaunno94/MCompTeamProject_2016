#ifndef ORBIS
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

void AudioComponent::SetPositions()
{
	Vec3 pos = m_Object->GetWorldTransform().GetTranslation();

	for (auto emt : m_AudioList) {
		emt->SetPosition(pos);
	}
}

Vec3 AudioComponent::GetVec3Velocity()
{
	btVector3 vel = m_Object->GetPhysicsComponent()->GetPhysicsBody()->getInterpolationLinearVelocity();
	Vec3 velProper = Vec3();
	velProper.x = vel.x();
	velProper.y = vel.y();
	velProper.z = vel.z();

	return velProper;
}

void AudioComponent::SetVelocities()
{
	Vec3 vel = GetVec3Velocity();

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

	SetRadius(200.0f);
}

// Car Audio

#define MAX_REV_VOL 0.5f
#define MIN_REV_VOL 0.1f

#define MAX_REV_PIT 1.5f
#define MIN_REV_PIT 0.2f

#define MAX_IDLE_VOL 0.15f
#define MIN_IDLE_VOL 0.0f

#define REV_SPEED_VOL 50.0f // degree of interpolation
#define REV_SPEED_PIT 70.0f 
#define IDLE_SPEED_VOL 15.0f // lower = faster cut off

enum SoundHolder {
	IDLE,
	REV
};

void AudioCompCar::Update()
{
	SetPositions();
	SetVelocities();

	// Handle volume
	Vec3 vel = GetVec3Velocity();
	float speed = vel.Length();

	float revVol = fmin(MAX_REV_VOL, (speed / REV_SPEED_VOL)); // max vol
	revVol = fmax(revVol, MIN_REV_VOL); // min vol

	float idleVol = fmin(MAX_IDLE_VOL, (1 - (speed / IDLE_SPEED_VOL)) ); 
	idleVol = fmax(idleVol, MIN_IDLE_VOL); 

	m_AudioList[REV]->SetVolume(revVol);
	m_AudioList[IDLE]->SetVolume(idleVol);

	// Handle pitch
	float speedNorm = fmax(MIN_REV_PIT, (speed / REV_SPEED_PIT)); // min pitch
	speedNorm = fmin(speedNorm, MAX_REV_PIT); // max pitch

	// gear shift
	if (speedNorm < 0.6f) { // boost pitch when going slow
		speedNorm += 0.4f;
	}
	else if (speedNorm < 0.9f) {
		speedNorm += 0.2f;
	}

	
	m_AudioList[REV]->SetPitch(speedNorm);
}

// Car Audio listener

void AudioCompCarLitener::Update()
{
	SoundSystem::Instance()->SetListenerMatrix(m_Object->GetWorldTransform());
	SoundSystem::Instance()->SetListenerVelocity(GetVec3Velocity());

	AudioCompCar::Update();
}
#endif