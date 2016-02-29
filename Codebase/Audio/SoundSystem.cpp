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

	// create context
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	// sounds dont get too loud when close
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

	// generate sources per channel
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
	for (auto emitter : totalEmitters) {
		delete emitter;
	}

	for (auto src : sources) {
		alDeleteSources(1, &src->source);
		delete src;
	}

	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

void SoundSystem::SetListenerMatrix(const Mat4& transform)
{
	listenerTransform = transform;
	listenerPos = transform.GetTranslation();
}

void		SoundSystem::Update(float msec) {
	UpdateListener(); // update listener position

	//Update values for every node, whether in range or not
	for (auto emitter : totalEmitters) {
		if (emitter->GetIsGlobal()){
			emitter->SetPosition(listenerPos);
		}
		emitter->Update(msec);
	}

	CullNodes();	//First off, remove nodes that are too far away

	std::sort(totalEmitters.begin(), totalEmitters.end(), SoundEmitter::CompareNodesByPriority);	//Then sort by priority
	AttachSources(sources.size());

	emitters.clear();	//We're done for the frame! empty the emitters list
}

void	SoundSystem::CullNodes() {
	std::vector<SoundEmitter*>	tempEMT;

	int size = 0;
	int sizeMax = sources.size(); // max allowed emitters

	for (auto emt : totalEmitters) {

		float length;

		// distance from listener
		length = (listenerPos -
			emt->GetPosition()).Length();
		
		// if emitter should play
		if (length < emt->GetRadius() && 
			emt->GetSound() &&
			emt->GetTimeLeft() > 0 &&
			size < sizeMax) {

			emitters.push_back(emt);
			tempEMT.push_back(emt);
			size++;
		} 
		else{
			emt->DetachSource();
			if (!emt->GetIsSingle()) {
				tempEMT.push_back(emt);
			}
			else {
				delete emt; // delete single instances
			}
		}

		if (size >= sizeMax) {
			break;
		}
	}

	totalEmitters = tempEMT;
	std::cout << "SIZE:: " << totalEmitters.size() << std::endl;

}

void SoundSystem::AttachSources(int numSources) {
	for (auto emt : emitters) {
		if (!emt->GetSource()) {	//Don't attach a new source if we already have one!
			emt->AttachSource(GetSource());
		}
	}
}

OALSource*	SoundSystem::GetSource() {
	for (auto src : sources) {
		OALSource* s = src;
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
	alListenerfv(AL_VELOCITY, (float*)&listenerVel);
	alListenerfv(AL_ORIENTATION, (float*)&dirup);
}

void	SoundSystem::Play(Sound* s, SoundMOD modifier) {
	SoundEmitter* n = new SoundEmitter();
	n->SetSound(s);
	n->setIsSingle(true);

	n->SetPriority(modifier.priority);
	n->SetVolume(modifier.volume);
	n->SetRadius(modifier.radius);
	n->SetPitch(modifier.pitch);
	n->SetLooping(modifier.looping);
	n->SetIsGlobal(modifier.isGlobal);
	n->SetPosition(modifier.position);

	totalEmitters.push_back(n);
}