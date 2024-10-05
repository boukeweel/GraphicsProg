#include "Scene_W2.h"
#include "Utils.h"
#include "Material.h"

void dae::Scene_W2::Initialize()
{
	m_Camera.origin = { 0.f,3.f,-9 };
	m_Camera.fovAngle = 45.f;

	//default: Material id0 >> SolidColor Material (RED)
	constexpr unsigned char matId_Solid_Red = 0;
	const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

	const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
	const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
	const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

	//Plane
	AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
	AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
	AddPlane({ 0.f, 0, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
	AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
	AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);

	//Spheres
	AddSphere({ -1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
	AddSphere({ 0.f, 1.f, 0.f }, .75f, matId_Solid_Blue);
	AddSphere({ 1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
	AddSphere({ -1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);
	AddSphere({ 0.f, 3.f, 0.f }, .75f, matId_Solid_Red);
	AddSphere({ 1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);

	//light
	AddPointLight({ 0.f,5.f,-5.f }, 70.f, colors::White);
}
