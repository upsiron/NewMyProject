#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>
#include <memory>
#include "texture.h"
#include "shader.h"
#include "ComputeShader.h"

class Texture;
class Shader;
class ComputeShader;
class ParticleSystem
{
private:
	struct VERTEX
	{
		DirectX::XMFLOAT3 Pos;		//�ʒu
		DirectX::XMFLOAT3 Normal;	//�@��
		DirectX::XMFLOAT2 Tex;		//UV���W
		DirectX::XMFLOAT4 Color;	//���_�F
		DirectX::XMFLOAT4 Param;	//�ėp�p�����[�^
	};
	VERTEX* v;			//���_�f�[�^
	

	// ���_�f�[�^
	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
	//�e�N�X�`�����p
	std::shared_ptr<Texture> texture = nullptr;
	// �萔�o�b�t�@�̃f�[�^��`
	struct ConstantBufferForPerFrame {
		DirectX::XMFLOAT4X4	view;			// �r���[�ϊ��s��
		DirectX::XMFLOAT4X4	projection;		// �����ϊ��s��
	};
	// �ǉ�
	D3D11_TEXTURE2D_DESC ColorTexture2dDesc;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ColorTexture;

	std::unique_ptr<ComputeShader> computeShader;


public:

	ParticleSystem(ID3D11Device* device, int num = 1000);
	ParticleSystem(ID3D11Device* device, std::shared_ptr<Texture> texture, int num = 1000);
	~ParticleSystem() { delete[] data; delete[] v; }
	void Update(float elapsed_time);

	void Animation(float elapsed_time, float speed = 0.1f);

	void Render(ID3D11DeviceContext* immediate_context, Shader* shader, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, ID3D11BlendState* state);

	void Set(
		int type,
		int index,
		float timer,
		DirectX::XMFLOAT3 p,
		DirectX::XMFLOAT3 v = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3 f = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3 c = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT2 size = DirectX::XMFLOAT2(1.0f, 1.0f)
	);

	//����g����������Ȃ������݂͎g��Ȃ��̂ŃR�����g�A�E�g
	/*void Snow(DirectX::XMFLOAT3 pos, int max);
	void Spark(DirectX::XMFLOAT3 pos, int max);
	void Fire(DirectX::XMFLOAT3 pos, int max);
	void Smoke(DirectX::XMFLOAT3 pos, int max);*/

	void Star(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 velocity, int max);
	void hanabi(DirectX::XMFLOAT3 pos, int max);
	void Sphere(DirectX::XMFLOAT3 pos, int max);

	struct ParticleData
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 size;
		DirectX::XMFLOAT2 sizeA;
		DirectX::XMFLOAT3 posV;
		DirectX::XMFLOAT3 posA;
		DirectX::XMFLOAT3 color;
		float alpha;
		float timer;
		float animeTimer;
		float elapsedTime;
		int type;
	};
	ParticleData* data; //�p�[�e�B�N���f�[�^

	int numParticles;

	//�ԉΊ֘A�ϐ�
	float particleAngle;
	float hanabiX = 0;
	float hanabiY = 0;
	float hanabiZ = 0;
	float forceRand = 0;
	float angleRand = 0;
	const float dangle = DirectX::XMConvertToRadians(10.0f);

	DirectX::XMFLOAT3 hanabiColor[7]
	{
		{1.0f,1.0f,1.0f},
		{1.0f,0.0f,0.0f},//red
		{0.0f,1.0f,0.0f},//green
		{1.0f,0.0f,1.0f},//bule
		{1.0f,1.0f,0.0f},//yellow
		{1.0f,0.0f,1.0f},//purple
		{0.0f,1.0f,1.0f}
	};
	
	int hanabiRand;
};