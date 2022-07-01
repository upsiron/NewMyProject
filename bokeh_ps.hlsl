#include "bokeh.hlsli"


static const float waight[] =
{
    { 1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0 },
    { 2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0 },
    { 1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0 },
};


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
   
    //テクスチャサイズ取得
    uint width, height;
    texture_map.GetDimensions(width, height);
	//テクセルサイズ取得
    const float2 texcel = float2(1.0f / (float) width, 1.0f / (float) height);

    float3 work = (float3) 0;
    uint n = 0;
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            work += texture_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord + float2(texcel.x * x, texcel.y * y)).rgb * waight[n];
            n++;
        }
    }
    color.rgb = work;
    
    return color;
}