//#include "DxSystem.h"
#include "ParticleSystem.h"
#include "Player.h"

//������
ParticleSystem::ParticleSystem(ID3D11Device* device, int num)
{
	data = new ParticleData[num];
	ZeroMemory(data, sizeof(ParticleData) * num);
	v = new VERTEX[num];
	ZeroMemory(v, sizeof(VERTEX) * num);

	numParticles = num;
	for (int i = 0; i < numParticles; i++) { data[i].type = -1; }
	//�p�[�e�B�N���쐬�Ɖ摜���[�h
	load_texture_from_file(device, L"Data/Particle/particle256x256.png", ColorTexture.GetAddressOf(), &ColorTexture2dDesc);


	//���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	//���_�����̃o�b�t�@
	bd.ByteWidth = sizeof(VERTEX) * numParticles;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, VertexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBufferForPerFrame);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;

	hr = device->CreateBuffer(&cbd, nullptr, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	computeShader = std::make_unique<ComputeShader>();
	computeShader->Initialize(this);
}

//�����ɉ摜��ݒ肵�ď�����
ParticleSystem::ParticleSystem(ID3D11Device* device, std::shared_ptr<Texture> texture, int num)
{
	data = new ParticleData[num];
	ZeroMemory(data, sizeof(ParticleData) * num);
	v = new VERTEX[num];
	ZeroMemory(v, sizeof(VERTEX) * num);

	numParticles = num;
	for (int i = 0; i < numParticles; i++) { data[i].type = -1; }
	//�p�[�e�B�N���쐬�Ɖ摜�ݒ�
	this->texture = texture;

	//	���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	//���_�����̃o�b�t�@
	bd.ByteWidth = sizeof(VERTEX) * numParticles;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, VertexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBufferForPerFrame);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;

	hr = device->CreateBuffer(&cbd, nullptr, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	computeShader = std::make_unique<ComputeShader>();
	computeShader->Initialize(this);
}


void ParticleSystem::Animation(float elapsed_time, float speed)
{
	for (int i = 0; i < numParticles; i++) {
		if (data[i].type < 0) continue;

		data[i].animeTimer += elapsed_time;
	}
}

void ParticleSystem::Update(float elapsed_time)
{	
	for (int i = 0; i < numParticles; i++)
	{
		data[i].elapsedTime = elapsed_time;
	}
	computeShader->Update();
}

void ParticleSystem::Render(ID3D11DeviceContext* immediate_context, Shader* shader, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, ID3D11BlendState* state)
{
	//�V�F�[�_�L����
	shader->Activate(immediate_context);

	//�萔�o�b�t�@�̍X�V
	ConstantBufferForPerFrame cb;
	cb.view = view;
	cb.projection = projection;
	immediate_context->UpdateSubresource(ConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	immediate_context->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	immediate_context->GSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	immediate_context->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());

	//�v���~�e�B�u�E�g�|���W�[���Z�b�g
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//�e�N�X�`���ݒ�
	immediate_context->PSSetShaderResources(0, 1, ColorTexture.GetAddressOf());

	immediate_context->OMSetBlendState(state, nullptr, 0xFFFFFFFF);


	int n = 0; //�p�[�e�B�N��������
	for (int i = 0; i < numParticles; i++)
	{
		if (data[i].type < 0) continue;
		++n;
	}

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, &computeShader->pBufferDrawFrom.p, &stride, &offset);

	//�����_�����O(�C���f�b�N�X�t��)
	immediate_context->Draw(n, 0);

	//�V�F�[�_������
	shader->Disactivate(immediate_context);
}

//void ParticleSystem::Set(int type, float timer,
//	DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v, DirectX::XMFLOAT3 f, DirectX::XMFLOAT2 size)
//{
//	for (int i = 0; i < numParticles; i++) {
//		if (data[i].type >= 0) continue;
//		data[i].type = type;
//		data[i].pos.x = p.x;
//		data[i].pos.y = p.y;
//		data[i].pos.z = p.z;
//		data[i].posV.x = v.x;
//		data[i].posV.y = v.y;
//		data[i].posV.z = v.z;
//		data[i].posA.x = f.x;
//		data[i].posA.y = f.y;
//		data[i].posA.z = f.z;
//		data[i].size.x = size.x;
//		data[i].size.y = size.y;
//		data[i].alpha = 1.0f;
//		data[i].timer = timer;
//		break;
//	}
//}

