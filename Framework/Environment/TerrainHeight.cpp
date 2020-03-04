#include "Framework.h"
#include "TerrainHeight.h"

TerrainHeight::TerrainHeight(Shader * shader, wstring heightFile)
	: shader(shader)
{
	heightMap = new Texture(heightFile);
	terrain = new Terrain(shader, L"Gray256_Test.png");
	//terrain = new Terrain(,);
	
	//ExVertexData();
	
	
	//CreateNormalData();
	
	//CreateIndexData();
	//CreateNormalData();
	//CreateBuffer();

	CreateVertexData();
	//CreateIndexData();
	CreateNormalData();
	//CreateBuffer();

	ExUpdates();
	ExBuffer();

}

TerrainHeight::~TerrainHeight()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);
	SafeDeleteArray(verticesHeight);
	SafeRelease(vertexBuffer);
	SafeRelease(lineBuffer);
	SafeRelease(indexBuffer);
	SafeDelete(terrain);
	SafeDelete(heightMap);
}

void TerrainHeight::Update()
{
	Matrix world;
	D3DXMatrixIdentity(&world);
	Color color=(0,1,0,1);
	shader->AsVector("Color")->SetFloatVector(color);
	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());

	
	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertices, sizeof(TerrainVertex), 0);
}

void TerrainHeight::Render()
{
	
	UINT stride = sizeof(TerrainVertex);
	UINT offset = 0;
	/*
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static bool bWireframe = false;
	ImGui::Checkbox("TerrainWireframe", &bWireframe);
	ImGui::SliderFloat("vertexy", &vertices[0].Position.y, -10000000, 100000000);
	ImGui::SliderFloat("vertexYN", &vertices[0].Normal.y, -10000000, 100000000);
	shader->DrawIndexed(0, bWireframe == true ? 1 : 0, indexCount);
	*/

	//UINT stride = sizeof(VertexColor);
	//UINT offset = 0;
	
	ImGui::SliderFloat("vertexYNorm", &normaa[0].y, -10000000, 100000000);
	ImGui::SliderFloat("height", &verticesHeight[0].Position.y, -10000000, 100000000);
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	shader->Draw(0, 0, vertexCount*2);
}

void TerrainHeight::GetNorm(vector<Vector3> norm)
{
	 norms = norm;

}

void TerrainHeight::CreateVertexData()
{
	vector<Color> heights;
	heightMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	width = heightMap->GetWidth();
	height = heightMap->GetHeight();


	vertexCount = width * height;
	vertices = new TerrainVertex[vertexCount * 2];
	verticesHeight = new TerrainVertex[vertexCount * 2];
	
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
	for (int i = 0; i < vertexCount * 2; i++) {
		verticesHeight[i] = vertices[i/2];
	}

	
	

}

void TerrainHeight::CreateIndexData()
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

void TerrainHeight::CreateNormalData()
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



	/*
	for (UINT i = 0; i < vertexCount; i++)
	{

		verticesHeight[i*2].Position.y = vertices[i].Position.y + vertices[i].Normal.y*10.0f;
	}*/
	//for (int i = 0; i < vertexCount * 2; i++) {
	//vertices[i] = verticesHeight[i];
	//}

}

void TerrainHeight::CreateBuffer()
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

void TerrainHeight::CreateHeight()
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



}

void TerrainHeight::ExVertexData()
{
	vertices = new TerrainVertex[10];
	vertices[0].Position.x = 0.0f;
	vertices[0].Position.y = 0.0f;
	vertices[0].Position.z = 0.0f;
	vertices[1].Position.x = 0.3f;
	vertices[1].Position.y = 0.3f; 
	vertices[1].Position.z = 0.3f;
}

void TerrainHeight::ExBuffer()
{
	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(TerrainVertex) * vertexCount * 2;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}
}

void TerrainHeight::ExUpdates()
{
	normaa = terrain->NormData();
	//vector<float>norm = terrain->NormData();

	for (int i = 0; i < vertexCount*2; i++) {
		if (i % 2 == 0) {
			verticesHeight[i].Position.x= verticesHeight[i].Position.x + normaa[i / 2].x;
			verticesHeight[i].Position.y = verticesHeight[i].Position.y + normaa[i/2].y ;
			verticesHeight[i].Position.z = verticesHeight[i].Position.z + normaa[i / 2].z;
			//verticesHeight[i].Position.y = verticesHeight[i].Position.y + 1.0f;
		}
		vertices[i] = verticesHeight[i];
	}
}
