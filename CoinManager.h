#pragma once
#include <vector>
#include "Coin.h"
#include "Object.h"

// �R�C���}�l�[�W���[
class CoinManager
{
private:
	CoinManager() {}
	~CoinManager() {}

public:

	// �B��̃C���X�^���X�擾
	static CoinManager& Instance()
	{
		static CoinManager instance;
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

	// �R�C���o�^
	void Register(Object* coin);

	// ������
	void Clear();

	// �f�o�b�O���\��
	void DrawDebugGUI();

	// �R�C�����擾
	int GetCoinCount()const { return static_cast<int>(coins.size()); }

	// �R�C���擾
	Object* GetCoin(int index) { return coins.at(index); }

	// �R�C���폜
	void Remove(Object* coin);

	// �t�^����ID����R�C�����擾
	Object* GetCoinFromId(int id);

private:
	std::vector<Object*> coins;
	std::vector<Object*> removes;
};