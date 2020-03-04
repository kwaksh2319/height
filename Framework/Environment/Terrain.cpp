#include "Framework.h"
#include "Terrain.h"


Terrain::Terrain(Shader * shader, wstring heightFile)
	: shader(shader)
{
	heightMap = new Texture(heightFile);

	CreateVertexData();
	CreateIndexData();
	CreateNormalData();
	CreateBuffer();
	
}

Terrain::~Terrain()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);
	SafeDeleteArray(verticesHeight);
	SafeRelease(vertexBuffer);
	
	SafeRelease(indexBuffer);

	SafeDelete(heightMap);
}

void Terrain::Update()
{
	Matrix world;
	D3DXMatrixIdentity(&world);
	Color color = (0, 1, 1, 1);
	shader->AsVector("Color")->SetFloatVector(color);
	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());
}

void Terrain::Render()
{
	UINT stride = sizeof(TerrainVertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static bool bWireframe = false;
	ImGui::Checkbox("TerrainWireframe", &bWireframe);
	ImGui::SliderFloat("vertexy", &vertices[0].Position.y,-10000000,100000000);
	ImGui::SliderFloat("vertexYN", &vertices[0].Normal.y, -10000000, 100000000);
	shader->DrawIndexed(0, bWireframe == true ? 1 : 0, indexCount);
}

void Terrain::CreateVertexData()
{
	vector<Color> heights;
	heightMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	width = heightMap->GetWidth();
	height = heightMap->GetHeight();

	
	vertexCount = width  * height;
	vertices = new TerrainVertex[vertexCount];
	verticesHeight = new TerrainVertex[vertexCount];
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{

			UINT index = width * z + x;
			UINT pixel = width * (height - 1 - z) + x;
		   
			vertices[index].Position.x = (float)x;
			vertices[index].Position.y = heights[pixel].r * 255 / 10.0f;
			vertices[index].Position.z = (float)z;
		   
			

			
		}
	
	}
	for (int i = 0; i < vertexCount*2; i++) {
		//verticesHeight[i] = vertices[i/2];
	}
	for (int i = 0; i < vertexCount * 2; i++) {
		 // vertices[i]= verticesHeight[i];
	}
	
	
}

void Terrain::CreateIndexData()
{
	indexCount = (width - 1) * (height - 1) * 6;
	indices = new UINT[indexCount];
	indicesHeight = new UINT[indexCount];
	UINT index = 0;
	
	for (UINT y = 0; y < height - 1; y++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			indices[index + 0] = width * y + x;
			indices[index + 1] = width * (y + 1) + x;
			indices[index + 2] = width * y + x + 1;
			indices[index + 3] = width * y + x + 1;
			indices[index + 4] = width * (y + 1) + x;
			indices[index + 5] = width * (y + 1) + x + 1;


			index += 6;
		}
	}

	
}

void Terrain::CreateNormalData()
{
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		TerrainVertex v0 = vertices[index0];
		TerrainVertex v1 = vertices[index1];
		TerrainVertex v2 = vertices[index2];


		Vector3 e1 = v1.Position - v0.Position;
		Vector3 e2 = v2.Position - v0.Position;

		Vector3 normal;
		D3DXVec3Cross(&normal, &e1, &e2);

		vertices[index0].Normal += normal;
		vertices[index1].Normal += normal;
		vertices[index2].Normal += normal;
	}

	for (UINT i = 0; i < vertexCount; i++)
		D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);

	for (UINT i = 0; i < vertexCount; i++)
	{
		tmpSaveNorm.push_back(vertices[i].Normal);
		//vertices[vertexCount-i].Position.y = vertices[i].Position.y + vertices[i].Normal.y*10.0f;
	}


}

void Terrain::CreateBuffer()
{



	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(TerrainVertex) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;
		
		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}

	
	
	//Create IndexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = indices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &indexBuffer));
	}

	
}


vector<Vector3> Terrain::NormData()
{
	return tmpSaveNorm;
}
