#pragma once
#include "framework.h"
#include <memory> 
#include <directxmath.h>
#include "StaticMesh.h"

class StaticObject
{
protected:
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 scale;
    DirectX::XMFLOAT3 angle;
    DirectX::XMFLOAT4X4 world;
    std::shared_ptr<StaticMesh> mesh;
public:
    //------------------------------------------  // �������E���  //------------------------------------------ 
    StaticObject() {};
    StaticObject(std::shared_ptr<StaticMesh>& mesh);
    ~StaticObject();
    //------------------------------------------  // �X�V�E�`��  //------------------------------------------  
    void Update();  //���[���h�s��쐬 
    void Render(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT4X4& view,
        const DirectX::XMFLOAT4X4& projection,
        const DirectX::XMFLOAT4& lightDirection,
        const DirectX::XMFLOAT4& materialColor,
        bool wireframe);
    //------------------------------------------  // ���X�V  //------------------------------------------ 
    void SetPosition(const DirectX::XMFLOAT3& p);
    void SetScale(const DirectX::XMFLOAT3& s);
    void SetAngle(const DirectX::XMFLOAT3& a);
    void SetWorld(DirectX::XMFLOAT4X4 m);

    int StaticObject::RayPick(const DirectX::XMFLOAT3& startPosition,
        const DirectX::XMFLOAT3& endPosition,
        DirectX::XMFLOAT3* outPosition,
        DirectX::XMFLOAT3* outNormal);

    const DirectX::XMFLOAT3& GetPosition() const { return position; };
    const DirectX::XMFLOAT3& GetScale() const { return scale; };
    const DirectX::XMFLOAT3& GetAngle() const { return angle; };
    const DirectX::XMFLOAT4X4& GetWorld() const { return world; };

    //�X�e�[�W�x�[�X�����p�\����
    struct Squea
    {
        DirectX::XMFLOAT3 SpritPosition[9] = { { 0.0f, 0.0f, 0.0f } };
    };
    Squea squea;
};



