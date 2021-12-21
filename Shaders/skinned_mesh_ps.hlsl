// UNIT.16
#include "skinned_mesh.hlsli"

// UNIT.17
Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	//return pin.color;

	// UNIT.17
	return diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * pin.color;
}

