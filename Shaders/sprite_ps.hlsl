//float4 main() : SV_TARGET
//{
//	return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}

//// UNIT.04
Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);

// UNIT.02
#include "sprite.hlsli"
float4 main(VS_OUT pin) : SV_TARGET
{
    //return float4(1, 0, 0, 1);
    //pin.color;

	// UNIT.04
    return diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * pin.color;
}

////#include "sprite.hlsli"


////--------------------------------------------
////	ピクセルシェーダー
////--------------------------------------------

////float4 main(PSInput input) : SV_TARGET0
////{
////    float4 color = (float4) 0;
////    color = DiffuseTexture.Sample(DecalSampler, input.Tex);
////    color *= input.Color;
////    return color;
////}

