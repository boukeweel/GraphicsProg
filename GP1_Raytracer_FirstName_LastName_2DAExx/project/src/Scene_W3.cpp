#include "Scene_W3.h"
#include "Utils.h"
#include "Material.h"
#include "Renderer.h"

void dae::Scene_W3::Initialize()
{
	m_Camera.origin = { 0.f,3.f,-9.f };
	m_Camera.fovAngle = 45.f;

	//materials
	const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, 1.f));
	const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, .6f));
	const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, .1f));
	const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 1.f, 1.f));
	const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 1.f, .6f));
	const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 1.f, .1f));

	const auto matLamber_GrayBlue = AddMaterial(new Material_Lambert({ .49,0.57f,0.57f }, 1.f));

	//planes
	AddPlane(Vector3{ 0.f,0.f,10.f }, Vector3{ 0.f,0.f,-1.f }, matLamber_GrayBlue); //back
	AddPlane(Vector3{ 0.f,0.f,0.f }, Vector3{ 0.f,1.f,0.f }, matLamber_GrayBlue);   //bottom
	AddPlane(Vector3{ 0.f,10.f,0.f }, Vector3{ 0.f,-1.f,0.f }, matLamber_GrayBlue); //top
	AddPlane(Vector3{ 5.f,0.f,0.f }, Vector3{ -1.f,0.f,0.f }, matLamber_GrayBlue);  //right
	AddPlane(Vector3{ -5.f,0.f,0.f }, Vector3{ 1.f,0.f,0.f }, matLamber_GrayBlue);  // left

	/*const auto matLambertPhong1 = AddMaterial(new Material_LambertPhong(colors::Blue, 0.5f, 0.5f, 3.f));
	const auto matLambertPhong2 = AddMaterial(new Material_LambertPhong(colors::Blue, 0.5f, 0.5f, 15.f));
	const auto matLambertPhong3 = AddMaterial(new Material_LambertPhong(colors::Blue, 0.5f, 0.5f, 50.f));

	AddSphere(Vector3{ -1.75f,1.f,0.f }, .75, matLambertPhong1);
	AddSphere(Vector3{ 0.f,1.f,0.f }, .75, matLambertPhong2);
	AddSphere(Vector3{ 1.75f,1.f,0.f }, .75, matLambertPhong3);*/

	//spheres
	AddSphere(Vector3{ -1.75f,1.f,0.f }, .75, matCT_GrayRoughMetal);
	AddSphere(Vector3{ 0.f,1.f,0.f }, .75, matCT_GrayMediumMetal);
	AddSphere(Vector3{ 1.75f,1.f,0.f }, .75, matCT_GraySmoothMetal);
	AddSphere(Vector3{ -1.75f,3.f,0.f }, .75, matCT_GrayRoughPlastic);
	AddSphere(Vector3{ 0.f,3.f,0.f }, .75, matCT_GrayMediumPlastic);
	AddSphere(Vector3{ 1.75f,3.f,0.f }, .75, matCT_GraySmoothPlastic);

	//lights
	AddPointLight(Vector3{ 0.f,5.f,5.f }, 50.f, ColorRGB{ 1.f,.61f,.45f }); //BackLight
	AddPointLight(Vector3{ -2.5f,5.f,-5.f }, 70.f, ColorRGB{ 1.f,.8f,.45f }); //Front light
	AddPointLight(Vector3{ 2.5f,2.5f,-5.f }, 50.f, ColorRGB{ .34f,.47f,.68f }); //fill Light
}
