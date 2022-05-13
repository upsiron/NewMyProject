//--------------------------------------------
//	テクスチャ
//--------------------------------------------


Texture2D DiffuseTexture : register(t0);

SamplerState DecalSampler : register(s0);



//--------------------------------------------
//	グローバル変数
//--------------------------------------------
cbuffer CBPerBloom : register(b3)
{
    float threshold;
    float dummy1;
    float dummy2;
    float dummy3;

};


//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
    float4 Color : COLOR; //頂点カラー無し
};



struct PSInput
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD;
    float4 Color : COLOR;
};


struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};