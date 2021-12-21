#include <stdio.h>
#include "Logger.h"
#include "Misc.h"
#include "Sound.h"

// ���C�u���������N(����̓v���W�F�N�g�ݒ�ł���Ăق���)
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")

// WAVE�^�O�쐬�}�N��
#define MAKE_WAVE_TAG_VALUE(c1, c2, c3, c4)  ( c1 | (c2<<8) | (c3<<16) | (c4<<24) )

// ����
SoundSource::SoundSource(IDirectSound8* ds, const char* filename)
{
	HRESULT hr;

	// wav�t�@�C���ǂݍ���
	Wave wav(filename);

	// WAV �t�H�[�}�b�g���Z�b�g�A�b�v
	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = wav.fmt.channel;
	wfx.nSamplesPerSec = wav.fmt.sampleRate;
	wfx.wBitsPerSample = wav.fmt.quantumBits;
	wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
	wfx.cbSize = sizeof(WAVEFORMATEX);

	// �o�b�t�@�Z�b�g�A�b�v
	DSBUFFERDESC desc;
	memset(&desc, 0, sizeof(DSBUFFERDESC) );
	desc.dwSize			= sizeof(DSBUFFERDESC);
	desc.lpwfxFormat	= &wfx;
	desc.dwBufferBytes	= static_cast<DWORD>(wav.data.size());
	desc.dwFlags		= DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;

	// �T�E���h�o�b�t�@�쐬
	hr = ds->CreateSoundBuffer(&desc, &soundBuffer, NULL);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �o�b�t�@��WAV�f�[�^���R�s�[
	LPVOID buffer1, buffer2;
	DWORD bufferSize1, bufferSize2;
	hr = soundBuffer->Lock(0, desc.dwBufferBytes, &buffer1, &bufferSize1, &buffer2, &bufferSize2, 0);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	if (bufferSize1 > 0)
	{
		::memcpy(buffer1, wav.data.data(), bufferSize1);
	}
	if (bufferSize2 > 0)
	{
		::memcpy(buffer2, wav.data.data() + bufferSize1, bufferSize2);
	}
	hr = soundBuffer->Unlock(buffer1, bufferSize1, buffer2, bufferSize2);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

SoundSource::~SoundSource()
{
	if (soundBuffer != nullptr)
	{
		//soundBuffer->Release();
		soundBuffer = nullptr;
	}
}

void SoundSource::Play(bool loop)
{
	soundBuffer->Stop();	// �Đ����Ă����ꍇ�͒�~
	soundBuffer->SetCurrentPosition(0);	// �Đ��ʒu���ŏ��ɖ߂�

	DWORD priority = 0;	// 0�`0xFFFFFFFF
	HRESULT hr = soundBuffer->Play(0, priority, loop ? DSBPLAY_LOOPING : 0);
	if (FAILED(hr))
	{
		// �������s���A�����T�E���h���������񓯎�����������ƋN���邱�Ƃ�����
		if (hr == E_OUTOFMEMORY) return;

		// �o�b�t�@���X�g�̏ꍇ���X�g�A���Ă݂�
		if (hr == DSERR_BUFFERLOST)
		{
			hr = soundBuffer->Restore();
			if (FAILED(hr)) return;

			// �ēx�Đ����Ă݂�
			soundBuffer->Play(0, priority, loop ? DSBPLAY_LOOPING : 0);
		}
	}
}

void SoundSource::Stop()
{
	soundBuffer->Stop();
}

void SoundSource::SetVolume(float volume)
{
	// �f�V�x���֕ϊ�
	LONG decibel;
	if (volume < 0.002f)
	{
		decibel = DSBVOLUME_MIN;
	}
	else if (volume >= 1.0f)
	{
		decibel = DSBVOLUME_MAX;
	}
	else
	{
		decibel = static_cast<LONG>(33.22f * 100.f * log10f(volume));
	}

	soundBuffer->SetVolume(decibel);
}

void SoundSource::SetPan(float pan)
{
	LONG decibel;
	float absPan = fabsf(pan);
	if (absPan <= 0.002f)
	{
		decibel = DSBPAN_CENTER;
	}
	else if (pan <= -1.0f)
	{
		decibel = DSBPAN_LEFT;
	}
	else if (pan >= 1.0f)
	{
		decibel = DSBPAN_RIGHT;
	}
	else
	{
		decibel = static_cast<LONG>(33.22f * 100.f * log10f(1.0f - absPan));
		if (pan > 0.0f) decibel = -decibel;
	}

	soundBuffer->SetPan(decibel);
}

// �T�E���h�}�l�[�W���[
SoundManager::SoundManager(HWND hWnd)
{
	HRESULT hr;

	// �f�o�C�X�̍쐬
	hr = DirectSoundCreate8(NULL, &directSound, NULL);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = directSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// DSBUFFERDESC �\���̂��Z�b�g�A�b�v
	DSBUFFERDESC desc;
	memset(&desc, 0, sizeof(DSBUFFERDESC));
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = (DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D);

	// �v���C�}���o�b�t�@���쐬����B
	hr = directSound->CreateSoundBuffer(&desc, &primaryBuffer, NULL);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// WAV �t�H�[�}�b�g���Z�b�g�A�b�v
	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;
	wfx.nSamplesPerSec = 44100;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
	wfx.cbSize = sizeof(WAVEFORMATEX);

	primaryBuffer->SetFormat(&wfx);
}

SoundManager::~SoundManager()
{
	if (primaryBuffer != nullptr)
	{
		primaryBuffer->Stop();
		primaryBuffer->Release();
		primaryBuffer = nullptr;
	}
	if (directSound != nullptr)
	{
		directSound->Release();
		directSound = nullptr;
	}
}

std::unique_ptr<SoundSource> SoundManager::CreateSoundSource(const char* filename)
{
	return std::make_unique<SoundSource>(directSound, filename);
}

// WAVE�f�[�^
Wave::Wave(const char* filename)
{
	// WAV�t�@�C���ǂݍ���
	FILE* fp = nullptr;
	errno_t error = fopen_s(&fp, filename, "rb");
	_ASSERT_EXPR_A(error == 0, "WAV File not found");

	// �t�@�C���̃T�C�Y�����߂�
	fseek(fp, 0, SEEK_END);
	size_t size = static_cast<size_t>(ftell(fp));
	fseek(fp, 0, SEEK_SET);

	size_t readBytes = 0;

	// RIFF�w�b�_
	fread(&riff, sizeof(riff), 1, fp);
	readBytes += sizeof(riff);

	// "RIFF" �Ƃ̈�v���m�F
	_ASSERT_EXPR_A(riff.tag == MAKE_WAVE_TAG_VALUE('R', 'I', 'F', 'F'), "not in RIFF format");

	// "WAVE" �Ƃ̈�v���m�F
	_ASSERT_EXPR_A(riff.type == MAKE_WAVE_TAG_VALUE('W', 'A', 'V', 'E'), "not in WAVE format");

	while (size > readBytes)
	{
		Wave::Chunk chunk;
		fread(&chunk, sizeof(chunk), 1, fp);
		readBytes += sizeof(chunk);

		// 'fmt '
		if (chunk.tag == MAKE_WAVE_TAG_VALUE('f', 'm', 't', ' '))
		{
			fread(&fmt, sizeof(fmt), 1, fp);
			readBytes += sizeof(fmt);

			// �g���̈悪����Γǂݎ̂�
			if (chunk.size > sizeof(Wave::Fmt))
			{
				UINT16 extSize;
				fread(&extSize, sizeof(extSize), 1, fp);
				readBytes += sizeof(extSize);

				if (readBytes + chunk.size == size)
				{
					break;
				}

				fseek(fp, extSize, SEEK_CUR);
				readBytes += extSize;
			}
		}
		// 'data'
		else if (chunk.tag == MAKE_WAVE_TAG_VALUE('d', 'a', 't', 'a'))
		{
			data.resize(chunk.size);
			fread(data.data(), chunk.size, 1, fp);
			readBytes += chunk.size;

			// 8-bit wav �t�@�C���̏ꍇ�� unsigned -> signed �̕ϊ����K�v
			if (fmt.quantumBits == 8)
			{
				for(UINT32 i = 0; i < chunk.size; ++i)
				{
					data[i] -= 128;
				}
			}
		}
		// ����ȊO
		else
		{
			if (readBytes + chunk.size == size) break;

			// �ǂݔ�΂�
			fseek(fp, chunk.size, SEEK_CUR);
			readBytes += chunk.size;
		}
	}

	fclose(fp);
}
