#pragma once
#include "Character.h"

class Obstacle : public Character
{
public:
	Obstacle() {}
	Obstacle(std::shared_ptr<SkinnedMesh>& mesh) :
		Character(mesh) {};
	~Obstacle() override {}

	// �X�V����
	virtual void Update(float elapsedTime) = 0;

	// �`�揈��
	virtual void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe) = 0;

	// �f�o�b�O�G�l�~�[���\��
	virtual void DrawDebugGUI() {};


	void	SetId(int id) { this->id = id; }
	int		GetId() { return id; }

	void	SetExistFlg(bool flg) { ExistFlg = flg; };
	bool    GetExistFlg() { return ExistFlg; }

	
	//��Q���̏�ʂ�4�_�����ϐ�
	DirectX::XMFLOAT3 ObstacleUpLeftTop;
	DirectX::XMFLOAT3 ObstacleUpRightTop;
	DirectX::XMFLOAT3 ObstacleUpLeftBottom;
	DirectX::XMFLOAT3 ObstacleUpRightBottom;
	//��Q���̉��ʂ�4�_�����ϐ�
	DirectX::XMFLOAT3 ObstacleDownLeftTop;
	DirectX::XMFLOAT3 ObstacleDownRightTop;
	DirectX::XMFLOAT3 ObstacleDownLeftBottom;
	DirectX::XMFLOAT3 ObstacleDownRightBottom;
protected:
	int	state = 0;
	int id = 0;

private:
	float ExistFlg = false;
};