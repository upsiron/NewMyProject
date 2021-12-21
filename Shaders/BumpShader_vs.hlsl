#include "BumpShader.hlsli"

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
    float4 P = float4(input.Position, 1.0);
    //WVP �ϊ�  
    output.Position = mul(P, matWVP);
    //���[���h�ϊ�  
    float3 wPos = mul(P, World).xyz;
    output.wPosition = wPos;
    float3 wN = mul((float3x3) World, input.Normal).xyz;
    wN = normalize(wN);
    output.wNormal = wN;
    
    //�ڋ�ԂR���̍쐬 
    float3 VZ = wN; 
    float3 VY = {0,1,0.001};//�� 
    float3 VX; 
    VY = normalize(VY); 
    VX = normalize(cross(VZ, VY));//�O�� 
 
    output.VX = VX;
    output.VY = VY;
    output.VZ = VZ;
    

    float3 L = normalize(LightDir.xyz);
    float3 K = float3(1, 1, 1);
    //�g�U���ˁi�֐��g�p�j  
    //float3 LC = Diffuse(wN, L, LightColor.rgb, K);
    output.Color = materialColor;
    //�e�N�X�`���[���W  
    output.Tex = input.Tex;

    return output;
}