// UNIT.02
struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
	// UNIT.04
    float2 texcoord : TEXCOORD;
};

////--------------------------------------------
////	�e�N�X�`��
////--------------------------------------------

//Texture2D DiffuseTexture : register(t0);
//SamplerState DecalSampler : register(s0);


////--------------------------------------------
////	�O���[�o���ϐ�
////--------------------------------------------


////--------------------------------------------
////	�f�[�^�[�t�H�[�}�b�g
////--------------------------------------------

//struct VSInput
//{
//    float3 Position : POSITION;
//    float3 Normal : NORMAL;
//    float2 Tex : TEXCOORD;
//    float4 Color : COLOR;
//};

//struct PSInput
//{
//    float4 Position : SV_POSITION;
//    float2 Tex : TEXCOORD;
//    float4 Color : COLOR;
//};
