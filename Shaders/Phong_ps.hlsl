#include "Phong.hlsli"


//-------------------------------------------- 
// ���ʔ��ˊ֐�(�t�H��) 
//-------------------------------------------- 
// N:�@��(���K���ς�) 
// L:���˃x�N�g��(���K���ς�) 
// C:���ˌ�(�F�E����) 
// E:���_�����x�N�g��(���K���ς�) 
// K:���˗�(0�`1.0) 
// Power:�n�C���C�g�̋���(�P���x) 
float3 PhongSpecular(float3 N, float3 L, float3 C, float3 E, float3 K, float Power)
{ //�n�[�t�x�N�g��  
    float3 R = normalize(reflect(L, N));
    float3 S = dot(R, E);
    S = max(0, S);
    S = pow(S, Power);
    S = S * K * C;
    return S;
}

float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
    float D = dot(N, -L);
    D = max(0, D); // ���̒l���O�ɂ���  
    return C * D * K;
}

float4 main(PSInput input) : SV_Target
{
    float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex);
    float3 N = normalize(input.wNormal);
    float3 E = normalize(EyePos.xyz - input.wPosition);
    float3 L = normalize(LightDir.xyz);
    
    // ����  
    float3 A = AmbientColor.rgb;

   //�g�U����  
    float3 C = LightColor.rgb * 2;
    float3 Kd = float3(1, 1, 1);
    float3 D = Diffuse(N, L, C, Kd);

   //���ʔ���  
    float3 Ks = float3(1.5f, 1.5f, 1.5f);
    float3 S = PhongSpecular(N, L, C, E, Ks, 20);
    color *= input.Color * float4(A + D + S, 1.0);
    return color;
}