//�R���s���[�g�V�F�[�_�[�Ōv�Z���邽�߂ɑ��鐔�l���Z�b�g
void ParticleSystem::Set(int type, int index, float timer, DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v, DirectX::XMFLOAT3 f, DirectX::XMFLOAT3 c, DirectX::XMFLOAT2 size)
{
	data[index].type = type;
	data[index].pos.x = p.x;
	data[index].pos.y = p.y;
	data[index].pos.z = p.z;
	data[index].posV.x = v.x;
	data[index].posV.y = v.y;
	data[index].posV.z = v.z;
	data[index].posA.x = f.x;
	data[index].posA.y = f.y;
	data[index].posA.z = f.z;
	data[index].color.x = c.x;
	data[index].color.y = c.y;
	data[index].color.z = c.z;
	data[index].size.x = size.x;
	data[index].size.y = size.y;
	data[index].alpha = 1.0f;
	data[index].timer = timer;
}


//����g����������Ȃ��p�[�e�B�N���̓����A���݂͎g��Ȃ��̂ŃR�����g�A�E�g

//void ParticleSystem::Snow(DirectX::XMFLOAT3 pos, int max)
//{
//	for (int i = 0; i < max; i++) {
//		//�����ʒu
//		DirectX::XMFLOAT3 p = { 0,0,0 };
//		p.x = pos.x + (rand() % 10001 - 5000) * 0.01f;
//		p.y = pos.y;
//		p.z = pos.z + (rand() % 10001 - 5000) * 0.01f;
//		//��������
//		DirectX::XMFLOAT3 v = { 0,0,0 };
//		v.y = -(rand() % 10001) * 0.0002f - 0.002f;
//		//��
//		DirectX::XMFLOAT3 f = { 0,0,0 };
//		f.x = (rand() % 10001) * 0.00001f + 0.1f;
//		f.z = (rand() % 10001 - 5000) * 0.00001f;
//		//�傫��
//		DirectX::XMFLOAT2 s = { .2f,.2f };
//
//		Set(12, 5, p, v, f, s);
//	}
//}
//
//void ParticleSystem::Spark(DirectX::XMFLOAT3 pos, int max)
//{
//	for (int i = 0; i < max; i++) {
//		DirectX::XMFLOAT3 p;
//		p.x = pos.x;
//		p.y = pos.y;
//		p.z = pos.z;
//
//		DirectX::XMFLOAT3 v = { 0,0,0 };
//		v.x = (rand() % 10001 - 5000) * 0.0001f;
//		v.y = (rand() % 10001) * 0.0002f + 1.2f;
//		v.z = (rand() % 10001 - 5000) * 0.0001f;
//
//		DirectX::XMFLOAT3 f = { 0,-1.2f,0 };
//		DirectX::XMFLOAT2 s = { 0.05f,0.05f };
//
//		Set(2, 3, p, v, f, s);
//
//	}
//}
//
//void ParticleSystem::Fire(DirectX::XMFLOAT3 pos, int max)
//{
//	for (int i = 0; i < max; i++) {
//
//		data[i].type = 2;
//		data[i].pos.x = pos.x + (rand() % 10001 - 5000) * 0.00005f;
//		data[i].pos.y = pos.y + (rand() % 10001) * 0.0001f + 0.2f;
//		data[i].pos.z = pos.z + (rand() % 10001 - 5000) * 0.00005f;
//		data[i].posV.x = (rand() % 10001 - 5000) * 0.0002f;
//		data[i].posV.y = (rand() % 10001) * 0.0004f + 0.005f;
//		data[i].posV.z = (rand() % 10001 - 5000) * 0.0002f;
//		data[i].posA.x = 0;
//		data[i].posA.y = -2.0f;
//		data[i].posA.z = 0;
//		data[i].size.x = 0.05f;
//		data[i].size.y = 0.05f;
//		data[i].alpha = 1.0f;
//		data[i].timer = 4.0f;
//	}
//}
//
//void ParticleSystem::Smoke(DirectX::XMFLOAT3 pos, int max)
//{
//	for (int i = 0; i < max; i++) {
//
//		data[i].type = 2;
//		data[i].pos.x = pos.x + (rand() % 20001 - 5000) * 0.01f;
//		data[i].pos.y = pos.y + (rand() % 20001 - 5000) * 0.01f;
//		data[i].pos.z = pos.z + (rand() % 20001 - 5000) * 0.01f;
//		data[i].posV.x = sinf(particleAngle);
//		data[i].posV.y = cosf(particleAngle);
//		data[i].posV.z = 0;
//		data[i].posA.x = 0;
//		data[i].posA.y = 0;
//		data[i].posA.z = 0;
//		data[i].color.x = 1;
//		data[i].color.y = 1;
//		data[i].color.z = 1;
//		data[i].size.x = 0.05f;
//		data[i].size.y = 0.05f;
//		data[i].alpha = 1.0f;
//		data[i].timer = 4.0f;
//	}
//}

