//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------


Texture2D DiffuseTexture : register(t0);

SamplerState DecalSampler : register(s0);



//--------------------------------------------
//	�O���[�o���ϐ�
//--------------------------------------------
cbuffer CBPerBloom : register(b3)
{
    float threshold;
    float dummy1;
    float dummy2;
    float dummy3;

};


//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------
struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
    float4 Color : COLOR; //���_�J���[����
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