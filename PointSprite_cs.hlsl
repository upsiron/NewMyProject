#include "PointSprite.hlsli"


//スレッドの数（１グループにスレッドはいくつか？）
#define TH_X 100
#define TH_Y 1
#define TH_Z 1

struct ParticleData
{
    float3 pos;
    float2 size;
    float2 sizeA;
    float3 posV;
    float3 posA;
    float3 color;
    float alpha;
    float timer;
    float animeTimer;
    float elapsedTime;
    int type;
};

struct VERTEX
{
    float3 Pos;
    float3 Normal;
    float2 Tex;
    float4 Color;
    float4 Param;
};

// 入力バッファ(構造化バッファ。読み込み専用)
StructuredBuffer<ParticleData> particle : register(t0);
// 出力バッファ(構造化バッファ。読み書き可能)
RWStructuredBuffer<VERTEX> BufOut : register(u0);
RWStructuredBuffer<ParticleData> CpuBufOut : register(u1);


[numthreads(TH_X, TH_Y, TH_Z)]
void main(const CSInput input)
{
    
    int i = input.dispatch.x;
    float resultVX, resultVY, resultVZ;
    float resultX, resultY, resultZ;
    float resultAX, resultAY, resultAZ;
    float resultCX, resultCY, resultCZ;
    float resultW, resultH;
    float resultAW, resultAH;
    float resultTimer;
    float resultAlpha;
    float resultElapsedTime;
    float resultType;
    
    resultType = particle[i].type;
    //if (resultType < 0)
    {
       resultVX = particle[i].posV.x + particle[i].posA.x * particle[i].elapsedTime;
       resultVY = particle[i].posV.y + particle[i].posA.y * particle[i].elapsedTime;
       resultVZ = particle[i].posV.z + particle[i].posA.z * particle[i].elapsedTime;

       resultX = particle[i].pos.x + resultVX * particle[i].elapsedTime;
       resultY = particle[i].pos.y + resultVY * particle[i].elapsedTime;
       resultZ = particle[i].pos.z + resultVZ * particle[i].elapsedTime;
        
       resultW = particle[i].size.x;
       resultH = particle[i].size.y;
        
       resultAW = particle[i].sizeA.x;
       resultAH = particle[i].sizeA.y;
        
       resultAX = particle[i].posA.x;
       resultAY = particle[i].posA.y;
       resultAZ = particle[i].posA.z;
        
       resultCX = particle[i].color.x;
       resultCY = particle[i].color.y;
       resultCZ = particle[i].color.z;
 
       resultTimer = particle[i].timer - particle[i].elapsedTime;
       //resultAlpha = particle[i].alpha;
       resultAlpha = particle[i].timer / 2.0f;
       
       resultElapsedTime = particle[i].elapsedTime;
       if (resultTimer <= 0)resultType = -1;
    }
    
    
    //出力バッファに計算結果を入れる
    BufOut[i].Pos.x = resultX;
    BufOut[i].Pos.y = resultY;
    BufOut[i].Pos.z = resultZ;
    BufOut[i].Tex.x = resultW;
    BufOut[i].Tex.y = resultH;
    BufOut[i].Color.x = (float) resultType;
    BufOut[i].Color.y = BufOut[i].Color.z = 1.0f;
    BufOut[i].Color.w = resultAlpha;
    BufOut[i].Param.x = resultCX;
    BufOut[i].Param.y = resultCY;
    BufOut[i].Param.z = resultCZ;
    BufOut[i].Param.a = resultCZ;
    
    CpuBufOut[i].type = resultType;
    CpuBufOut[i].pos.x = resultX;
    CpuBufOut[i].pos.y = resultY;
    CpuBufOut[i].pos.z = resultZ;
    CpuBufOut[i].posV.x = resultVX;
    CpuBufOut[i].posV.y = resultVY;
    CpuBufOut[i].posV.z = resultVZ;
    CpuBufOut[i].posA.x = resultAX;
    CpuBufOut[i].posA.y = resultAY;
    CpuBufOut[i].posA.z = resultAZ;
    CpuBufOut[i].color.x = resultCX;
    CpuBufOut[i].color.y = resultCY;
    CpuBufOut[i].color.z = resultCZ;
    CpuBufOut[i].size.x = resultW;
    CpuBufOut[i].size.y = resultH;
    CpuBufOut[i].sizeA.x = resultAW;
    CpuBufOut[i].sizeA.y = resultAH;
    CpuBufOut[i].timer = /*2.0f;*/ resultTimer;
    CpuBufOut[i].alpha = resultAlpha;
    CpuBufOut[i].elapsedTime = resultElapsedTime;

}