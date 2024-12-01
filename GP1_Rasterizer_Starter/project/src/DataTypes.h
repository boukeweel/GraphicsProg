#pragma once
#include "Maths.h"
#include "vector"

namespace dae
{
	struct Vertex
	{
		Vector3 position{};
		ColorRGB color{colors::White};
		Vector2 uv{}; //W2
		//Vector3 normal{}; //W4
		//Vector3 tangent{}; //W4
		//Vector3 viewDirection{}; //W4
	};

	struct Vertex_Out
	{
		Vector4 position{};
		ColorRGB color{ colors::White };
		Vector2 uv{};
		//Vector3 normal{};
		//Vector3 tangent{};
		//Vector3 viewDirection{};
	};

	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};

	struct Mesh
	{
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		PrimitiveTopology primitiveTopology{ PrimitiveTopology::TriangleStrip };

		std::vector<Vertex_Out> vertices_out{};
		Matrix worldMatrix{};

		void ResetVertices()
		{
			vertices_out.clear();
			vertices_out.reserve(vertices.size());

			for (size_t i = 0; i < vertices.size(); ++i)
			{
				Vertex_Out vertex_out{
					{ vertices[i].position.x,vertices[i].position.y ,vertices[i].position.z ,1.f },
					vertices[i].color,
					vertices[i].uv
				};
				vertices_out.emplace_back(vertex_out);
			}
		}
	};

	struct BoundaryBox
	{
		Vector2 Smallest;
		Vector2 Biggest;
	};
}
