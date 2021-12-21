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
    //------------------------------------------  // 初期化・解放  //------------------------------------------ 
    StaticObject() {};
    StaticObject(std::shared_ptr<StaticMesh>& mesh);
    ~StaticObject();
    //------------------------------------------  // 更新・描画  //------------------------------------------  
    void Update();  //ワールド行列作成 
    void Render(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT4X4& view,
        const DirectX::XMFLOAT4X4& projection,
        const DirectX::XMFLOAT4& lightDirection,
        const DirectX::XMFLOAT4& materialColor,
        bool wireframe);
    //------------------------------------------  // 情報更新  //------------------------------------------ 
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

    //ステージベース分割用構造体
    struct Squea
    {
        DirectX::XMFLOAT3 SpritPosition[9] = { { 0.0f, 0.0f, 0.0f } };
    };
    Squea squea;
};



