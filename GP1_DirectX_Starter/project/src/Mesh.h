#pragma once
#include "EffectBase.h"
#include "pch.h"

namespace dae {

	class Texture;

	struct Vertex
	{
		Vector3 Position{};
		ColorRGB Color{ colors::White };
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
	};

	struct Material
	{
		Texture* pDiffuse = nullptr;
		Texture* pNormal = nullptr;
		Texture* pSpecular = nullptr;
		Texture* pGloss = nullptr;
	};

	enum class primitiveTechnology
	{
		TriangleList,
		TriangleStrip
	};

	class Mesh
	{
	public:

		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,Material* pMaterial);
		Mesh(const std::string& objName, Material* pMaterial);

		virtual ~Mesh();

		void SetPosition(Vector3 translate);
		void SetScale(Vector3 scale);
		void SetYawRotation(float yaw);

		void AddYawRotation(float yawDelta);

	protected:
		void UpdateWorldMatrix();

		std::vector<Vertex> m_modelVertices{};
		std::vector<uint32_t> m_Indices{};
		UINT m_IndicesCount{ 0 };

		Material* m_pMaterial{};

		Matrix	m_WorldMatrix
		{
			{1.0f,0.0f,0.0f,0.0f},
			{0.0f,1.0f,0.0f,0.0f},
			{0.0f,0.0f,1.0f,0.0f},
			{0.0f,0.0f,0.0f,1.0f}
		};

		float m_YawRotation{0};
		Vector3 m_Scale{1.f,1.f,1.f};
		Vector3 m_Position{0.f,0.f,0.f};
	};
}
