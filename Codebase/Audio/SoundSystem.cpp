#include "SoundSystem.h"

SoundSystem* SoundSystem::instance = NULL;

SoundSystem::SoundSystem(unsigned int channels) {
	masterVolume = 1.0f;

	std::cout << "Creating SoundSystem!" << std::endl;
	std::cout << "Found the following devices: " << alcGetString(NULL, ALC_DEVICE_SPECIFIER) << std::endl;	//outputs all OAL devices

	device = alcOpenDevice(NULL);	//Open the 'best' device

	if (!device) {
		std::cout << "Failed to create SoundSystem! (No valid device!)" << std::endl;
		return;
	}

	std::cout << "SoundSystem created with device: " << alcGetString(device, ALC_DEVICE_SPECIFIER) << std::endl;	//Outputs used OAL device!

	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

	for (unsigned int i = 0; i < channels; ++i) {
		ALuint source;

		alGenSources(1, &source);
		ALenum error = alGetError();

		if (error == AL_NO_ERROR)	{
			sources.push_back(new OALSource(source));
		}
		else{
			break;
		}
	}

	std::cout << "SoundSystem has " << sources.size() << " channels available!" << std::endl;
}

SoundSystem::~SoundSystem(void)	{
	for (std::vector<SoundEmitter*>::iterator i = totalEmitters.begin(); i != totalEmitters.end(); ++i) {
		delete (*i);
	}

	for (std::vector<OALSource*>::iterator i = sources.begin(); i != sources.end(); ++i) {
		alDeleteSources(1, &(*i)->source);
		delete (*i);
	}

	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

void SoundSystem::SetListener(const Mat4& transform)
{
	listenerTransform = transform;
	listenerPos = transform.GetTranslation();
}

void		SoundSystem::Update(float msec) {
	UpdateListener(); // update listener position

	//Update values for every node, whether in range or not
	for (std::vector<SoundEmitter*>::iterator i = totalEmitters.begin(); i != totalEmitters.end(); ++i) {
		if ((*i)->GetIsGlobal()){
			(*i)->SetPosition(listenerPos);
		}
		(*i)->Update(msec);
	}

	CullNodes();	//First off, remove nodes that are too far away

	if (emitters.size() > sources.size()) {
		std::sort(emitters.begin(), emitters.end(), SoundEmitter::CompareNodesByPriority);	//Then sort by priority

		DetachSources(emitters.begin() + (sources.size() + 1), emitters.end());		//Detach sources from nodes that won't be covered this frame
		AttachSources(emitters.begin(), emitters.begin() + (sources.size()));	//And attach sources to nodes that WILL be covered this frame
	}
	else{
		AttachSources(emitters.begin(), emitters.end());//And attach sources to nodes that WILL be covered this frame
	}

	emitters.clear();	//We're done for the frame! empty the emitters list
}

void	SoundSystem::CullNodes() {
	for (std::vector<SoundEmitter*>::iterator i = totalEmitters.begin(); i != totalEmitters.end();) {

		float length;

		// distance from listener
		length = (listenerPos -
			(*i)->GetPosition()).Length();
		

		if (length < (*i)->GetRadius() && (*i)->GetSound() && (*i)->GetTimeLeft() > 0) {
			emitters.push_back((*i));
		}
		else{
			(*i)->DetachSource();
		}

		++i;

	}
}

void	SoundSystem::DetachSources(std::vector<SoundEmitter*>::iterator from, std::vector<SoundEmitter*>::iterator to) {
	for (std::vector<SoundEmitter*>::iterator i = from; i != to; ++i) {
		(*i)->DetachSource();
	}
}

void	SoundSystem::AttachSources(std::vector<SoundEmitter*>::iterator from, std::vector<SoundEmitter*>::iterator to) {
	for (std::vector<SoundEmitter*>::iterator i = from; i != to; ++i) {
		if (!(*i)->GetSource()) {	//Don't attach a new source if we already have one!
			(*i)->AttachSource(GetSource());
		}
	}
}

OALSource*	SoundSystem::GetSource() {
	for (std::vector<OALSource*>::iterator i = sources.begin(); i != sources.end(); ++i) {
		OALSource*s = *i;
		if (!s->inUse) {
			return s;
		}
	}
	return NULL;
}

void		SoundSystem::SetMasterVolume(float value)	{
	masterVolume = value;
	alListenerf(AL_GAIN, masterVolume);
}

// FIX THIS SHIT
void	SoundSystem::UpdateListener() {

	Vec3 dirup[2];
	//forward
	dirup[0].x = listenerTransform.values[2];
	dirup[0].y = listenerTransform.values[6];
	dirup[0].z = listenerTransform.values[10];
	//Up
	dirup[1].x = listenerTransform.values[1];
	dirup[1].y = listenerTransform.values[5];
	dirup[1].z = listenerTransform.values[9];

	dirup[0].Normalize();
	dirup[1].Normalize();

	alListenerfv(AL_POSITION, (float*)&listenerPos);
	alListenerfv(AL_ORIENTATION, (float*)&dirup);
	
}

void	SoundSystem::Play(Sound* s, SoundMOD modifier) {
	SoundEmitter* n = new SoundEmitter();
	n->SetSound(s);
	n->SetPriority(SOUNDPRIORITY_HIGH);

	n->SetVolume(modifier.volume);
	n->SetRadius(modifier.radius);
	n->SetPitch(modifier.pitch);
	n->SetLooping(modifier.looping);
	n->SetIsGlobal(modifier.isGlobal);
	n->SetPosition(modifier.position);

	totalEmitters.push_back(n);
}