//�w�i�X�^�[�̓���
void ParticleSystem::Star(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 velocity, int max)
{
	for (int i = 0; i < max; i++) {
		if (data[i].timer <= 0)
		{
			//�p�[�e�B�N���X�^�[�Z�b�g
			Set(2,                                                  //type
				i,                                                  //index
				float(rand() % 20),								    //timer
				{
					pos.x + (rand() % 20001 - 5000) * 0.01f,		//position
					pos.y + (rand() % 20001 - 5000) * 0.01f,
					pos.z + (rand() % 20001 - 5000) * 0.01f
				},
				velocity,                                           //velocity
				{ 0.0f,0.0f,0.0f },                                 //force
				{ 1.0f,1.0f,1.0f },                                 //color
				{ 0.05f,0.05f });                                   //size
		}
	}
}

void ParticleSystem::Star(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 velocity, int max, DirectX::XMFLOAT3 color)
{
	for (int i = 0; i < max; i++) {
		if (data[i].timer <= 0)
		{
			//�p�[�e�B�N���X�^�[�Z�b�g
			Set(2,                                                  //type
				i,                                                  //index
				float(rand() % 20),								    //timer
				{
					pos.x + (rand() % 20001 - 5000) * 0.01f,		//position
					pos.y + (rand() % 20001 - 5000) * 0.01f,
					pos.z + (rand() % 20001 - 5000) * 0.01f
				},
				velocity,                                           //velocity
				{ 0.0f,0.0f,0.0f },                                 //force
				color,												//color
				{ 0.05f,0.05f });                                   //size
		}
	}
}

//���U���g�ԉ΂̓���
void ParticleSystem::hanabi(DirectX::XMFLOAT3 pos, int max)
{
	hanabiX = (rand() % 10001 - 6001) * 0.01f;
	hanabiY = (rand() % 10001 - 6001) * 0.01f;
	hanabiZ = float(rand() % 50 + 30);
	hanabiRand = rand() % 7;
	for (int i = 0; i < max; i++) {
		if (data[i].timer <= 0)
		{
			forceRand = (rand() % 5001) * 0.002f - 0.01f;
			angleRand = DirectX::XMConvertToRadians(rand() % 360);
			Set(2,                                                                 //type
				i,                                                                 //index
				float(rand() % 5),                                                 //timer
				{ hanabiX,hanabiY,hanabiZ },                                       //position
				{ cosf(angleRand) * forceRand, sinf(angleRand) * forceRand, 0.0f },//velocity
				{ 0.0f,-3.2f,0.0f },                                               //force
				{ hanabiColor[hanabiRand] },                                       //color
				{ 0.05f,0.05f });                                                  //size
		}
	}
}

//
void ParticleSystem::Fire(DirectX::XMFLOAT3 pos, int max)
{
	for (int i = 0; i < max; i++) {
		if (data[i].timer <= 0)
		{
			Set(2,                                                  //type
				i,                                                  //index
				4.0f,												//timer
				{
					pos.x + (rand() % 10001 - 5000) * 0.0002f,	//position
					pos.y /*+ (rand() % 10001) * 0.0001f + 0.2f*/,
					pos.z + (rand() % 10001 - 5000) * 0.0002f
				},
				{
					0.0f,//(rand() % 10001 - 5000) * 0.0002f,              //velocity
					(rand() % 10001) * 0.0002f + 1.2f,
					0.0f,//(rand() % 10001 - 5000) * 0.0002f,
				},
				{ 0.0f,0.0,0.0f },                                //force
				{ 0.0f,1.0f,0.0f },                                 //color
				{ 0.1f,0.1f });                                     //size
		}
	}
}

void ParticleSystem::Sphere(DirectX::XMFLOAT3 pos, int max)
{
	for (int i = 0; i < max; i++) {
		if (data[i].timer <= 0)
		{
			angleRand = DirectX::XMConvertToRadians(rand() % 360);
			Set(2,													//type
				i,													//index
				float(rand() % 7),									//timer
				pos,												//position
				{ cosf(angleRand) * 2, sinf(angleRand) * 2, 0.0f },	//velocity
				{ 0.0f,0.0f,0.0f },									//force
				{ hanabiColor[6] },									//color
				{ 0.05f,0.05f });									//size
		}
	}
}

