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
    //	初期化
    DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;

    //	拡大・縮小
    S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

    //	回転
    Rx = DirectX::XMMatrixRotationX(angle.x);				//	X軸を基準とした回転行列
    Ry = DirectX::XMMatrixRotationY(angle.y);				//	Y軸を基準とした回転行列
    Rz = DirectX::XMMatrixRotationZ(angle.z);				//	Z軸を基準とした回転行列
    R = Rx * Ry * Rz;

    //	平行移動
    T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

    DirectX::XMStoreFloat4x4(&this->world, S*R*T); 
}

void StaticObject::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& lightDirection, const DirectX::XMFLOAT4& materialColor, bool wireframe)
{
    DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&this->world);
    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
    DirectX::XMMATRIX WVP = W * V * P;
    DirectX::XMFLOAT4X4 world_view_projection;//ワールド・ビュー・プロジェクション合成行列 
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

    // オブジェクト空間でのレイに変換  
    DirectX::XMVECTOR worldStart = DirectX::XMLoadFloat3(&startPosition); 
    DirectX::XMVECTOR worldEnd = DirectX::XMLoadFloat3(&endPosition);
    DirectX::XMVECTOR localStart = DirectX::XMVector3TransformCoord(worldStart, inverseTransform); 
    DirectX::XMVECTOR localEnd = DirectX::XMVector3TransformNormal(worldEnd, inverseTransform); 

    // レイピック  
    float outDistance;
    DirectX::XMFLOAT3 start, end;
    DirectX::XMStoreFloat3(&start, localStart);  
    DirectX::XMStoreFloat3(&end, localEnd); 
    int ret = mesh->RayPick(start, end, outPosition, outNormal, &outDistance); 
    if (ret != -1)  {
        // オブジェクト空間からワールド空間へ変換   
        DirectX::XMVECTOR localPosition = DirectX::XMLoadFloat3(outPosition); 
        DirectX::XMVECTOR localNormal = DirectX::XMLoadFloat3(outNormal);   
        DirectX::XMVECTOR worldPosition = DirectX::XMVector3TransformCoord(localPosition, worldTransform);  
        DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(localNormal, worldTransform); 

    DirectX::XMStoreFloat3(outPosition, worldPosition);   DirectX::XMStoreFloat3(outNormal, worldNormal);
    }

    return ret;
}
