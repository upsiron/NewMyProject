#include "PointSprite.hlsli"

//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------
float4 main(PSInput input) : SV_Target
{
    float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex) * input.Color;
    return color;
}

