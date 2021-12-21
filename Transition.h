#pragma once
#include <directxmath.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include "sprite.h"

class Transition
{
private:
    std::unique_ptr<Sprite> img;
    DirectX::XMFLOAT2 pos;
    DirectX::XMFLOAT2 size;
    DirectX::XMFLOAT2 texPos;
    DirectX::XMFLOAT2 texSize;
    DirectX::XMFLOAT4 color;              // �`��F
public:
    void init(ID3D11Device* device, wchar_t* filename, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, DirectX::XMFLOAT2 texPos, DirectX::XMFLOAT2 texSize, DirectX::XMFLOAT4 color);			//������
    void fadeOut(float alpha);			//�X�V
    void fadeIn(float alpha);
    void render(ID3D11DeviceContext* context);		                //�`��(�w�i)
    float GetAlpha() { return color.w; };
};