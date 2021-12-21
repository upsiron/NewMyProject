#include "Phong.hlsli"

//-------------------------------------------- 
// 拡散反射関数(ランバート) 
//-------------------------------------------- 
// N:法線(正規化済み) 
// L:入射ベクトル(正規化済み) 
// C:入射光(色・強さ) 
// K:反射率 

float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
    float D = dot(N, -L);
    D = max(0, D); // 負の値を０にする  
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
    
    //WVP 変換  
    output.Position = mul(P, matWVP);
    //ワールド変換  
    float3 wPos = mul(P, World).xyz;
    output.wPosition = wPos;
    float3 wN = mul(N,World).xyz;
    wN = normalize(wN);
    output.wNormal = wN;

    float3 L = normalize(LightDir.xyz);
    float3 K = float3(1, 1, 1);
    ////拡散反射（関数使用）  
    //float3 LC = Diffuse(wN, L, LightColor.rgb, K);
    output.Color = materialColor;
    //テクスチャー座標  
    output.Tex = input.Tex;

    return output;
}