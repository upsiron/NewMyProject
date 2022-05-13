#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Framework.h"
#include "ParticleSystem.h"
//CComPtr�֘A
#include <atlbase.h>
#include <atlcom.h>
#include <atlcoll.h>
#include <atlstr.h>

class ParticleSystem;

class ComputeShader
{
private:
	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();
	ID3D11DeviceContext* context = framework.GetImmediateContext();
public:
	ParticleSystem* particleSystem;
	void CreateShader(void);									    //�V�F�[�_�̍쐬(�V�F�[�_�[�쐬)
	void CreateResource(void);									    //���\�[�X�̍쐬(�o�b�t�@�m��)
	void CreateSRV(void);										    //�V�F�[�_ ���\�[�X �r���[�̍쐬�i���̓r���[�j
	void CreateUAV(void);										    //�A���I�[�_�[�h�E�A�N�Z�X�E�r���[�̍쐬�i�o�̓r���[�j
	void RunComputeShader(void);								    //�R���s���[�g �V�F�[�_���g�������Z


	CComPtr <ID3D11ComputeShader> pComputeShader = NULL;			// �R���s���[�g�E�V�F�[�_
	CComPtr <ID3D11UnorderedAccessView> pUAV = NULL;
	CComPtr <ID3D11UnorderedAccessView> pCpuUAV = NULL;
	CComPtr <ID3D11ShaderResourceView>  pSRV = NULL;
	CComPtr <ID3D11Buffer> pToCpuReadBackBuffer = NULL;
	CComPtr <ID3D11Buffer> pToCpuWriteBuffer = NULL;
	CComPtr <ID3D11Buffer> pBufferInput = NULL;
	CComPtr <ID3D11Buffer> pBufferOutput = NULL;
	CComPtr <ID3D11Buffer> pCpuBufferOutput = NULL;
	CComPtr <ID3D11Buffer> pBufferDrawFrom = NULL;

	CComPtr <ID3D11Buffer> pCBuffer = NULL;						    // �萔�o�b�t�@

	// �V�F�[�_�̃R���p�C�� �I�v�V����
#if defined(DEBUG) || defined(_DEBUG)
	UINT g_flagCompile = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION
		| D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#else
	UINT g_flagCompile = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#endif

	struct VERTEX
	{
		DirectX::XMFLOAT3 Pos;	//�ʒu
		DirectX::XMFLOAT3 Normal;//�@��
		DirectX::XMFLOAT2 Tex;	//UV���W
		DirectX::XMFLOAT4 Color;	//���_�F
		DirectX::XMFLOAT4 Param;	//�ėp�p�����[�^
	};
	VERTEX* v;			//���_�f�[�^

public:
	ComputeShader() {};
	~ComputeShader()
	{
		//delete device;
		//device = nullptr;
		//delete context;
		//context = nullptr;
		
		//delete particleSystem;
		//particleSystem = nullptr;
		delete v;
		v = nullptr;
	}
	void Initialize(ParticleSystem* ParticleSystem);
	void Update();

	DirectX::XMFLOAT3 data;
};
