#pragma once

#include <vector>
#include "Obstacle.h"
#include "Object.h"

// ��Q���}�l�[�W���[
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

	// ��Q���o�^
	void Register(Object* obstacle);

	// ������
	void Clear();

	// // �f�o�b�O���\��
	void DrawDebugGUI();

	// ��Q�����擾
	int GetObstacleCount()const { return static_cast<int>(obstacles.size()); }

	// ��Q���擾
	Object* GetObstacle(int index) { return obstacles.at(index); }

	// ��Q���폜
	void Remove(Object* obstacle);

	// �t�^����ID�����Q�����擾
	Object* GetObstacleFromId(int id);

private:
	std::vector<Object*> obstacles;
	std::vector<Object*> removes;
};