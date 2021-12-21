#include "BumpShader.hlsli"


//-------------------------------------------- 
// 鏡面反射関数(フォン) 
//-------------------------------------------- 
// N:法線(正規化済み) 
// L:入射ベクトル(正規化済み) 
// C:入射光(色・強さ) 
// E:視点方向ベクトル(正規化済み) 
// K:反射率(0～1.0) 
// Power:ハイライトの強さ(輝き度) 
float3 PhongSpecular(float3 N, float3 L, float3 C, float3 E, float3 K, float Power)
{ //ハーフベクトル  
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
    D = max(0, D); // 負の値を０にする  
    return C * D * K;
}

float4 main(PSInput input) : SV_Target
{
    //float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex);
    float3 N = normalize(input.wNormal);
    //float3 E = normalize(EyePos.xyz - input.wPosition);
    //float3 L = normalize(LightDir.xyz); // 環境光  
    float3 A = AmbientColor.rgb;

    
    float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex); 
    //float3 N = normalize(input.wNormal); 
    float3 E = normalize(EyePos.xyz - input.wPosition); 
    float3 L = normalize(LightDir.xyz); 
    float3 VX = normalize(input.VX); 
    float3 VY = normalize(input.VY); 
    float3 VZ = normalize(input.VZ); 
    //接空間からワールド空間へ変換する行列 
    float3x3 mat = { VX,VY,VZ }; 
 
    N = NormalTexture.Sample(DecalSampler, input.Tex).rgb;
    N = N * 2.0 - 1.0;
    //ノーマルテクスチャ法線をワールドへ変換  
    N = normalize(mul(N, mat));
    
   //拡散反射  
    float3 C = LightColor.rgb;
    float3 Kd = float3(1, 1, 1);
    float3 D = Diffuse(N, L, C, Kd);

   //鏡面反射  
    float3 Ks = float3(0.5f, 0.5f, 0.5f);
    float3 S = PhongSpecular(N, L, C, E, Ks, 20);
    color *= input.Color * float4(A + D + S*5.0f, 1.0);
    
    //return float4(1, 0, 0, 1);
    //color.rgb = N;
    return color;
}
