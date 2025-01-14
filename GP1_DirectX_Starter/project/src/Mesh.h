#pragma once
#include "Effect.h"
#include "pch.h"

namespace dae {

	struct Vertex
	{
		Vector3 Position{};
		ColorRGB Color{ colors::White };
		Vector2 uv{};
		//Vector3 normal{};
		//Vector3 tangent{};
		//Vector3 viewDirection{};
	};

	struct Material
	{
		Texture* pDiffuse = nullptr;
		//Texture* pOpacity = nullptr;
		//Texture* pNormal = nullptr;
		//Texture* pSpecular = nullptr;
		//Texture* pGloss = nullptr;
	};

	enum class primitiveTechnology
	{
		TriangleList,
		TriangleStrip
	};

	class Mesh
	{
	public:
		Mesh(ID3D11Device* pDevice,Effect* pEffect, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,Material* pMaterial);
		~Mesh();

		void Render(ID3D11DeviceContext* pDeviceContext, const Matrix& viewProjectionMatrix) const;

	private:
		void InitializeMesh(ID3D11Device* pDevice);

		Effect* m_pEffects{nullptr};

		ID3D11Buffer* m_pVertexBuffer{ nullptr };
		ID3D11Buffer* m_pIndexBuffer{ nullptr };
		ID3D11InputLayout* m_pInputLayout{ nullptr };

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

		float m_YawRotation;
		Vector3 m_Scale;
		Vector3 m_Position;
	};
}
