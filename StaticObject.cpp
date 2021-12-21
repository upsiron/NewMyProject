#include "StaticObject.h"

StaticObject::StaticObject(std::shared_ptr<StaticMesh>& mesh)
{
    this->mesh = mesh;
}

StaticObject::~StaticObject()
{
    this->mesh = NULL;
}

void StaticObject::Update()
{
    //	������
    DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;

    //	�g��E�k��
    S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

    //	��]
    Rx = DirectX::XMMatrixRotationX(angle.x);				//	X������Ƃ�����]�s��
    Ry = DirectX::XMMatrixRotationY(angle.y);				//	Y������Ƃ�����]�s��
    Rz = DirectX::XMMatrixRotationZ(angle.z);				//	Z������Ƃ�����]�s��
    R = Rx * Ry * Rz;

    //	���s�ړ�
    T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

    DirectX::XMStoreFloat4x4(&this->world, S*R*T); 
}

void StaticObject::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& lightDirection, const DirectX::XMFLOAT4& materialColor, bool wireframe)
{
    DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&this->world);
    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
    DirectX::XMMATRIX WVP = W * V * P;
    DirectX::XMFLOAT4X4 world_view_projection;//���[���h�E�r���[�E�v���W�F�N�V���������s�� 
    DirectX::XMStoreFloat4x4(&world_view_projection, WVP);
    mesh->Render(immediate_context,world_view_projection,this->world,lightDirection, materialColor,wireframe);
}

void StaticObject::SetPosition(const DirectX::XMFLOAT3& p)
{
    position.x = p.x;
    position.y = p.y;
    position.z = p.z;
}

void StaticObject::SetScale(const DirectX::XMFLOAT3& s)
{
    scale.x = s.x;
    scale.y = s.y;
    scale.z = s.z;
}

void StaticObject::SetAngle(const DirectX::XMFLOAT3& a)
{
    angle.x = a.x;
    angle.y = a.y;
    angle.z = a.z;
}

void StaticObject::SetWorld(DirectX::XMFLOAT4X4 m)
{
    world = m;
}

int StaticObject::RayPick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal)
{
    DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&world);  
    DirectX::XMMATRIX inverseTransform = DirectX::XMMatrixInverse(nullptr, worldTransform);

    // �I�u�W�F�N�g��Ԃł̃��C�ɕϊ�  
    DirectX::XMVECTOR worldStart = DirectX::XMLoadFloat3(&startPosition); 
    DirectX::XMVECTOR worldEnd = DirectX::XMLoadFloat3(&endPosition);
    DirectX::XMVECTOR localStart = DirectX::XMVector3TransformCoord(worldStart, inverseTransform); 
    DirectX::XMVECTOR localEnd = DirectX::XMVector3TransformNormal(worldEnd, inverseTransform); 

    // ���C�s�b�N  
    float outDistance;
    DirectX::XMFLOAT3 start, end;
    DirectX::XMStoreFloat3(&start, localStart);  
    DirectX::XMStoreFloat3(&end, localEnd); 
    int ret = mesh->RayPick(start, end, outPosition, outNormal, &outDistance); 
    if (ret != -1)  {
        // �I�u�W�F�N�g��Ԃ��烏�[���h��Ԃ֕ϊ�   
        DirectX::XMVECTOR localPosition = DirectX::XMLoadFloat3(outPosition); 
        DirectX::XMVECTOR localNormal = DirectX::XMLoadFloat3(outNormal);   
        DirectX::XMVECTOR worldPosition = DirectX::XMVector3TransformCoord(localPosition, worldTransform);  
        DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(localNormal, worldTransform); 

    DirectX::XMStoreFloat3(outPosition, worldPosition);   DirectX::XMStoreFloat3(outNormal, worldNormal);
    }

    return ret;
}
