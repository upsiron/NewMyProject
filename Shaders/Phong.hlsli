//--------------------------------------------
//	テクスチャ
//--------------------------------------------

Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
//	グローバル変数
//--------------------------------------------
#define MAX_BONES 128
cbuffer CBPerMesh : register(b0)
{
    row_major float4x4 matWVP;
    row_major float4x4 World;
    float4 materialColor;
    float4 LightDir; //ライトの方向 
    row_major float4x4 BoneTransforms[MAX_BONES];
};

cbuffer CBPerFrame2 : register(b2)
{
    float4 LightColor; //ライトの色 
    float4 AmbientColor; //環境光 
    float4 EyePos; //カメラ座標 
}; 

//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
struct VSInput
{
    float4 Position : POSITION;
    float4 Normal : NORMAL;
    float2 Tex : TEXCOORD;
    float4 BoneWeights : WEIGHTS;
    uint4  BoneIndices : BONES;
    //float4 Color : COLOR;
};



struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
    float3 wNormal : TEXCOORD1; //ワールド法線  
    float3 wPosition : TEXCOORD2; //ワールド座標  
    float2 Tex : TEXCOORD3;
};