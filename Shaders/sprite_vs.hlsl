//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//	return pos;
//}

// UNIT.02
#include "sprite.hlsli"

VS_OUT main(float4 pos : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD/*UNIT.04*/)
{
	VS_OUT vout;
	vout.pos = pos;
	vout.color = color;
	// UNIT.04
	vout.texcoord = texcoord;

	return vout;
}
