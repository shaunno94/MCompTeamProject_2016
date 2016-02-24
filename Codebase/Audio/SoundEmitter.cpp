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
	pitch = SOUND_PITCH;
	volume = SOUND_VOLUME;
	radius = SOUND_RADIUS;
	timeLeft = 0.0f;
	isLooping = SOUND_LOOP;
	oalSource = NULL;
	sound = NULL;
	position = Vec3();

	streamPos = 0;
	isGlobal = SOUND_GLOBAL;

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
		alSourceQueueBuffers(oalSource->source, numBuffered, &streamBuffers[0]);
	}
	else
	{
		alSourcei(oalSource->source, AL_BUFFER, sound->GetBuffer());
		alSourcef(oalSource->source, AL_SEC_OFFSET, (sound->GetLength() / 1000.0) - (timeLeft / 1000.0));
	}

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

	if (sound && sound->IsStreaming())
	{
		int numProcessed = 0;
		ALuint tempBuffer;
		alGetSourcei(oalSource->source, AL_BUFFERS_PROCESSED, &numProcessed);
		while (numProcessed--)
		{
			alSourceUnqueueBuffers(oalSource->source, 1, &tempBuffer);
		}
	}

	oalSource = NULL;
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

		if (oalSource)
		{
			alSourcef(oalSource->source, AL_GAIN, volume);
			alSourcef(oalSource->source, AL_PITCH, pitch);
			alSourcef(oalSource->source, AL_MAX_DISTANCE, radius);
			alSourcef(oalSource->source, AL_REFERENCE_DISTANCE, radius * 0.2f);

			alSourcefv(oalSource->source, AL_POSITION, (float*)&position);

			if (sound->IsStreaming())
			{
				int numProcessed;
				alGetSourcei(oalSource->source, AL_BUFFERS_PROCESSED, &numProcessed);
				alSourcei(oalSource->source, AL_LOOPING, 0);

				while (numProcessed--/* && streamPos > 0*/)  	//The && prevents clipping at the end of sounds!
				{
					ALuint freeBuffer;

					alSourceUnqueueBuffers(oalSource->source, 1, &freeBuffer);

					streamPos -= sound->StreamData(freeBuffer, streamPos);
					alSourceQueueBuffers(oalSource->source, 1, &freeBuffer);

					if (streamPos < 0 && isLooping)
					{
						streamPos += sound->GetLength();
					}
				}
			}
			else
			{
				alSourcei(oalSource->source, AL_LOOPING, isLooping ? 1 : 0);
			}
		}
	}
}
