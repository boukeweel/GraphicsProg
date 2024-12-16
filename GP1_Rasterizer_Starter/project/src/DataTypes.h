#pragma once
#include "Maths.h"
#include "Texture.h"
#include "vector"

namespace dae
{
	struct Vertex
	{
		Vector3 position{};
		ColorRGB color{colors::White};
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
		Vector3 viewDirection{};
	};

	struct Vertex_Out
	{
		Vector4 position{};
		ColorRGB color{ colors::White };
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
		Vector3 viewDirection{};
	};

	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};

	struct Material
	{
		Texture* pDiffuse{ nullptr };
		Texture* pNormal{nullptr};
		Texture* pSpecular{ nullptr };
		Texture* pGloss{ nullptr };

		ColorRGB DiffuseColor{ colors::White };
		float DiffuseReflectance{ 7.f };
	};

	struct Mesh
	{
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		PrimitiveTopology primitiveTopology{ PrimitiveTopology::TriangleStrip };
		Material material{};

		std::vector<Vertex_Out> vertices_out{};
		Matrix worldMatrix{};

		Matrix rotationTransform{};
		Matrix translationTransform{};
		Matrix scaleTransform{};

		void Translate(const Vector3& translation)
		{
			translationTransform = Matrix::CreateTranslation(translation);
		}

		void RotateY(float yaw)
		{
			rotationTransform = Matrix::CreateRotationY(yaw);
		}

		void Scale(const Vector3& scale)
		{
			scaleTransform = Matrix::CreateScale(scale);
		}

		void UpdateTransforms()
		{
			worldMatrix = scaleTransform * rotationTransform * translationTransform;
		}

		void ResetVertices()
		{
			vertices_out.clear();
			vertices_out.reserve(vertices.size());

			for (size_t i = 0; i < vertices.size(); ++i)
			{
				Vertex_Out vertex_out{
					{ vertices[i].position.x,vertices[i].position.y ,vertices[i].position.z ,1.f },
					vertices[i].color,
					vertices[i].uv,
					vertices[i].normal,
					vertices[i].tangent,
					vertices[i].viewDirection
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
