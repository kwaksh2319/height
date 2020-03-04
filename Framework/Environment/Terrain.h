#pragma once

class Terrain
{
public:
	typedef VertexNormal TerrainVertex;

public:
	Terrain(Shader* shader, wstring heightFile);
	~Terrain();

	void Update();
	void Render();

	void Pass(UINT val) { pass = val; }
	vector<Vector3> NormData();
private:
	void CreateVertexData();
	void CreateIndexData();
	void CreateNormalData();
	void CreateBuffer();
	void CreateHeight();
	

private:
	UINT pass = 0;
	Shader* shader;

	
	UINT width, height;

	UINT vertexCount;
	TerrainVertex* vertices;
	TerrainVertex* verticesHeight;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* lineBuffer;

	UINT indexCount;
	UINT* indices;
	UINT* indicesHeight;
	ID3D11Buffer* indexBuffer;
	
	Texture* heightMap;
	vector<Vector3> tmpSaveNorm;

};
