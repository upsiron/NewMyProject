#include "ComputeShader.h"
#include "Shader.h"

void ComputeShader::Initialize(ParticleSystem* _ParticleSystem)
{
	particleSystem =_ParticleSystem;

	// �R���s���[�g�E�V�F�[�_�̍쐬
	CreateShader();

	// ���\�[�X�̍쐬�i�e��f�[�^�o�b�t�@�j
	CreateResource();

	// �V�F�[�_ ���\�[�X �r���[�̍쐬�i���͗p�r���[�j
	CreateSRV();

	// �A���I�[�_�[�h�E�A�N�Z�X�E�r���[�̍쐬�@�i�o�͗p�r���[�j
	CreateUAV();



}

void ComputeShader::Update()
{
	// �R���s���[�g�E�V�F�[�_���g�������Z
	RunComputeShader();
	return;
}

//�R���s���[�g�V�F�[�_�̍쐬
void ComputeShader::CreateShader(void)
{
	HRESULT hr = S_OK;
	hr = create_cs_from_cso(device, "Shaders/PointSprite_cs.cso", &pComputeShader.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return;
}

//���̓��\�[�X�o�b�t�@�A�o�̓��\�[�X�o�b�t�@�ACPU����̃��[�h�p�o�b�t�@���̍쐬
void ComputeShader::CreateResource(void)
{

	HRESULT hr/* = S_OK*/;

	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.ByteWidth = sizeof(ParticleSystem::ParticleData) * particleSystem->NumParticles; // �o�b�t�@ �T�C�Y
	Desc.Usage = D3D11_USAGE_DEFAULT;//�X�e�[�W�̓��o�͂�OK�BGPU�̓��o��OK�B
	Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // �\�����o�b�t�@
	Desc.StructureByteStride = sizeof(ParticleSystem::ParticleData);

	D3D11_SUBRESOURCE_DATA SubResource;//�T�u���\�[�X�̏������p�f�[�^���`
	SubResource.pSysMem = particleSystem->data;//
	SubResource.SysMemPitch = 0;
	SubResource.SysMemSlicePitch = 0;

	//Particle
	// �ŏ��̓��̓��\�[�X(�f�[�^������������)
	hr = device->CreateBuffer(&Desc, &SubResource, &pBufferInput.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �ŏ��̏o�̓��\�[�X�i�������p�f�[�^�͕K�v�Ȃ��j
	hr = device->CreateBuffer(&Desc, NULL, &pCpuBufferOutput.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//HRESULT hr = S_OK;

	//D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.ByteWidth = sizeof(VERTEX) * particleSystem->NumParticles; // �o�b�t�@ �T�C�Y
	Desc.Usage = D3D11_USAGE_DEFAULT;//�X�e�[�W�̓��o�͂�OK�BGPU�̓��o��OK�B
	Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // �\�����o�b�t�@
	Desc.StructureByteStride = sizeof(VERTEX);

	struct VERTEX* posVertex = new VERTEX[particleSystem->NumParticles];

	D3D11_SUBRESOURCE_DATA VerSubResource;//�T�u���\�[�X�̏������p�f�[�^���`
	VerSubResource.pSysMem = posVertex;//
	VerSubResource.SysMemPitch = 0;
	VerSubResource.SysMemSlicePitch = 0;


	// �ŏ��̏o�̓��\�[�X�i�������p�f�[�^�͕K�v�Ȃ��j
	hr = device->CreateBuffer(&Desc, NULL, &pBufferOutput.p);
	delete[] posVertex;
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//--------------------------------------------------------
	// ���[�h�o�b�N�p�o�b�t�@ ���\�[�X�̍쐬�iCPU������A�N�Z�X�ł���X�e�[�W���O�o�b�t�@�j
	//--------------------------------------------------------
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.ByteWidth = sizeof(ParticleSystem::ParticleData) * particleSystem->NumParticles;	// �o�b�t�@ �T�C�Y

	Desc.Usage = D3D11_USAGE_STAGING;  // CPU����ǂݏ����\�ȃ��\�[�X
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // CPU����ǂݍ���
	Desc.StructureByteStride = sizeof(ParticleSystem::ParticleData);

	hr = device->CreateBuffer(&Desc, NULL, &pToCpuReadBackBuffer.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//--------------------------------------------------------
	// �������ݗp�o�b�t�@ ���\�[�X�̍쐬�iCPU������A�N�Z�X�ł���X�e�[�W���O�o�b�t�@�j
	//--------------------------------------------------------
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.ByteWidth = sizeof(ParticleSystem::ParticleData) * particleSystem->NumParticles;	// �o�b�t�@ �T�C�Y

	//Desc.Usage = D3D11_USAGE_STAGING;  // CPU����ǂݏ����\�ȃ��\�[�X
	Desc.Usage = D3D11_USAGE_DYNAMIC;  // CPU����ǂݏ����\�ȃ��\�[�X
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU����ǂݍ���
	Desc.StructureByteStride = sizeof(ParticleSystem::ParticleData);

	hr = device->CreateBuffer(&Desc, NULL, &pToCpuWriteBuffer.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	{
		// ���_�o�b�t�@�̒�`�@�i�������݂ɂ��Ή��j
		D3D11_BUFFER_DESC vBufferDesc;
		vBufferDesc.Usage = D3D11_USAGE_DEFAULT;      // �f�t�H���g�g�p�@
		vBufferDesc.ByteWidth = sizeof(VERTEX) * particleSystem->NumParticles; // PerticleCount = ���_��
		vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���_�o�b�t�@�ƃX�g���[���o�͂Ƃ��Ďg��
		vBufferDesc.CPUAccessFlags = 0;
		vBufferDesc.MiscFlags = 0;
		vBufferDesc.StructureByteStride = 0;

		// ���_�o�b�t�@�̃T�u���\�[�X�̏����l(���_���W)
		struct VERTEX* posVertex = new VERTEX[particleSystem->NumParticles];

		// ���_�o�b�t�@�̃T�u���\�[�X�̒�`(�o�b�t�@�[���\�[�X�ł���������Ă�j
		D3D11_SUBRESOURCE_DATA vSubData;//�������p�f�[�^���쐬
		vSubData.pSysMem = posVertex;  // �o�b�t�@�E�f�[�^�̏����l
		vSubData.SysMemPitch = 0;
		vSubData.SysMemSlicePitch = 0;

		// �`��p���_���̓o�b�t�@�̍쐬�@�@�������p�f�[�^�𑗂��Ă���
		hr = device->CreateBuffer(&vBufferDesc, &vSubData, &pBufferDrawFrom.p);
		delete[] posVertex;
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	return;
}

//�V�F�[�_ ���\�[�X �r���[�̍쐬�i���͗p�r���[�j
void ComputeShader::CreateSRV(void)
{
	HRESULT hr = S_OK;

	D3D11_SHADER_RESOURCE_VIEW_DESC DescSRV;
	ZeroMemory(&DescSRV, sizeof(DescSRV));
	DescSRV.Format = DXGI_FORMAT_UNKNOWN;
	DescSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	DescSRV.Buffer.ElementWidth = particleSystem->NumParticles; // �f�[�^��

	//Particle Data  �V�F�[�_ ���\�[�X �r���[�̍쐬
	hr = device->CreateShaderResourceView(pBufferInput.p, &DescSRV, &pSRV.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return;
}

//�A���I�[�_�[�h�E�A�N�Z�X�E�r���[�̍쐬�@�o�͗p�r���[
void ComputeShader::CreateUAV(void)
{
	HRESULT hr = S_OK;


	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
	ZeroMemory(&DescUAV, sizeof(DescUAV));
	DescUAV.Format = DXGI_FORMAT_UNKNOWN;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	DescUAV.Buffer.NumElements = particleSystem->NumParticles; // �f�[�^��

    // Particle Data �A���I�[�_�[�h�E�A�N�Z�X�E�r���[�̍쐬
	hr = device->CreateUnorderedAccessView(pBufferOutput.p, &DescUAV, &pUAV.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// Particle Data �A���I�[�_�[�h�E�A�N�Z�X�E�r���[�̍쐬
	hr = device->CreateUnorderedAccessView(pCpuBufferOutput.p, &DescUAV, &pCpuUAV.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return;
}

//�R���s���[�g �V�F�[�_���g�������Z
void ComputeShader::RunComputeShader(void)
{
	HRESULT hr = S_OK;
	
	//���̓f�[�^���o�b�t�@�ɓ����
	D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
	context->Map(pToCpuWriteBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	ParticleSystem::ParticleData* pBuf = (ParticleSystem::ParticleData*)MappedResource.pData;
	memcpy(MappedResource.pData, particleSystem->data, sizeof(ParticleSystem::ParticleData) * particleSystem->NumParticles);
	context->Unmap(pToCpuWriteBuffer, 0);

	context->CopyResource(pBufferInput.p, pToCpuWriteBuffer.p);

	//�萔�o�b�t�@���R���s���[�g�V�F�[�_�[�ɐݒ�
	context->CSSetConstantBuffers(0, 1, &pCBuffer.p);

	// �V�F�[�_��ݒ�
	context->CSSetShader(pComputeShader.p, NULL, 0);

	// �A���I�[�_�[�h�E�A�N�Z�X�E�r���[�̐ݒ�i�o�͗p�j
	context->CSSetUnorderedAccessViews(0, 1, &pUAV.p, NULL);
	context->CSSetUnorderedAccessViews(1, 1, &pCpuUAV.p, NULL);

	// �V�F�[�_ ���\�[�X �r���[�̐ݒ�i���͗p�j
	context->CSSetShaderResources(0, 1, &pSRV.p);

	// �R���s���[�g�E�V�F�[�_�̎��s
	context->Dispatch(2000, 1, 1);//�O���[�v�̐�

	//GPU���̌��ʂ�CPU���烊�[�h�\�ȃX�e�[�W���O�o�b�t�@�ɓ]��
	context->CopyResource(pToCpuReadBackBuffer.p, pCpuBufferOutput.p);

	//vertexbuffer�ɓ����buffer�ɏo�͂���buffer���R�s�[
	context->CopyResource(pBufferDrawFrom.p, pBufferOutput.p);

	//�o�̓f�[�^���o�b�t�@�ɓ����
	D3D11_MAPPED_SUBRESOURCE MappedResourceResult = { 0 };
	context->Map(pToCpuReadBackBuffer, 0, D3D11_MAP_READ, 0, &MappedResourceResult);
	ParticleSystem::ParticleData* pBufRes = (ParticleSystem::ParticleData*)MappedResourceResult.pData;
	memcpy(particleSystem->data, pBufRes, sizeof(ParticleSystem::ParticleData) * particleSystem->NumParticles);
	context->Unmap(pToCpuReadBackBuffer, 0);

	return;
}
