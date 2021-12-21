#pragma once

#include <DirectXMath.h>

class Camera
{
private:
	Camera(){}
	~Camera(){}

public:
	// �B��̃C���X�^���X�擾
	static Camera& Instance()
	{
		static Camera camera;
		return camera;
	}

	// �w�����������
	void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

	// �p�[�X�y�N�e�B�u�ݒ�
	void SetPerspecticeFov(float fovY, float aspect, float nearZ, float farZ);

	// �r���[�s��擾
	const DirectX::XMFLOAT4X4& GetView() const { return view; }

	//�v���W�F�N�V�����s��
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

	// ���_�擾
	const DirectX::XMFLOAT3& GetEye()const { return eye; }

	// �����_�擾
	const DirectX::XMFLOAT3& GetFocus()const { return focus; }

	// ������擾
	const DirectX::XMFLOAT3& GetUp()const { return up; }

	// �O�����擾
	const DirectX::XMFLOAT3& GetFront()const { return front; }

	// �E�����擾
	const DirectX::XMFLOAT3& GetRight()const { return right; }

private:
	DirectX::XMFLOAT4X4	view = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4X4	projection = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };;

	DirectX::XMFLOAT3	eye = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3	focus = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3	up = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3	front = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3	right = { 0.0f,0.0f,0.0f };
};