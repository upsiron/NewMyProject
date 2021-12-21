#pragma once

#include <vector>
#include "Enemy.h"

// �G�l�~�[�}�l�[�W���[
class EnemyManager
{
private:
	EnemyManager() {}
	~EnemyManager() {}

public:

	// �B��̃C���X�^���X�擾
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe);

	// �G�l�~�[�o�^
	void Register(Enemy* enemy);

	// ������
	void Clear();

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	// // �f�o�b�O���\��
	void DrawDebugGUI();

	// �G�l�~�[���擾
	int GetEnemyCount()const { return static_cast<int>(enemies.size()); }

	// �G�l�~�[�擾
	Enemy* GetEnemy(int index) { return enemies.at(index); }

	// �G�l�~�[�폜
	void Remove(Enemy* enemy);

	// �t�^����ID����G�l�~�[���擾
	Enemy* GetEnemyFromId(int id);

private:
	std::vector<Enemy*> enemies;
	std::vector<Enemy*> removes;

	// �G�l�~�[���m�̏Փˏ���
	void CollisionEnemyVsEnemies();
public:
	int identity = 0;	// �t�^����ID�̒l(���̒l��MetaAI::Identity::Enemy�����Z���ĕt�^����)
}; 