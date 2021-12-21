#include "Phong.hlsli"

//-------------------------------------------- 
// �g�U���ˊ֐�(�����o�[�g) 
//-------------------------------------------- 
// N:�@��(���K���ς�) 
// L:���˃x�N�g��(���K���ς�) 
// C:���ˌ�(�F�E����) 
// K:���˗� 

float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
    float D = dot(N, -L);
    D = max(0, D); // ���̒l���O�ɂ���  
    return C * D * K;
}

PSInput main(VSInput input)
{
    PSInput output = (PSInput) 0;
    float3 p = { 0.0f, 0.0f, 0.0f};
    float3 n = { 0.0f, 0.0f, 0.0f};
    //float4 P = { 0, 0, 0 ,0};
    //float4 N = { 0, 0, 0 ,0};
    for (int i = 0; i < 4; i++)
    {
        p += (input.BoneWeights[i] * mul(input.Position, BoneTransforms[input.BoneIndices[i]])).xyz;
        n += (input.BoneWeights[i] * mul(float4(input.Normal.xyz, 0), BoneTransforms[input.BoneIndices[i]])).xyz;
    }
    
    float4 P = float4(p, 1.0);
    float4 N = float4(n, 0.0f);
    
    //WVP �ϊ�  
    output.Position = mul(P, matWVP);
    //���[���h�ϊ�  
    float3 wPos = mul(P, World).xyz;
    output.wPosition = wPos;
    float3 wN = mul(N,World).xyz;
    wN = normalize(wN);
    output.wNormal = wN;

    float3 L = normalize(LightDir.xyz);
    float3 K = float3(1, 1, 1);
    ////�g�U���ˁi�֐��g�p�j  
    //float3 LC = Diffuse(wN, L, LightColor.rgb, K);
    output.Color = materialColor;
    //�e�N�X�`���[���W  
    output.Tex = input.Tex;

    return output;
}