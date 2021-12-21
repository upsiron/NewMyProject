#include "shader.h"
#include "texture.h"
#include "misc.h"
#include "StaticMesh.h"
#include "camera.h"

#include <fbxsdk.h>
using namespace fbxsdk;

#include <vector>
#include <functional>

void fbxamatrix_to_xmfloat4x4(const FbxAMatrix &fbxamatrix, DirectX::XMFLOAT4X4 &xmfloat4x4)
{
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
}

int StaticMesh::RayPick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal, float* outLength)
{
	/*引数
	startPosition : レイを飛ばす開始座標
	endPosition   : レイを飛ばす終了座標
	outPosition   : レイが当たった座標
	outNormal     : レイが当たった面の法線
	outLength     : レイが当たった面までの距離  戻り値 : マテリアル番号*/

	int ret = -1;  
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&startPosition);  
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec); 
	float neart; 
	DirectX::XMStoreFloat(&neart, length);

	DirectX::XMVECTOR position, normal; 

	for (const auto& it : faces) {
		// 面頂点取得   
		DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&it.position[0]);
		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&it.position[1]);
		DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&it.position[2]);
		//  3 辺算出 
		DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b, a);
		DirectX::XMVECTOR bc = DirectX::XMVectorSubtract(c, b);
		DirectX::XMVECTOR ca = DirectX::XMVectorSubtract(a, c);
		// 外積による法線算出 
		DirectX::XMVECTOR n = DirectX::XMVector3Cross(ab, bc);
		// 内積の結果がプラスならば裏向き
		DirectX::XMVECTOR dot = DirectX::XMVector3Dot(dir, n);
		float fdot;
		DirectX::XMStoreFloat(&fdot, dot);
		if (fdot >= 0) continue; 

		// 交点算出  
		//cp  あたった座標
		//cp = start(レイの開始座標) + dir(レイの向き) * t(スカラー)    tを求める
		//レイの開始位置から頂点aへのベクトルと法線を使って直角三角形を作る
		//そこから射影(d)を作り、d+法線でd'を作る
		//  [　　　|-----------------------|start
		//　|　 　d|射影                 / ↓     
		//  |　　　|-------------------/---↓dir ]
		//　|　　　↑|                /     |     |
		//　|　　　↑|              /       |     |
		//　|　　　↑|            /         |     |
		//d'{ 法線 ↑|         /            |      } t(比率)
		//　|　　　↑|       /              |     |
		//　|　　　↑|     /                |     |
		//　|　　　↑|  /                   |     |
		//　[　 　　|/_____________________|____]______c
		//     　　a　\                    |           |
	 	//       　　　　\                 |           |
		//          　　　　\              |           |
		//             　　　　\           cp          |
		//                　　　　\                    |
		//                   　　　　\                 | 
		//                      　　　　\              |
		//                         　　　　\           |
		//                            　　　　\        |
		//                               　　　　\     |
		//                                  　　　　\  |
		//                                    　　　　\|b
		//
		DirectX::XMVECTOR v0 = DirectX::XMVectorSubtract(a, start);
		DirectX::XMVECTOR t = DirectX::XMVectorDivide(DirectX::XMVector3Dot(n, v0), dot); 
		float ft;
		DirectX::XMStoreFloat(&ft, t);
		if (ft < 0.0f || ft > neart) continue; 

		DirectX::XMVECTOR cp = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(dir, t), start);

		//  内点判定  
		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(a, cp);
		DirectX::XMVECTOR temp1 = DirectX::XMVector3Cross(v1, ab);
		DirectX::XMVECTOR work1 = DirectX::XMVector3Dot(temp1, n); 
		float fwork1;
		DirectX::XMStoreFloat(&fwork1, work1); 
		if (fwork1 < 0.0f) continue;

		DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(b, cp);
		DirectX::XMVECTOR temp2 = DirectX::XMVector3Cross(v2, bc);
		DirectX::XMVECTOR work2 = DirectX::XMVector3Dot(temp2, n);
		float fwork2;
		DirectX::XMStoreFloat(&fwork2, work2);
		if (fwork2 < 0.0f) continue;

		DirectX::XMVECTOR v3 = DirectX::XMVectorSubtract(c, cp);
		DirectX::XMVECTOR temp3 = DirectX::XMVector3Cross(v3, ca);
		DirectX::XMVECTOR work3 = DirectX::XMVector3Dot(temp3, n);
		float fwork3;
		DirectX::XMStoreFloat(&fwork3, work3);
		if (fwork3 < 0.0f) continue;

		// 情報保存 
		position = cp; 
		normal = n;
		neart = ft; 
		ret = it.materialIndex;
	} 
	if (ret != -1)  { 
		DirectX::XMStoreFloat3(outPosition, position);
		DirectX::XMStoreFloat3(outNormal, normal);
	} 
	//最も近いヒット位置までの距離  
	*outLength = neart;
	return ret;
}


