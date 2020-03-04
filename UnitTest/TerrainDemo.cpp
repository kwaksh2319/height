#include "stdafx.h"
#include "TerrainDemo.h"


void TerrainDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(4, 0, 0);
	Context::Get()->GetCamera()->Position(29, 6, -19);
	((Freedom *)Context::Get()->GetCamera())->Speed(20, 2);

	shader = new Shader(L"11_Terrain.fx");

	terrain = new Terrain(shader, L"Gray256_Test.png");
	terrainH = new TerrainHeight(shader, L"Gray256_Test.png");
	//terrainH->GetNorm(terrain->NormData());
}

void TerrainDemo::Destroy()
{
	SafeDelete(shader);

	SafeDelete(terrain);
	SafeDelete(terrainH);
}

void TerrainDemo::Update()
{
	terrain->Update();
	//terrainH->GetNorm(terrain->NormData());
	terrainH->Update();
}

void TerrainDemo::Render()
{
	terrain->Render();
	
	terrainH->Render();
}
