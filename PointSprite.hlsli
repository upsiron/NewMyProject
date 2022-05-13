// **************************************************
//
//				パーティクルの描画処理
//
// **************************************************
//--------------------------------------------
//	テクスチャ
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);


//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Size : TEXCOORD;
    float4 Color : COLOR;
    float4 Param : PARAM; //汎用パラメータ
};

struct GSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Size : TEXCOORD;
    float4 Color : COLOR;
    float4 Param : PARAM; //汎用パラメータ
};

struct PSInput
{
    float4 Position : SV_POSITION; // 頂点座標(射影座標系)
    float2 Tex : TEXCOORD; // テクスチャ座標
    float4 Color : COLOR0;
};

struct CSInput
{
    uint3 groupThread : SV_GroupThreadID;
    uint3 group : SV_GroupID;
    uint groupIndex : SV_GroupIndex;
    uint3 dispatch : SV_DispatchThreadID;
};

//--------------------------------------------
//	グローバル変数
//--------------------------------------------

cbuffer CBPerFrame : register(b0)
{
    row_major float4x4 View; // ビュー変換行列
    row_major float4x4 Projection; // 透視変換行列
};


cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_projection;
    float4 light_direction;
    float4 camera_position;
};

