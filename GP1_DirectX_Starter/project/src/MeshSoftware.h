#pragma once
#include "Mesh.h"

namespace dae {
	class Camera;

	struct Vertex_Out
	{
		Vector4 position{};
		ColorRGB color{ colors::White };
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
		Vector3 viewDirection{};
	};

	struct BoundaryBox
	{
		Vector2 Smallest;
		Vector2 Biggest;
	};

	class MeshSoftware : public Mesh
	{
	public:
		MeshSoftware(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Material* pMaterial, SDL_Surface* pBackBuffer, uint32_t* pBackBufferPixels, float* pDepthBufferPixels, int width, int height);
		MeshSoftware(const std::string& objName, Material* pMaterial, SDL_Surface* pBackBuffer, uint32_t* pBackBufferPixels, float* pDepthBufferPixels, int width, int height);

		~MeshSoftware() override;

		void Render(const Camera* pCamera) const;

	private:
		void InitializeMesh();

		void VertexTransformationFunction(const Camera* pCamera) const;

		void TriangleStrip() const;
		void TriangleList() const;

		void Rasterize(const size_t v0, const size_t v1, const size_t v2) const;

		void PixelShading(const int pixelIndex, const Vertex_Out& vertex_out) const;

		void ResetVertices() const;
		float Remap(float value, float fromMin, float fromMax, float toMin = 0.0f, float toMax = 1.0f) const;

		std::vector<Vertex_Out*> m_pVertexOut{};
		primitiveTechnology m_primitiveTopology{ primitiveTechnology::TriangleList };

		Vector3 m_LightDirection{ .577f,-.577f,.577f };
		float m_Shininess{ 25.f };
		float m_PhongSpecular{ 0.5f };
		float m_DiffuseReflectance{ 7.f };

		//toggels
		enum class LightingMode
		{
			ObservedArea,	//Lambert cosine law
			Diffuse,		//Incident radiance
			Specular,		//Scattering of the light
			Combined		//ObservedArea*Radiance*BRDF
		};

		LightingMode m_CurrentLightingMode{ LightingMode::Combined };
		bool m_ShouldRotated{ true };
		bool m_NormalMapActive{ true };
		bool m_DepthToggle{ false };

		//does not own them just uses them
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{ nullptr };
		float* m_pDepthBufferPixels{ nullptr };

		int m_Width;
		int m_Height;
	};
}