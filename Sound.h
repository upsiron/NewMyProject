#pragma once

#include <memory>
#include <vector>
#include <dsound.h>

// ����
class SoundSource
{
public:
	SoundSource(IDirectSound8* ds, const char* filename);
	~SoundSource();

	void Play(bool loop);
	void Stop();
	void SetVolume(float volume);	// 0.0 �` 1.0
	void SetPan(float pan);			// -1.0[L] �` 1.0[R]

private:
	IDirectSoundBuffer*		soundBuffer = nullptr;
};

// �T�E���h�}�l�[�W���[
class SoundManager
{
public:
	SoundManager(HWND hWnd);
	~SoundManager();

	std::unique_ptr<SoundSource> CreateSoundSource(const char* filename);

private:
	IDirectSound8*			directSound = nullptr;
	IDirectSoundBuffer*		primaryBuffer = nullptr;
};

// WAVE�f�[�^
struct Wave
{
	Wave(const char* filename);

	// RIFF�w�b�_
	struct Riff
	{
		UINT32				tag;			// RIFF�`���̎��ʎq 'RIFF'
		UINT32				size;			// ����ȍ~�̃t�@�C���T�C�Y(�t�@�C���T�C�Y - 8)
		UINT32				type;			// RIFF�̎�ނ�\�����ʎq 'WAVE'
	};

	// �`�����N
	struct Chunk
	{
		UINT32				tag;			// �`�����N�`���̎��ʎq 'fmt ' 'data'
		UINT32				size;			// �f�[�^�T�C�Y('fmt '���j�APCM�Ȃ��16 'data'�g�`�f�[�^�T�C�Y)
	};

	// fmt �`�����N
	struct Fmt
	{
		UINT16				fmtId;			// �t�H�[�}�b�gID(���j�APCM�Ȃ��1)
		UINT16				channel;		// �`�����l����(���m����:1 �X�e���I:2)
		UINT32				sampleRate;		// �T���v�����O���[�g(44.1kHz�Ȃ�44100)
		UINT32				transRate;		// �f�[�^���x(Byte/sec) [ 44.1kHz 16bit �X�e���I 44100�~2�~2 ]
		UINT16				blockSize;		// �u���b�N�T�C�Y(Byte/sample�~�`�����l����)
		UINT16				quantumBits;	// �T���v��������̃r�b�g��(bit/sample)
	};

	Riff					riff;
	Fmt						fmt;
	std::vector<char>		data;
};
