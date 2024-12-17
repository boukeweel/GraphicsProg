#pragma once
#include "Effect.h"
#include "pch.h"

namespace dae {

	struct VS_INPUT
	{
		Vector3 Position{};
		ColorRGB Color{ colors::White };
		//Vector2 uv{};
		//Vector3 normal{};
		//Vector3 tangent{};
		//Vector3 viewDirection{};
	};

	struct VS_OUTPUT
	{
		Vector4 Position{};
		ColorRGB Color{ colors::White };
		//Vector2 uv{};
		//Vector3 normal{};
		//Vector3 tangent{};
		//Vector3 viewDirection{};
	};

	enum class primitiveTechnology
	{
		TriangleList,
		TriangleStrip
	};

	class Mesh
	{
	public:
		Mesh(ID3D11Device* Device,std::vector<VS_INPUT> vertices,std::vector<uint32_t> indices);
		~Mesh();

	private:
		ID3D11Device* m_pDevice;

		std::vector<VS_INPUT> m_Vertices{};
		std::vector<uint32_t> m_Indices{};
		primitiveTechnology m_primitiveTechnology{ primitiveTechnology::TriangleList };

		std::vector<VS_OUTPUT> m_vertices_out{};
	};
}
