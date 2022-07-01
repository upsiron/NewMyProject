#include "FullScreenQuad.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
//#define LINEAR_BORDER_BLACK 3
//#define LINEAR_BORDER_WHITE 4
SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
	
    float3 mask = { 0.6, 0.6, 0.6 };
    color.rgb = (color.rgb * mask);
    
    return color;
}
