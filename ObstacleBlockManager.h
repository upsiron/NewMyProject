#pragma once

#include <vector>
#include "Obstacle.h"

// �G�l�~�[�}�l�[�W���[
class ObstacleBlockManager
{
private:
	ObstacleBlockManager() {}
	~ObstacleBlockManager() {}

public:

	// �B��̃C���X�^���X�擾
	static ObstacleBlockManager& Instance()
	{
		static ObstacleBlockManager instance;
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
	void Register(Obstacle* obstacle);

	// ������
	void Clear();

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	// // �f�o�b�O���\��
	void DrawDebugGUI();

	// �G�l�~�[���擾
	int GetObstacleCount()const { return static_cast<int>(obstacles.size()); }

	// �G�l�~�[�擾
	Obstacle* GetObstacle(int index) { return obstacles.at(index); }

	// �G�l�~�[�폜
	void Remove(Obstacle* obstacle);

	// �t�^����ID����G�l�~�[���擾
	Obstacle* GetObstacleFromId(int id);

private:
	std::vector<Obstacle*> obstacles;
	std::vector<Obstacle*> removes;

	// �G�l�~�[���m�̏Փˏ���
	//void CollisionEnemyVsEnemies();
public:
	int identity = 0;	// �t�^����ID�̒l(���̒l��MetaAI::Identity::Enemy�����Z���ĕt�^����)
};