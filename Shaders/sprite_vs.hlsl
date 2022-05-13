//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//	return pos;
//}

// UNIT.02
#include "sprite.hlsli"

VS_OUT main(float4 pos : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD /*UNIT.04*/)
{
    VS_OUT vout;
    vout.pos = pos;
    vout.color = color;
	// UNIT.04
    vout.texcoord = texcoord;

    return vout;
}

//#include "sprite.hlsli"
////--------------------------------------------
////	頂点シェーダー
////--------------------------------------------

//PSInput main(VSInput input)
//{
//    PSInput output = (PSInput) 0;

//	// 出力値設定.
//    output.Position = float4(input.Position, 1);
//    output.Color = input.Color;
//    output.Tex = input.Tex;

//    return output;
//}

