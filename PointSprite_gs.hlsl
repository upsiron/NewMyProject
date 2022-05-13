#include "PointSprite.hlsli"

//--------------------------------------------
//	ジオメトリシェーダー
//--------------------------------------------

[maxvertexcount(4)]
void main(point GSInput In[1],
	inout TriangleStream<PSInput> ParticleStream)
{
	// 座標変換　(ワールド座標系　→　ビュー座標系)
    float4 pos = mul(float4(In[0].Position, 1.0), View);

	// 点を面にする
    float w = In[0].Size.x;
    float h = In[0].Size.y;
    float4 posLT = pos + float4(-w, h, 0.0, 0.0);
    float4 posLB = pos + float4(-w, -h, 0.0, 0.0);
    float4 posRT = pos + float4(w, h, 0.0, 0.0);
    float4 posRB = pos + float4(w, -h, 0.0, 0.0);

    //float4 posLT = pos + float4(-w, h, 0.0, 0.0);
    //float4 posLB = pos + float4(w, h, 0.0, 0.0);
    //float4 posRT = pos + float4(-w, -h, 0.0, 0.0);
    //float4 posRB = pos + float4(w, -h, 0.0, 0.0);


	// 左上の点の位置(射影座標系)・UV・色を計算して出力
    PSInput Out = (PSInput) 0;
    uint type = In[0].Color.r;
    float u = (type % 4) * 0.25f;
    float v = (type / 4) * 0.25f;
    //Out.Color = float4(1, 1, 1, In[0].Color.a);
    Out.Color = float4(In[0].Param.x, In[0].Param.y, In[0].Param.z, In[0].Color.a);
    Out.Position = mul(posLT, Projection);
    Out.Tex = float2(u, v);
    ParticleStream.Append(Out);

	// 右上の点の位置(射影座標系)・UV・色を計算して出力
    Out.Position = mul(posRT, Projection);
    Out.Tex = float2(u + 0.25, v);
    //Out.Color = float4(1, 1, 1, In[0].Color.a);
    Out.Color = float4(In[0].Param.x, In[0].Param.y, In[0].Param.z, In[0].Color.a);
    ParticleStream.Append(Out);

	// 左下の点の位置(射影座標系)・UV・色を計算して出力
    Out.Position = mul(posLB, Projection);
    Out.Tex = float2(u, v + 0.25);
    //Out.Color = float4(1, 1, 1, In[0].Color.a);
    Out.Color = float4(In[0].Param.x, In[0].Param.y, In[0].Param.z, In[0].Color.a);
    ParticleStream.Append(Out);

	// 右下の点の位置(射影座標系)・UV・色を計算して出力
    Out.Position = mul(posRB, Projection);
    Out.Tex = float2(u + 0.25, v + 0.25);
    //Out.Color = float4(1, 1, 1, In[0].Color.a);
    Out.Color = float4(In[0].Param.x, In[0].Param.y, In[0].Param.z, In[0].Color.a);
    ParticleStream.Append(Out);

    ParticleStream.RestartStrip();
}
