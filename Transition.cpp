#include "transition.h"

void Transition::init(ID3D11Device* device, wchar_t* filename, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, DirectX::XMFLOAT2 texPos, DirectX::XMFLOAT2 texSize, DirectX::XMFLOAT4 color)
{
    img = std::make_unique<Sprite>(device, filename);
    this->pos = pos;
    this->size = size;
    this->texPos = texPos;
    this->texSize = texSize;
    this->color = color;
}

void Transition::fadeOut(float alpha)
{
    color.w += alpha;
    if (color.w > 1.0f)
    {
        color.w = 1.0f;
    }
}

void Transition::fadeIn(float alpha)
{
    color.w -= alpha;
    if (color.w < 0.0f)
    {
        color.w = 0.0f;
    }
}

void Transition::render(ID3D11DeviceContext* context)
{
    img->Render(context, pos.x, pos.y, size.x, size.y, texPos.x, texPos.y, texSize.x, texSize.y, 0, color.x, color.y, color.z, color.w);
}
