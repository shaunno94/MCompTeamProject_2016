#include "SoundSystem.h"
#ifdef ORBIS
#include <libsysmodule.h>
#endif
SoundSystem* SoundSystem::instance = NULL;

SoundSystem::SoundSystem(unsigned int channels)
{
	masterVolume = 1.0f;

	std::cout << "Creating SoundSystem!" << std::endl;

#ifndef ORBIS
	std::cout << "Found the following devices: " << alcGetString(NULL, ALC_DEVICE_SPECIFIER) << std::endl;	//outputs all OAL devices

	device = alcOpenDevice(NULL);	//Open the 'best' device

	if (!device)
	{
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
	for (unsigned int i = 0; i < channels; ++i)
	{
		ALuint source;

		alGenSources(1, &source);
		ALenum error = alGetError();

		if (error == AL_NO_ERROR)
		{
			sources.push_back(new AudioSource(source));
		}
		else
		{
			break;
		}
	}
#else
	sceSysmoduleLoadModule(SCE_SYSMODULE_AUDIO_3D);

	sceAudio3dInitialize(0);

	SceAudio3dOpenParameters sParameters;
	sceAudio3dGetDefaultOpenParameters(&sParameters);
	sParameters.uiGranularity = 1024;
	sParameters.uiMaxObjects = channels;
	sParameters.uiQueueDepth = 1;

	SceUserServiceUserId userId;
	int ret = sceUserServiceGetInitialUser(&userId);

	ret = sceAudio3dPortOpen(SCE_USER_SERVICE_USER_ID_SYSTEM, &sParameters, &audioPort);

	for (int i = 0; i < channels; ++i) {
		SceAudio3dPortId id;
		ret = sceAudio3dObjectReserve(audioPort, &id);

		AudioSource src = AudioSource(id);

		sources.push_back(new AudioSource(src));
	}

#endif

	m_MaxDynamicSources = channels - 1;

	m_Background = new SoundEmitter();
	m_Background->SetPriority(SOUNDPRIORITY_ALWAYS);
	m_Background->SetIsGlobal(true);

	std::cout << "SoundSystem has " << sources.size() << " channels available!" << std::endl;
}

SoundSystem::~SoundSystem(void)
{
	for (auto emitter : totalEmitters)
	{
		delete emitter;
	}
	delete m_Background;

#ifndef ORBIS
	for (auto src : sources)
	{
		alDeleteSources(1, &src->source);
		delete src;
	}

	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
#endif
}

void SoundSystem::SetListenerMatrix(const Mat4& transform)
{
	listenerTransform = transform;
	listenerPos = transform.GetTranslation();
}

void SoundSystem::SetBackgroundMusic(Sound* snd)
{
	m_Background->SetSound(snd);
}

void SoundSystem::SetBackgroundVolume(float val)
{
	m_Background->SetVolume(
	  fmax(fmin(1.0f, val), 0.0f)
	);
}

void		SoundSystem::Update(float msec)
{
	UpdateListener(); // update listener position
	CullNodes();	//First off, remove nodes that are too far away
	std::sort(totalEmitters.begin(), totalEmitters.end(), SoundEmitter::CompareNodesByPriority);	//Then sort by priority
	AttachSources(); // add playable sounds
	//Update values for every node, whether in range or not
	for (auto emitter : emitters)
	{
		if (emitter->GetIsGlobal())
		{
			emitter->SetPosition(listenerPos);
		}
		emitter->Update(msec);
	}

	m_Background->Update(msec); // update background music

	emitters.clear();	//done for this frame
}

void	SoundSystem::CullNodes()
{
	std::vector<SoundEmitter*>	tempEMT;

	int size = 0;

	for (auto emt : totalEmitters)
	{

		float length;

		// distance from listener
		length = (listenerPos -
		          emt->GetPosition()).Length();

		// if emitter should play
		if (length < emt->GetRadius() &&
		    emt->GetSound() &&
		    emt->GetTimeLeft() > 0 &&
		    size < m_MaxDynamicSources)
		{

			emitters.push_back(emt);
			tempEMT.push_back(emt);
			size++;
		}
		else
		{
			emt->DetachSource();
			if (!emt->GetIsSingle())
			{
				tempEMT.push_back(emt);
			}
			else
			{
				delete emt; // delete single instances
			}
		}

		if (size >= m_MaxDynamicSources)
		{
			break;
		}
	}

	totalEmitters = tempEMT;
}

void SoundSystem::AttachSources()
{
	// attach background first
	if (m_Background->GetSound() && !m_Background->GetSource())
		m_Background->AttachSource(GetSource());

	for (auto emt : emitters)
	{
		if (!emt->GetSource())  	//Don't attach a new source if we already have one!
		{
			emt->AttachSource(GetSource());
		}
	}
}

AudioSource*	SoundSystem::GetSource()
{
	for (auto src : sources)
	{
		AudioSource* s = src;
		if (!s->inUse)
		{
			return s;
		}
	}
	return NULL;
}

void	SoundSystem::SetMasterVolume(float value)
{
	masterVolume = value;
	//alListenerf(AL_GAIN, masterVolume);
}

void	SoundSystem::UpdateListener()
{
	Vec3 forward = listenerTransform.GetRotation() * Vec3(0, 0, -1);
	Vec3 up = listenerTransform.GetRotation() * Vec3(0, 1, 0);

	Vec3 dirup[2];
	dirup[0] = forward;
	dirup[1] = up;

	dirup[0].Normalize();
	dirup[1].Normalize();

#ifndef ORBIS
	alListenerfv(AL_POSITION, (float*)&listenerPos);
	alListenerfv(AL_VELOCITY, (float*)&listenerVel);
	alListenerfv(AL_ORIENTATION, (float*)&dirup);
#endif // !ORBIS
}

void	SoundSystem::Play(Sound* s, SoundMOD modifier)
{
	SoundEmitter* n = new SoundEmitter();
	n->SetSound(s);
	n->setIsSingle(true);

	n->SetPriority(modifier.priority);
	n->SetVolume(modifier.volume);
	n->SetRadius(modifier.radius);
	n->SetPitch(modifier.pitch);
	n->SetLooping(modifier.looping);
	n->SetIsGlobal(modifier.isGlobal);

	totalEmitters.push_back(n);
}