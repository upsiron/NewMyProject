#include "BumpShader.hlsli"

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
    float4 P = float4(input.Position, 1.0);
    //WVP 変換  
    output.Position = mul(P, matWVP);
    //ワールド変換  
    float3 wPos = mul(P, World).xyz;
    output.wPosition = wPos;
    float3 wN = mul((float3x3) World, input.Normal).xyz;
    wN = normalize(wN);
    output.wNormal = wN;
    
    //接空間３軸の作成 
    float3 VZ = wN; 
    float3 VY = {0,1,0.001};//仮 
    float3 VX; 
    VY = normalize(VY); 
    VX = normalize(cross(VZ, VY));//外積 
 
    output.VX = VX;
    output.VY = VY;
    output.VZ = VZ;
    

    float3 L = normalize(LightDir.xyz);
    float3 K = float3(1, 1, 1);
    //拡散反射（関数使用）  
    //float3 LC = Diffuse(wN, L, LightColor.rgb, K);
    output.Color = materialColor;
    //テクスチャー座標  
    output.Tex = input.Tex;

    return output;
}