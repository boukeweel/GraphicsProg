#include "Scene_W4.h"
#include "Utils.h"
#include "Material.h"
#include "Renderer.h"

#pragma region Scene_week4
void dae::Scene_W4::Initialize()
{
	m_Camera.origin = { 0.f,1.f,-5.f };
	m_Camera.fovAngle = 45.f;

	const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f,0.57f,0.57f }, 1.f));
	const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

	//planes
	AddPlane(Vector3{ 0.f,0.f,10.f }, Vector3{ 0.f,0.f,-1.f }, matLambert_GrayBlue); //back
	AddPlane(Vector3{ 0.f,0.f,0.f }, Vector3{ 0.f,1.f,0.f }, matLambert_GrayBlue);   //bottom
	AddPlane(Vector3{ 0.f,10.f,0.f }, Vector3{ 0.f,-1.f,0.f }, matLambert_GrayBlue); //top
	AddPlane(Vector3{ 5.f,0.f,0.f }, Vector3{ -1.f,0.f,0.f }, matLambert_GrayBlue);  //right
	AddPlane(Vector3{ -5.f,0.f,0.f }, Vector3{ 1.f,0.f,0.f }, matLambert_GrayBlue);  // left

#pragma region cube
	pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
	Utils::ParseOBJ("resources/simple_cube.obj",
		pMesh->positions,
		pMesh->normals,
		pMesh->indices);

	pMesh->Scale({ 0.7f, 0.7f, 0.7f });
	pMesh->Translate({ 0.f,1.f,0.f });

	pMesh->UpdateTransforms();
#pragma endregion

#pragma region TriangleMesh
	//triangle_Mesh
	//pMesh = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
	//pMesh->positions = { {-.75f,-1.f,0.f},{-.75f,1.f,0.f},{.75f,1.f,1.f},{.75f,-1.f,0.f} };
	//pMesh->indices = {
	//	0,1,2,//triangle 1
	//	0,2,3 //triangle 2
	//};

	//pMesh->CalculateNormals();

	//pMesh->Translate({ 0.f, 1.5f, 0.f });
	//pMesh->RotateY(45.f);

	//pMesh->UpdateTransforms();
#pragma endregion

#pragma region Triangle
	////triangle
	/*auto triangle = Triangle{ {-.75f,.5f,0.f},{-.75f,2.f,0.f},{.75f,.5f,0.f} };
	triangle.cullMode = TriangleCullMode::NoCulling;
	triangle.materialIndex = matLambert_White;

	m_Triangles.emplace_back(triangle);*/
#pragma endregion

	//lights
	AddPointLight(Vector3{ 0.f,5.f,5.f }, 50.f, ColorRGB{ 1.f,.61f,.45f }); //BackLight
	AddPointLight(Vector3{ -2.5f,5.f,-5.f }, 70.f, ColorRGB{ 1.f,.8f,.45f }); //Front light
	AddPointLight(Vector3{ 2.5f,2.5f,-5.f }, 50.f, ColorRGB{ .34f,.47f,.68f }); //fill Light
}


void dae::Scene_W4::Update(dae::Timer* pTimer)
{
	Scene::Update(pTimer);

	pMesh->RotateY(PI_DIV_2 * pTimer->GetTotal());
	pMesh->UpdateTransforms();
}

#pragma endregion

