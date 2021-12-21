//--------------------------------------------
//	テクスチャ
//--------------------------------------------

Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);
Texture2D NormalTexture : register(t1);

//--------------------------------------------
//	グローバル変数
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
    row_major float4x4 matWVP;
    row_major float4x4 World;
    float4 materialColor;
    float4 LightDir; //ライトの方向 
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
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
    //float4 Color : COLOR;
};



struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
    float3 wNormal : TEXCOORD1; //ワールド法線  
    float3 wPosition : TEXCOORD2;//ワールド座標  
    float2 Tex : TEXCOORD3; 
 
    float3 VX : TEXCOORD4; //接空間用 X 軸  
    float3 VY  : TEXCOORD5; //接空間用 Y 軸  
    float3 VZ  : TEXCOORD6; //接空間用 Z 軸 
 }; 