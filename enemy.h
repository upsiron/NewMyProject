#pragma once
#include "Character.h"
//#include "Telegram.h"
//#include "DebugRenderer.h"


// �G�l�~�[
class Enemy : public Character
{
public:
	Enemy() {}
	Enemy(std::shared_ptr<SkinnedMesh>& mesh) :
		Character(mesh) {};
	~Enemy() override {}

	// �X�V����
	virtual void Update(float elapsedTime) = 0;

	// �`�揈��
	virtual void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe) = 0;

	// �j��
	void Destroy();

	// �f�o�b�O�v���~�e�B�u�`��
	virtual void DrawDebugPrimitive();

	// �f�o�b�O�G�l�~�[���\��
	virtual void DrawDebugGUI();

	// �U���t���O�擾
	bool	GetAttackFlg() { return attackFlg; }
	// �U���t���O�Z�b�g
	void	SetAttackFlg(bool flg) { attackFlg = flg; };
	void	SetId(int id) { this->id = id; }
	int		GetId() { return id; }

	// ���b�Z�[�W��M�����Ƃ��̏���
	/*virtual bool OnMessage(const Telegram& msg);

	virtual bool HandleMessage(const Telegram& msg);*/

	// �U���͈͎擾
	float GetAttackRange() { return attackRange; }

	// �f�o�b�O�����_���擾
	//DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }
	
protected:
	int	state = 0;
	bool attackFlg = false;
	int id = 0;
	float searchRange = 0.0f;
	float attackRange = 0.0f;

private:
	//std::unique_ptr<DebugRenderer>					debugRenderer;
};