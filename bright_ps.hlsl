#include "bright.hlsli"


//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
//float4 main(PSInput input) : SV_TARGET0
//{
//    float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex) * input.Color;

//    float3 tgray = color.r * 0.299
//		+ color.g * 0.587
//		+ color.b * 0.114;

//    if (dot(tgray, tgray) > threshold)
//        return color;

//    color = float4(0, 0, 0, 0);
//    return color;
//}


#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4
SamplerState sampler_states[5] : register(s0);
Texture2D texture_map : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture_map.Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
	
    //float3 mask = { 0.6, 0.6, 0.6 };
    //color.rgb = (color.rgb * mask);
    
    float3 tgray = color.r * 0.299
		+ color.g * 0.587
		+ color.b * 0.114;

    if (dot(tgray, tgray) > threshold)
        return color;

    color = float4(0, 0, 0, 0);
    //color.g *= 1.2f;
    return color;
}
