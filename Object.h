#pragma once
#include "Character.h"

class Object : public Character
{
public:
	Object();
	Object(std::shared_ptr<SkinnedMesh>& mesh) :
		Character(mesh) {};
	~Object() override {}

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
	DirectX::XMFLOAT3 ObstacleUpLeftTop = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 ObstacleUpRightTop = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 ObstacleUpLeftBottom = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 ObstacleUpRightBottom = { 0.0f,0.0f,0.0f };
	//��Q���̉��ʂ�4�_�����ϐ�
	DirectX::XMFLOAT3 ObstacleDownLeftTop = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 ObstacleDownRightTop = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 ObstacleDownLeftBottom = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 ObstacleDownRightBottom = { 0.0f,0.0f,0.0f };

	//�R�C���㏸�p
	float movePosition = 0.0f;
	void SetMovePosition(float position) { movePosition = position; }
	//�R�C����]�p
	float moveAngle = 0.1f;
	void SetMoveAngle(float angle) { moveAngle = angle; }

protected:

	int	state = 0;
	int id = 0;

private:
	float ExistFlg = false;
};