#pragma region RefrenceScene
void dae::Scene_W4_ReferenceScene::Initialize()
{
	sceneName = "Reference Scene";
	m_Camera.origin = { 0.f,3.f,-9.f };
	m_Camera.fovAngle = 45.f;

	const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, 1.f));
	const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, .6f));
	const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, .1f));
	const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, 1.f));
	const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, .6f));
	const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, .1f));

	const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f,0.57f,0.57f }, 1.f));
	const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

	//planes
	AddPlane(Vector3{ 0.f,0.f,10.f }, Vector3{ 0.f,0.f,-1.f }, matLambert_GrayBlue); //back
	AddPlane(Vector3{ 0.f,0.f,0.f }, Vector3{ 0.f,1.f,0.f }, matLambert_GrayBlue);   //bottom
	AddPlane(Vector3{ 0.f,10.f,0.f }, Vector3{ 0.f,-1.f,0.f }, matLambert_GrayBlue); //top
	AddPlane(Vector3{ 5.f,0.f,0.f }, Vector3{ -1.f,0.f,0.f }, matLambert_GrayBlue);  //right
	AddPlane(Vector3{ -5.f,0.f,0.f }, Vector3{ 1.f,0.f,0.f }, matLambert_GrayBlue);  // left

	//spheres
	AddSphere(Vector3{ -1.75f,1.f,0.f }, .75, matCT_GrayRoughMetal);
	AddSphere(Vector3{ 0.f,1.f,0.f }, .75, matCT_GrayMediumMetal);
	AddSphere(Vector3{ 1.75f,1.f,0.f }, .75, matCT_GraySmoothMetal);
	AddSphere(Vector3{ -1.75f,3.f,0.f }, .75, matCT_GrayRoughPlastic);
	AddSphere(Vector3{ 0.f,3.f,0.f }, .75, matCT_GrayMediumPlastic);
	AddSphere(Vector3{ 1.75f,3.f,0.f }, .75, matCT_GraySmoothPlastic);

	//triangles
	Triangle baseTriangle{ {-.75f,1.5f,0.f},{.75f,0.f,0.f},{-.75f,0.f,0.f} };

	m_pMeshes[0] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
	m_pMeshes[0]->AppendTriangle(baseTriangle, true);
	m_pMeshes[0]->Translate({ -1.75f,4.5f,0.f });
	m_pMeshes[0]->UpdateAABB();
	m_pMeshes[0]->UpdateTransforms();

	m_pMeshes[1] = AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLambert_White);
	m_pMeshes[1]->AppendTriangle(baseTriangle, true);
	m_pMeshes[1]->Translate({ 0.f,4.5f,0.f });
	m_pMeshes[1]->UpdateAABB();
	m_pMeshes[1]->UpdateTransforms();

	m_pMeshes[2] = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
	m_pMeshes[2]->AppendTriangle(baseTriangle, true);
	m_pMeshes[2]->Translate({ 1.75f,4.5f,0.f });
	m_pMeshes[2]->UpdateAABB();
	m_pMeshes[2]->UpdateTransforms();

	//lights
	AddPointLight(Vector3{ 0.f,5.f,5.f }, 50.f, ColorRGB{ 1.f,.61f,.45f }); //BackLight
	AddPointLight(Vector3{ -2.5f,5.f,-5.f }, 70.f, ColorRGB{ 1.f,.8f,.45f }); //Front light
	AddPointLight(Vector3{ 2.5f,2.5f,-5.f }, 50.f, ColorRGB{ .34f,.47f,.68f }); //fill Light
}

void dae::Scene_W4_ReferenceScene::Update(dae::Timer* pTimer)
{
	Scene::Update(pTimer);

	const auto yawAngle{ (cos(pTimer->GetTotal()) + 1.f) / 2.f * PI_2 };
	for (auto mesh : m_pMeshes)
	{
		mesh->RotateY(yawAngle);
		mesh->UpdateTransforms();
	}
}
#pragma endregion

void dae::Scene_W4_Bunny::Initialize()
{
	m_Camera.origin = { 0.f,1.f,-5.f };
	m_Camera.fovAngle = 45.f;

	const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f,0.57f,0.57f }, 1.f));
	const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

	//planes
	AddPlane(Vector3{ 0.f,0.f,10.f }, Vector3{ 0.f,0.f,-1.f }, matLambert_GrayBlue); //back
	AddPlane(Vector3{ 0.f,0.f,0.f }, Vector3{ 0.f,1.f,0.f }, matLambert_GrayBlue);   //bottom
	AddPlane(Vector3{ 0.f,10.f,0.f }, Vector3{ 0.f,-1.f,0.f }, matLambert_GrayBlue); //top
	AddPlane(Vector3{ 5.f,0.f,0.f }, Vector3{ -1.f,0.f,0.f }, matLambert_GrayBlue);  //right
	AddPlane(Vector3{ -5.f,0.f,0.f }, Vector3{ 1.f,0.f,0.f }, matLambert_GrayBlue);  // left

	pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
	Utils::ParseOBJ("resources/lowpoly_bunny.obj",
		pMesh->positions,
		pMesh->normals,
		pMesh->indices);

	pMesh->Scale({ 1.5f, 1.5f, 1.5f });
	//pMesh->Translate({ 0.f,1.f,0.f });
	pMesh->RotateY(PI);

	pMesh->UpdateAABB();
	pMesh->UpdateTransforms();

	//lights
	AddPointLight(Vector3{ 0.f,5.f,5.f }, 50.f, ColorRGB{ 1.f,.61f,.45f }); //BackLight
	AddPointLight(Vector3{ -2.5f,5.f,-5.f }, 70.f, ColorRGB{ 1.f,.8f,.45f }); //Front light
	AddPointLight(Vector3{ 2.5f,2.5f,-5.f }, 50.f, ColorRGB{ .34f,.47f,.68f }); //fill Light
}
