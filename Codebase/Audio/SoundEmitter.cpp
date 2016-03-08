#include "SoundEmitter.h"

Emitter::Emitter() {
	Reset();
}

void Emitter::Reset() {
	sound = nullptr;
	currentSource = nullptr;

	priority = SOUNDPRIORTY_LOW;
	volume = SOUND_VOLUME;
	radius = SOUND_RADIUS;
	pitch = SOUND_PITCH;

	isSingle = false;
	isLooping = SOUND_LOOP;
	isGlobal = SOUND_GLOBAL;
}


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
	Emitter::Reset();

	timeLeft = 0.0f;

	// for 3D and doppler
	position = Vec3();
	velocity = Vec3();

	// for streaming
	streamPos = 0;
	for (unsigned int i = 0; i < NUM_STREAM_BUFFERS; ++i)
	{
		streamBuffers[i] = 0;
	}
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
		if (sound->IsStreaming())
		{
			alGenBuffers(NUM_STREAM_BUFFERS, streamBuffers);
		}
		else
		{
			alDeleteBuffers(NUM_STREAM_BUFFERS, streamBuffers);
		}
	}
}

void		SoundEmitter::AttachSource(AudioSource* s)
{
	currentSource = s;

	if (!currentSource)
	{
		return;
	}

	currentSource->inUse = true;

	alSourceStop(currentSource->source);
	alSourcef(currentSource->source, AL_MAX_DISTANCE, radius);
	alSourcef(currentSource->source, AL_REFERENCE_DISTANCE, radius * 0.2f);

	if (sound->IsStreaming())
	{
		streamPos = timeLeft;
		int numBuffered = 0;
		while (numBuffered < NUM_STREAM_BUFFERS)
		{
			double streamed = sound->StreamData(streamBuffers[numBuffered], streamPos);

			if (streamed)
			{
				streamPos -= streamed;
				++numBuffered;
			}
			else
			{
				break;
			}
		}
		alSourceQueueBuffers(currentSource->source, numBuffered, &streamBuffers[0]);
	}
	else
	{
		alSourcei(currentSource->source, AL_BUFFER, sound->GetBuffer());
		alSourcef(currentSource->source, AL_SEC_OFFSET, (sound->GetLength() / 1000.0) - (timeLeft / 1000.0));
	}

	alSourcePlay(currentSource->source);
}

void		SoundEmitter::DetachSource()
{
	if (!currentSource)
	{
		return;
	}

	currentSource->inUse = false;

	alSourcef(currentSource->source, AL_GAIN, 0.0f);
	alSourceStop(currentSource->source);
	alSourcei(currentSource->source, AL_BUFFER, 0);

	if (sound && sound->IsStreaming())
	{
		int numProcessed = 0;
		ALuint tempBuffer;
		alGetSourcei(currentSource->source, AL_BUFFERS_PROCESSED, &numProcessed);
		while (numProcessed--)
		{
			alSourceUnqueueBuffers(currentSource->source, 1, &tempBuffer);
		}
	}

	currentSource = NULL;
}

void		SoundEmitter::Update(float msec)
{
	if (sound)
	{
		timeLeft -= (msec * pitch);

		if (isLooping)
		{
			while (timeLeft < 0)
			{
				timeLeft += sound->GetLength();
				//streamPos += sound->GetLength();
			}
		}

		if (currentSource)
		{
			alSourcef(currentSource->source, AL_GAIN, volume);
			alSourcef(currentSource->source, AL_PITCH, pitch);
			alSourcef(currentSource->source, AL_MAX_DISTANCE, radius);
			alSourcef(currentSource->source, AL_REFERENCE_DISTANCE, radius * 0.2f);

			alSourcefv(currentSource->source, AL_POSITION, (float*)&position);
			alSourcefv(currentSource->source, AL_VELOCITY, (float*)&velocity);

			if (sound->IsStreaming())
			{
				int numProcessed;
				alGetSourcei(currentSource->source, AL_BUFFERS_PROCESSED, &numProcessed);
				alSourcei(currentSource->source, AL_LOOPING, 0);

				while (numProcessed--/* && streamPos > 0*/)  	//The && prevents clipping at the end of sounds!
				{
					ALuint freeBuffer;

					alSourceUnqueueBuffers(currentSource->source, 1, &freeBuffer);

					streamPos -= sound->StreamData(freeBuffer, streamPos);
					alSourceQueueBuffers(currentSource->source, 1, &freeBuffer);

					if (streamPos < 0 && isLooping)
					{
						streamPos += sound->GetLength();
					}
				}
			}
			else
			{
				alSourcei(currentSource->source, AL_LOOPING, isLooping ? 1 : 0);
			}
		}
	}
}