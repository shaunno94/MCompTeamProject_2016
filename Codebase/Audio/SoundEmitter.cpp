#include "SoundEmitter.h"

SoundEmitter::SoundEmitter(void)
{
	Reset();
}

SoundEmitter::SoundEmitter(Sound* s)
{
	Reset();
	SetSound(s);
}

void	SoundEmitter::Reset()
{
	priority = SOUNDPRIORTY_LOW;
	pitch = 1.0f;
	volume = 1.0f;
	radius = 500.0f;
	timeLeft = 0.0f;
	isLooping = true;
	oalSource = NULL;
	sound = NULL;
	position = Vec3();
}

SoundEmitter::~SoundEmitter(void)
{
	DetachSource();
}

bool		SoundEmitter::CompareNodesByPriority(SoundEmitter* a, SoundEmitter* b)
{
	return (a->priority > b->priority) ? true : false;
}

void		SoundEmitter::SetSound(Sound* s)
{
	sound = s;
	DetachSource();
	if (sound)
	{
		timeLeft = sound->GetLength();
	}
}

void		SoundEmitter::AttachSource(OALSource* s)
{
	oalSource = s;

	if (!oalSource)
	{
		return;
	}

	oalSource->inUse = true;

	alSourceStop(oalSource->source);
	alSourcef(oalSource->source, AL_MAX_DISTANCE, radius);
	alSourcef(oalSource->source, AL_REFERENCE_DISTANCE, radius * 0.2f);
	alSourcei(oalSource->source, AL_BUFFER, sound->GetBuffer());
	alSourcef(oalSource->source, AL_SEC_OFFSET, (sound->GetLength() / 1000.0) - (timeLeft / 1000.0));

	std::cout << "Attaching! Timeleft: " << (sound->GetLength() / 1000.0) - (timeLeft / 1000.0) << std::endl;
	alSourcePlay(oalSource->source);
}

void		SoundEmitter::DetachSource()
{
	if (!oalSource)
	{
		return;
	}

	oalSource->inUse = false;

	alSourcef(oalSource->source, AL_GAIN, 0.0f);
	alSourceStop(oalSource->source);
	alSourcei(oalSource->source, AL_BUFFER, 0);

	oalSource->inUse = false;
	oalSource = NULL;
}

void		SoundEmitter::Update(float msec)
{
	timeLeft -= msec;
	

	while (isLooping && timeLeft < 0.0f) {
		timeLeft += sound->GetLength();
	}

	if (oalSource) {
		alSourcef(oalSource->source, AL_LOOPING, isLooping ? 1 : 0);
		alSourcef(oalSource->source, AL_MAX_DISTANCE, radius);
		alSourcef(oalSource->source, AL_REFERENCE_DISTANCE, radius*0.2f);
	}
}

void SoundEmitter::SetPosition(const Vec3& pos)
{
	position = pos;
	alSourcefv(oalSource->source, AL_POSITION, (float*)&position);
}
