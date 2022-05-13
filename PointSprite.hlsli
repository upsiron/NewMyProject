// **************************************************
//
//				�p�[�e�B�N���̕`�揈��
//
// **************************************************
//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);


//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------
struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Size : TEXCOORD;
    float4 Color : COLOR;
    float4 Param : PARAM; //�ėp�p�����[�^
};

struct GSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Size : TEXCOORD;
    float4 Color : COLOR;
    float4 Param : PARAM; //�ėp�p�����[�^
};

struct PSInput
{
    float4 Position : SV_POSITION; // ���_���W(�ˉe���W�n)
    float2 Tex : TEXCOORD; // �e�N�X�`�����W
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
//	�O���[�o���ϐ�
//--------------------------------------------

cbuffer CBPerFrame : register(b0)
{
    row_major float4x4 View; // �r���[�ϊ��s��
    row_major float4x4 Projection; // �����ϊ��s��
};


cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_projection;
    float4 light_direction;
    float4 camera_position;
};