StaticMesh::StaticMesh(ID3D11Device *device, const char *fbxFilename)
{
	// Create the FBX SDK manager
	FbxManager* manager = FbxManager::Create();

	// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h.
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

	// Create an importer.
	FbxImporter* importer = FbxImporter::Create(manager, "");

	// Initialize the importer.
	bool importStatus = false;
	importStatus = importer->Initialize(fbxFilename, -1, manager->GetIOSettings());
	_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

	// Create a new scene so it can be populated by the imported file.
	FbxScene* scene = FbxScene::Create(manager, "");

	// Import the contents of the file into the scene.
	importStatus = importer->Import(scene);
	_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

	// Convert mesh, NURBS and patch into triangle mesh
	fbxsdk::FbxGeometryConverter geometryConverter(manager);
	geometryConverter.Triangulate(scene, /*replace*/true);

	// Fetch node attributes and materials under this node recursively. Currently only mesh.	
	std::vector<FbxNode*> fetchedMeshes;
	std::function<void(FbxNode*)> traverse = [&](FbxNode* node)
	{
		if (node)
		{
			FbxNodeAttribute *fbxNodeAttribute = node->GetNodeAttribute();
			if (fbxNodeAttribute)
			{
				switch (fbxNodeAttribute->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
					fetchedMeshes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
			{
				traverse(node->GetChild(i));
			}
		}
	};
	traverse(scene->GetRootNode());

	meshes.resize(fetchedMeshes.size());

	//FbxMesh *fbx_mesh = fetched_meshes.at(0)->GetMesh();  // Currently only one mesh.
	for (size_t i = 0; i < fetchedMeshes.size(); i++)
	{
		FbxMesh *fbxMesh = fetchedMeshes.at(i)->GetMesh();
		Mesh &mesh = meshes.at(i);

		FbxAMatrix globalTransform = fbxMesh->GetNode()->EvaluateGlobalTransform(0);
		fbxamatrix_to_xmfloat4x4(globalTransform, mesh.globalTransform);

		// Fetch material properties.
		const int numberOfMaterials = fbxMesh->GetNode()->GetMaterialCount();

		//subsets.resize(number_of_materials);
		//subsets.resize(number_of_materials > 0 ? number_of_materials : 1);

		mesh.subsets.resize(numberOfMaterials > 0 ? numberOfMaterials : 1);
		for (int indexOfMaterial = 0; indexOfMaterial < numberOfMaterials; ++indexOfMaterial)
		{
			//subset &subset = subsets.at(index_of_material);
			Subset &subset = mesh.subsets.at(indexOfMaterial);

			const FbxSurfaceMaterial *surfaceMaterial = fbxMesh->GetNode()->GetMaterial(indexOfMaterial);

			const FbxProperty property = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
			const FbxProperty factor = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
			if (property.IsValid() && factor.IsValid())
			{
				FbxDouble3 color = property.Get<FbxDouble3>();
				double f = factor.Get<FbxDouble>();
				subset.diffuse.color.x = static_cast<float>(color[0] * f);
				subset.diffuse.color.y = static_cast<float>(color[1] * f);
				subset.diffuse.color.z = static_cast<float>(color[2] * f);
				subset.diffuse.color.w = 1.0f;
			}
			if (property.IsValid())
			{
				const int numberOfTextures = property.GetSrcObjectCount<FbxFileTexture>();
				if (numberOfTextures)
				{
					const FbxFileTexture* fileTexture = property.GetSrcObject<FbxFileTexture>();
					if (fileTexture)
					{
						const char *filename = fileTexture->GetRelativeFileName();
						// Create "diffuse.shader_resource_view" from "filename".
						//wchar_t texture_unicode[256];
						//MultiByteToWideChar(CP_ACP, 0, filename, strlen(filename) + 1, texture_unicode, 1024);
						//D3D11_TEXTURE2D_DESC texture2d_desc;
						//load_texture_from_file(device, texture_unicode, subset.diffuse.shader_resource_view.GetAddressOf(), &texture2d_desc);

						wchar_t fbxUnicode[256];
						MultiByteToWideChar(CP_ACP, 0, fbxFilename, static_cast<int>(strlen(fbxFilename) + 1), fbxUnicode, 1024);
						wchar_t textureUnicode[256];
						MultiByteToWideChar(CP_ACP, 0, fileTexture->GetFileName(), static_cast<int>(strlen(fileTexture->GetFileName()) + 1), textureUnicode, 1024);
						combine_resource_path(textureUnicode, fbxUnicode, textureUnicode);

						//material.texture_filename = texture_unicode;
						D3D11_TEXTURE2D_DESC texture2dDesc;
						load_texture_from_file(device, textureUnicode, subset.diffuse.shaderResourceView.GetAddressOf(), &texture2dDesc);
					}
				}
			}
		}
		for (Subset &subset : mesh.subsets)
		{
			if (!subset.diffuse.shaderResourceView)
			{
				make_dummy_texture(device, subset.diffuse.shaderResourceView.GetAddressOf());
			}
		}

		// Count the polygon count of each material
		if (numberOfMaterials > 0)
		{
			// Count the faces of each material
			const int numberOfPolygons = fbxMesh->GetPolygonCount();
			for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygons; ++indexOfPolygon)
			{
				const u_int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);

				//subsets.at(material_index).index_count += 3;
				mesh.subsets.at(materialIndex).indexCount += 3;

			}

			// Record the offset (how many vertex)
			int offset = 0;

			//for (subset &subset : subsets)
			for (Subset &subset : mesh.subsets)
			{
				subset.indexStart = offset;
				offset += subset.indexCount;
				// This will be used as counter in the following procedures, reset to zero
				subset.indexCount = 0;
			}
		}

		// Fetch mesh data
		std::vector<Vertex> vertices;
		std::vector<u_int> indices;
		u_int vertexCount = 0;

		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);

		const FbxVector4 *arrayOfControlPoints = fbxMesh->GetControlPoints();
		const int numberOfPolygons = fbxMesh->GetPolygonCount();
		indices.resize(numberOfPolygons * 3);

		//追加
		DirectX::XMMATRIX meshTransforms = DirectX::XMLoadFloat4x4(&mesh.globalTransform);

		for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygons; indexOfPolygon++)
		{
			// The material for current face.
			int indexOfMaterial = 0;
			if (numberOfMaterials > 0)
			{
				indexOfMaterial = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);
			}

			// Where should I save the vertex attribute index, according to the material
			//subset &subset = subsets.at(index_of_material);
			Subset &subset = mesh.subsets.at(indexOfMaterial);
			const int indexOffset = subset.indexStart + subset.indexCount;

			//追加
			Face face;
			face.materialIndex = indexOfMaterial; // material 番号 

			for (int indexOfVertex = 0; indexOfVertex < 3; indexOfVertex++)
			{
				Vertex vertex;
				const int indexOfControlPoint = fbxMesh->GetPolygonVertex(indexOfPolygon, indexOfVertex);
				vertex.position.x = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][0]);
				vertex.position.y = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][1]);
				vertex.position.z = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][2]);

				// 面ごとの頂点データ 
				//face.position[indexOfVertex] = vertex.position; 
				DirectX::XMVECTOR localPosition = DirectX::XMLoadFloat3(&vertex.position);
				DirectX::XMVECTOR gloabalPosition = DirectX::XMVector3TransformCoord(localPosition, meshTransforms);
				DirectX::XMStoreFloat3(&face.position[indexOfVertex], gloabalPosition);

				if (fbxMesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(indexOfPolygon, indexOfVertex, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}

				if (fbxMesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmapped_uv;
					fbxMesh->GetPolygonVertexUV(indexOfPolygon, indexOfVertex, uvNames[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}

				vertices.push_back(vertex);

				indices.at(indexOffset + indexOfVertex) = static_cast<u_int>(vertexCount);
				vertexCount += 1;
			}
			subset.indexCount += 3;
			// 面データ保存  
			faces.push_back(face);
		}
		//create_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
		mesh.createBuffers(device, vertices.data(), static_cast<int>(vertices.size()), indices.data(), static_cast<int>(indices.size()));
	}
	manager->Destroy();

	HRESULT hr = S_OK;

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	create_vs_from_cso(device, "Shaders/static_mesh_vs.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	create_ps_from_cso(device, "Shaders/static_mesh_ps.cso", pixelShader.GetAddressOf());

	//// create rasterizer state : solid mode
	//{
	//	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	//	rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	//	rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
	//	rasterizerDesc.FrontCounterClockwise = FALSE;
	//	rasterizerDesc.DepthBias = 0;
	//	rasterizerDesc.DepthBiasClamp = 0;
	//	rasterizerDesc.SlopeScaledDepthBias = 0;
	//	rasterizerDesc.DepthClipEnable = TRUE;
	//	rasterizerDesc.ScissorEnable = FALSE;
	//	rasterizerDesc.MultisampleEnable = FALSE;
	//	rasterizerDesc.AntialiasedLineEnable = FALSE;
	//	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[0].GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}
	//// create rasterizer state : wireframe mode
	//{
	//	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	//	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	//	rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
	//	rasterizerDesc.FrontCounterClockwise = FALSE;
	//	rasterizerDesc.DepthBias = 0;
	//	rasterizerDesc.DepthBiasClamp = 0;
	//	rasterizerDesc.SlopeScaledDepthBias = 0;
	//	rasterizerDesc.DepthClipEnable = TRUE;
	//	rasterizerDesc.ScissorEnable = FALSE;
	//	rasterizerDesc.MultisampleEnable = FALSE;
	//	rasterizerDesc.AntialiasedLineEnable = FALSE;
	//	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[1].GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}
	//// create depth stencil state
	//{
	//	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	//	depthStencilDesc.DepthEnable = TRUE;
	//	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	//	depthStencilDesc.StencilEnable = FALSE;
	//	depthStencilDesc.StencilReadMask = 0xFF;
	//	depthStencilDesc.StencilWriteMask = 0xFF;
	//	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	//	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}
	// create constant buffer
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(ConstantBuffer);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//// create sampler state
	//D3D11_SAMPLER_DESC samplerDesc = {};
	//samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.MipLODBias = 0;
	//samplerDesc.MaxAnisotropy = 16;
	//samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//memcpy(samplerDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	//samplerDesc.MinLOD = 0;
	//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	//hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

StaticMesh::StaticMesh(ID3D11Device* device, const char* fbxFilename, const char* shader_filename)
{
	// Create the FBX SDK manager
	FbxManager* manager = FbxManager::Create();

	// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h.
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

	// Create an importer.
	FbxImporter* importer = FbxImporter::Create(manager, "");

	// Initialize the importer.
	bool importStatus = false;
	importStatus = importer->Initialize(fbxFilename, -1, manager->GetIOSettings());
	_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

	// Create a new scene so it can be populated by the imported file.
	FbxScene* scene = FbxScene::Create(manager, "");

	// Import the contents of the file into the scene.
	importStatus = importer->Import(scene);
	_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

	// Convert mesh, NURBS and patch into triangle mesh
	fbxsdk::FbxGeometryConverter geometryConverter(manager);
	geometryConverter.Triangulate(scene, /*replace*/true);

	// Fetch node attributes and materials under this node recursively. Currently only mesh.	
	std::vector<FbxNode*> fetchedMeshes;
	std::function<void(FbxNode*)> traverse = [&](FbxNode* node)
	{
		if (node)
		{
			FbxNodeAttribute* fbxNodeAttribute = node->GetNodeAttribute();
			if (fbxNodeAttribute)
			{
				switch (fbxNodeAttribute->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
					fetchedMeshes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
			{
				traverse(node->GetChild(i));
			}
		}
	};
	traverse(scene->GetRootNode());

	meshes.resize(fetchedMeshes.size());

	//FbxMesh *fbx_mesh = fetched_meshes.at(0)->GetMesh();  // Currently only one mesh.
	for (size_t i = 0; i < fetchedMeshes.size(); i++)
	{
		FbxMesh* fbxMesh = fetchedMeshes.at(i)->GetMesh();
		Mesh& mesh = meshes.at(i);

		FbxAMatrix globalTransform = fbxMesh->GetNode()->EvaluateGlobalTransform(0);
		fbxamatrix_to_xmfloat4x4(globalTransform, mesh.globalTransform);

		// Fetch material properties.
		const int numberOfMaterials = fbxMesh->GetNode()->GetMaterialCount();

		//subsets.resize(number_of_materials);
		//subsets.resize(number_of_materials > 0 ? number_of_materials : 1);

		mesh.subsets.resize(numberOfMaterials > 0 ? numberOfMaterials : 1);
		for (int indexOfMaterial = 0; indexOfMaterial < numberOfMaterials; ++indexOfMaterial)
		{
			//subset &subset = subsets.at(index_of_material);
			Subset& subset = mesh.subsets.at(indexOfMaterial);

			const FbxSurfaceMaterial* surfaceMaterial = fbxMesh->GetNode()->GetMaterial(indexOfMaterial);

			const FbxProperty property = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
			const FbxProperty factor = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
			if (property.IsValid() && factor.IsValid())
			{
				FbxDouble3 color = property.Get<FbxDouble3>();
				double f = factor.Get<FbxDouble>();
				subset.diffuse.color.x = static_cast<float>(color[0] * f);
				subset.diffuse.color.y = static_cast<float>(color[1] * f);
				subset.diffuse.color.z = static_cast<float>(color[2] * f);
				subset.diffuse.color.w = 1.0f;
			}
			if (property.IsValid())
			{
				const int numberOfTextures = property.GetSrcObjectCount<FbxFileTexture>();
				if (numberOfTextures)
				{
					const FbxFileTexture* fileTexture = property.GetSrcObject<FbxFileTexture>();
					if (fileTexture)
					{
						const char* filename = fileTexture->GetRelativeFileName();
						// Create "diffuse.shader_resource_view" from "filename".
						//wchar_t texture_unicode[256];
						//MultiByteToWideChar(CP_ACP, 0, filename, strlen(filename) + 1, texture_unicode, 1024);
						//D3D11_TEXTURE2D_DESC texture2d_desc;
						//load_texture_from_file(device, texture_unicode, subset.diffuse.shader_resource_view.GetAddressOf(), &texture2d_desc);

						wchar_t fbxUnicode[256];
						MultiByteToWideChar(CP_ACP, 0, fbxFilename, static_cast<int>(strlen(fbxFilename) + 1), fbxUnicode, 1024);
						wchar_t textureUnicode[256];
						MultiByteToWideChar(CP_ACP, 0, fileTexture->GetFileName(), static_cast<int>(strlen(fileTexture->GetFileName()) + 1), textureUnicode, 1024);
						combine_resource_path(textureUnicode, fbxUnicode, textureUnicode);

						//material.texture_filename = texture_unicode;
						D3D11_TEXTURE2D_DESC texture2dDesc;
						load_texture_from_file(device, textureUnicode, subset.diffuse.shaderResourceView.GetAddressOf(), &texture2dDesc);
					}
				}
			}
		}
		for (Subset& subset : mesh.subsets)
		{
			if (!subset.diffuse.shaderResourceView)
			{
				make_dummy_texture(device, subset.diffuse.shaderResourceView.GetAddressOf());
			}
		}

		// Count the polygon count of each material
		if (numberOfMaterials > 0)
		{
			// Count the faces of each material
			const int numberOfPolygons = fbxMesh->GetPolygonCount();
			for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygons; ++indexOfPolygon)
			{
				const u_int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);

				//subsets.at(material_index).index_count += 3;
				mesh.subsets.at(materialIndex).indexCount += 3;

			}

			// Record the offset (how many vertex)
			int offset = 0;

			//for (subset &subset : subsets)
			for (Subset& subset : mesh.subsets)
			{
				subset.indexStart = offset;
				offset += subset.indexCount;
				// This will be used as counter in the following procedures, reset to zero
				subset.indexCount = 0;
			}
		}

		// Fetch mesh data
		std::vector<Vertex> vertices;
		std::vector<u_int> indices;
		u_int vertexCount = 0;

		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);

		const FbxVector4* arrayOfControlPoints = fbxMesh->GetControlPoints();
		const int numberOfPolygons = fbxMesh->GetPolygonCount();
		indices.resize(numberOfPolygons * 3);

		for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygons; indexOfPolygon++)
		{
			// The material for current face.
			int indexOfMaterial = 0;
			if (numberOfMaterials > 0)
			{
				indexOfMaterial = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);
			}

			// Where should I save the vertex attribute index, according to the material
			//subset &subset = subsets.at(index_of_material);
			Subset& subset = mesh.subsets.at(indexOfMaterial);
			const int indexOffset = subset.indexStart + subset.indexCount;

			for (int indexOfVertex = 0; indexOfVertex < 3; indexOfVertex++)
			{
				Vertex vertex;
				const int indexOfControlPoint = fbxMesh->GetPolygonVertex(indexOfPolygon, indexOfVertex);
				vertex.position.x = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][0]);
				vertex.position.y = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][1]);
				vertex.position.z = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][2]);

				if (fbxMesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(indexOfPolygon, indexOfVertex, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}

				if (fbxMesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmapped_uv;
					fbxMesh->GetPolygonVertexUV(indexOfPolygon, indexOfVertex, uvNames[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}

				vertices.push_back(vertex);

				indices.at(indexOffset + indexOfVertex) = static_cast<u_int>(vertexCount);
				vertexCount += 1;
			}
			subset.indexCount += 3;
		}
		//create_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
		mesh.createBuffers(device, vertices.data(), static_cast<int>(vertices.size()), indices.data(), static_cast<int>(indices.size()));
	}
	manager->Destroy();

	HRESULT hr = S_OK;

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	char cso_name[128] = {};
	strcpy_s(cso_name, shader_filename);
	strcat_s(cso_name, "_vs.cso");
	create_vs_from_cso(device, cso_name, vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));


	memset(cso_name, '\0', strlen(cso_name));
	strcpy_s(cso_name, shader_filename);
	strcat_s(cso_name, "_ps.cso");
	create_ps_from_cso(device, cso_name, pixelShader.GetAddressOf());

	//// create rasterizer state : solid mode
	//{
	//	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	//	rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	//	rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
	//	rasterizerDesc.FrontCounterClockwise = FALSE;
	//	rasterizerDesc.DepthBias = 0;
	//	rasterizerDesc.DepthBiasClamp = 0;
	//	rasterizerDesc.SlopeScaledDepthBias = 0;
	//	rasterizerDesc.DepthClipEnable = TRUE;
	//	rasterizerDesc.ScissorEnable = FALSE;
	//	rasterizerDesc.MultisampleEnable = FALSE;
	//	rasterizerDesc.AntialiasedLineEnable = FALSE;
	//	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[0].GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}
	//// create rasterizer state : wireframe mode
	//{
	//	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	//	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	//	rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
	//	rasterizerDesc.FrontCounterClockwise = FALSE;
	//	rasterizerDesc.DepthBias = 0;
	//	rasterizerDesc.DepthBiasClamp = 0;
	//	rasterizerDesc.SlopeScaledDepthBias = 0;
	//	rasterizerDesc.DepthClipEnable = TRUE;
	//	rasterizerDesc.ScissorEnable = FALSE;
	//	rasterizerDesc.MultisampleEnable = FALSE;
	//	rasterizerDesc.AntialiasedLineEnable = FALSE;
	//	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[1].GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}
	//// create depth stencil state
	//{
	//	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	//	depthStencilDesc.DepthEnable = TRUE;
	//	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	//	depthStencilDesc.StencilEnable = FALSE;
	//	depthStencilDesc.StencilReadMask = 0xFF;
	//	depthStencilDesc.StencilWriteMask = 0xFF;
	//	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	//	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}
	// create constant buffer
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(ConstantBuffer);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//// create sampler state
	//D3D11_SAMPLER_DESC samplerDesc = {};
	//samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.MipLODBias = 0;
	//samplerDesc.MaxAnisotropy = 16;
	//samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//memcpy(samplerDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	//samplerDesc.MinLOD = 0;
	//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	//hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void StaticMesh::Mesh::createBuffers(ID3D11Device *device, Vertex *vertices, int numVertices, u_int *indices, int numIndices)
{
	HRESULT hr;
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		bufferDesc.ByteWidth = sizeof(Vertex)*numVertices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.

		hr = device->CreateBuffer(&bufferDesc, &subresource_data, vertexBuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		bufferDesc.ByteWidth = sizeof(u_int)*numIndices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = indices;
		subresourceData.SysMemPitch = 0; //Not use for index buffers.
		subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
		hr = device->CreateBuffer(&bufferDesc, &subresourceData, indexBuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void StaticMesh::Render(ID3D11DeviceContext *immediateContext, const DirectX::XMFLOAT4X4 &worldViewProjection, const DirectX::XMFLOAT4X4 &worldInverseTranspose, const DirectX::XMFLOAT4 &lightDirection, const DirectX::XMFLOAT4 &materialColor, bool wireframe)
{
	for (Mesh &mesh : meshes)
	{
		u_int stride = sizeof(Vertex);
		u_int offset = 0;
		immediateContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		immediateContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		immediateContext->IASetInputLayout(inputLayout.Get());

		immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
		immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

		//immediateContext->OMSetDepthStencilState(depthStencilState.Get(), 1);
		/*if (wireframe)
		{
			immediateContext->RSSetState(rasterizerStates[1].Get());
		}
		else
		{
			immediateContext->RSSetState(rasterizerStates[0].Get());
		}*/


		ConstantBuffer data;
		DirectX::XMStoreFloat4x4(&data.worldViewProjection, DirectX::XMLoadFloat4x4(&mesh.globalTransform)* DirectX::XMLoadFloat4x4(&worldViewProjection));
		DirectX::XMStoreFloat4x4(&data.worldInverseTranspose, DirectX::XMLoadFloat4x4(&mesh.globalTransform) * DirectX::XMLoadFloat4x4(&worldInverseTranspose));
		data.lightDirection = lightDirection;

		for (Subset &subset : mesh.subsets)
		{
			data.materialColor.x = subset.diffuse.color.x * materialColor.x;
			data.materialColor.y = subset.diffuse.color.y * materialColor.y;
			data.materialColor.z = subset.diffuse.color.z * materialColor.z;
			data.materialColor.w = materialColor.w;
			immediateContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
			immediateContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
			immediateContext->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			immediateContext->PSSetShaderResources(0, 1, subset.diffuse.shaderResourceView.GetAddressOf());
			//immediateContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
			immediateContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);
		}
	}


}
