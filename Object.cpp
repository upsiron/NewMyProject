#include "Object.h"

Object::Object()
{
	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();

	obstacleMesh = std::make_shared<SkinnedMesh>(device, "Data/Stage/block2.fbx", true);
	coinMesh = std::make_shared<SkinnedMesh>(device, "Data/Stage/block.fbx", true);
}
