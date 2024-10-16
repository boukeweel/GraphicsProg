#include "Scene_W4.h"
#include "Utils.h"
#include "Material.h"
#include "Renderer.h"

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

	auto triangle = Triangle{ {-.75f,.5f,0.f},{-.75f,2.f,0.f},{.75f,.5f,0.f} };
	triangle.cullMode = TriangleCullMode::NoCulling;
	triangle.materialIndex = matLambert_White;

	m_Triangles.emplace_back(triangle);

	//lights
	AddPointLight(Vector3{ 0.f,5.f,5.f }, 50.f, ColorRGB{ 1.f,.61f,.45f }); //BackLight
	AddPointLight(Vector3{ -2.5f,5.f,-5.f }, 70.f, ColorRGB{ 1.f,.8f,.45f }); //Front light
	AddPointLight(Vector3{ 2.5f,2.5f,-5.f }, 50.f, ColorRGB{ .34f,.47f,.68f }); //fill Light
}
