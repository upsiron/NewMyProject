#include "bright.hlsli"

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------

//PSInput main(VSInput input)
//{
//    PSInput output = (PSInput) 0;

//	// 出力値設定.
//    output.Position = float4(input.Position, 1);
//    output.Color = input.Color;
//    output.Tex = input.Tex;

//    return output;
//}

VS_OUT main(in uint vertexid : SV_VERTEXID)
{
    VS_OUT vout;
    const float2 position[4] = { { -1, +1 }, { +1, +1 }, { -1, -1 }, { +1, -1 } };
    const float2 texcoords[4] = { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } };
    vout.position = float4(position[vertexid], 0, 1);
    vout.texcoord = texcoords[vertexid];
    return vout;
}

