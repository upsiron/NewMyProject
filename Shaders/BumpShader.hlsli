//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------

Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);
Texture2D NormalTexture : register(t1);

//--------------------------------------------
//	�O���[�o���ϐ�
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
    row_major float4x4 matWVP;
    row_major float4x4 World;
    float4 materialColor;
    float4 LightDir; //���C�g�̕��� 
};

cbuffer CBPerFrame2 : register(b2)
{
    float4 LightColor; //���C�g�̐F 
    float4 AmbientColor; //���� 
    float4 EyePos; //�J�������W 
}; 

//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
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
    float3 wNormal : TEXCOORD1; //���[���h�@��  
    float3 wPosition : TEXCOORD2;//���[���h���W  
    float2 Tex : TEXCOORD3; 
 
    float3 VX : TEXCOORD4; //�ڋ�ԗp X ��  
    float3 VY  : TEXCOORD5; //�ڋ�ԗp Y ��  
    float3 VZ  : TEXCOORD6; //�ڋ�ԗp Z �� 
 }; 