#pragma once

class TerrainHeight
{
public:
	typedef VertexNormal TerrainVertex;

public:
	TerrainHeight(Shader* shader, wstring heightFile);
	~TerrainHeight();

	void Update();
	void Render();

	void Pass(UINT val) { pass = val; }
	void GetNorm(vector<Vector3> norm);

private:
	void CreateVertexData();
	void CreateIndexData();
	void CreateNormalData();
	void CreateBuffer();
	void CreateHeight();


	void ExVertexData();
	void ExBuffer();
	void ExUpdates();


private:
	class Terrain*terrain;
	UINT pass = 0;
	Shader* shader;
	vector<Vector3> norms;
	vector<Vector3>normaa;
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
};
