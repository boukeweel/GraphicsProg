#include "MeshSoftware.h"

#include "Camera.h"
#include "Texture.h"

namespace dae
{
	MeshSoftware::MeshSoftware(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Material* pMaterial,
		SDL_Surface* pBackBuffer, uint32_t* pBackBufferPixels, float* pDepthBufferPixels, int width, int height):
		Mesh(vertices,indices,pMaterial), m_pBackBuffer{pBackBuffer},m_pBackBufferPixels{pBackBufferPixels},m_pDepthBufferPixels{pDepthBufferPixels}, m_Width{ width }, m_Height{ height }
	{
		InitializeMesh();
	}

	MeshSoftware::MeshSoftware(const std::string& objName, Material* pMaterial, 
		SDL_Surface* pBackBuffer, uint32_t* pBackBufferPixels, float* pDepthBufferPixels, int width, int height):
		Mesh(objName,pMaterial), m_pBackBuffer{ pBackBuffer }, m_pBackBufferPixels{ pBackBufferPixels }, m_pDepthBufferPixels{ pDepthBufferPixels },m_Width{width},m_Height{height}
	{
		InitializeMesh();
	}

	void MeshSoftware::InitializeMesh()
	{
		m_pVertexOut.reserve(m_modelVertices.size());

		for (size_t i = 0; i < m_modelVertices.size(); ++i)
		{
			m_pVertexOut.emplace_back(new Vertex_Out{});
		}
	}


	MeshSoftware::~MeshSoftware()
	{
		for (Vertex_Out* vertex : m_pVertexOut)
		{
			delete vertex;
			vertex = nullptr;
		}
	}

	void MeshSoftware::Render(const Camera* pCamera) const
	{
		VertexTransformationFunction(pCamera);

		if (m_primitiveTopology == primitiveTechnology::TriangleStrip)
		{
			TriangleStrip();
		}
		else
		{
			TriangleList();
		}
	}

	void MeshSoftware::VertexTransformationFunction(const Camera* pCamera) const
	{
		ResetVertices();

		for (Vertex_Out* vertex : m_pVertexOut)
		{
			vertex->position = pCamera->GetViewProjectionMatrix().TransformPoint(vertex->position);

			//transform the normal and tagents so its in the correct position
			vertex->normal = pCamera->GetViewProjectionMatrix().TransformVector(vertex->normal).Normalized();
			vertex->tangent = pCamera->GetViewProjectionMatrix().TransformVector(vertex->tangent).Normalized();

			//get the viewDirection
			vertex->viewDirection = (vertex->position.GetXYZ() - pCamera->GetOrigin()).Normalized();

			/*vertex->position = pCamera->GetViewProjectionMatrix().TransformPoint(vertex->position);*/

			vertex->position.x /= vertex->position.w;
			vertex->position.y /= vertex->position.w;
			vertex->position.z /= vertex->position.w;

			vertex->position.z = 1.f / vertex->position.z;

			vertex->position.x = (vertex->position.x + 1.f) / 2.f * static_cast<float>(m_Width);
			vertex->position.y = (1.f - vertex->position.y) / 2.f * static_cast<float>(m_Height);
		}
	}

	void MeshSoftware::TriangleStrip() const
	{
		for (size_t i = 0; i < m_Indices.size() - 2; i++)
		{
			size_t v0;
			size_t v1;
			size_t v2;
			if (i % 2 == 0)
			{
				v0 = m_Indices[i];
				v1 = m_Indices[i + 1];
				v2 = m_Indices[i + 2];
			}
			else
			{
				v0 = m_Indices[i];
				v1 = m_Indices[i + 2];
				v2 = m_Indices[i + 1];
			}

			Rasterize(v0, v1, v2);
		}
	}

	void MeshSoftware::TriangleList() const
	{
		for (size_t i = 0; i < m_Indices.size() - 2; i += 3)
		{
			const size_t v0 = m_Indices[i];
			const size_t v1 = m_Indices[i + 1];
			const size_t v2 = m_Indices[i + 2];
			Rasterize(v0, v1, v2);
		}
	}

	void MeshSoftware::Rasterize(const size_t v0, const size_t v1, const size_t v2) const
	{
		const Vector2 V0 = { m_pVertexOut[v0]->position.x, m_pVertexOut[v0]->position.y };
		const Vector2 V1 = { m_pVertexOut[v1]->position.x, m_pVertexOut[v1]->position.y };
		const Vector2 V2 = { m_pVertexOut[v2]->position.x, m_pVertexOut[v2]->position.y };

		const float w0 = m_pVertexOut[v0]->position.w;
		const float w1 = m_pVertexOut[v1]->position.w;
		const float w2 = m_pVertexOut[v2]->position.w;

		//dont know if this should be here but it fixes a chrash
		if (w0 < 0 || w1 < 0 || w2 < 0) return;

		const BoundaryBox boundaryBox{
			{
				std::clamp(std::min({V0.x, V1.x, V2.x}), 0.0f, static_cast<float>(m_Width - 1)),
				std::clamp(std::min({V0.y, V1.y, V2.y}), 0.0f, static_cast<float>(m_Height - 1))
			},
			{
				std::clamp(std::max({V0.x, V1.x, V2.x}), 0.0f, static_cast<float>(m_Width - 1)),
				std::clamp(std::max({V0.y, V1.y, V2.y}), 0.0f, static_cast<float>(m_Height - 1))
			}
		};

		for (int py = boundaryBox.Smallest.y; py <= boundaryBox.Biggest.y; ++py)
		{
			for (int px = boundaryBox.Smallest.x; px <= boundaryBox.Biggest.x; ++px)
			{
				// Calculate the pixel center point in screen space
				const Vector2 P(px + 0.5f, py + 0.5f);

				// Calculate sub-areas for barycentric coordinates
				float W0 = Vector2::Cross(V2 - V1, P - V1); // Opposite V0
				if (W0 <= 0) continue;
				float W1 = Vector2::Cross(V0 - V2, P - V2); // Opposite V1
				if (W1 <= 0) continue;
				float W2 = Vector2::Cross(V1 - V0, P - V0); // Opposite V2
				if (W2 <= 0) continue;

				const float totalArea = W0 + W1 + W2;
				const Vector3 weights
				{
					W0 / totalArea,
					W1 / totalArea,
					W2 / totalArea
				};

				//get the depth of triangle
				const float nonlinearDepth = 1.0f / (
					weights.x / (1.f / m_pVertexOut[v0]->position.z) +
					weights.y / (1.f / m_pVertexOut[v1]->position.z) +
					weights.z / (1.f / m_pVertexOut[v2]->position.z));

				//get the index where in screen this pixel is
				const int pixelIndex = px + py * m_Width;

				if (nonlinearDepth < 0 || nonlinearDepth > 1)
					continue;

				if (nonlinearDepth < m_pDepthBufferPixels[pixelIndex])
				{
					m_pDepthBufferPixels[pixelIndex] = nonlinearDepth;

					const Vector2 uv0 = m_pVertexOut[v0]->uv;
					const Vector2 uv1 = m_pVertexOut[v1]->uv;
					const Vector2 uv2 = m_pVertexOut[v2]->uv;

					const float linearDepth = 1.0f / (
						weights.x / w0 +
						weights.y / w1 +
						weights.z / w2);

					const Vector2 interpolatedUV = linearDepth * (
						uv0 / w0 * weights.x +
						uv1 / w1 * weights.y +
						uv2 / w2 * weights.z);

					const Vector3 InterpolatedNormal = linearDepth * (
						m_pVertexOut[v0]->normal / w0 * weights.x +
						m_pVertexOut[v1]->normal / w1 * weights.y +
						m_pVertexOut[v2]->normal / w2 * weights.z);

					const Vector3 InterpolatedTangent = linearDepth * (
						m_pVertexOut[v0]->tangent / w0 * weights.x +
						m_pVertexOut[v1]->tangent / w1 * weights.y +
						m_pVertexOut[v2]->tangent / w2 * weights.z);

					const Vector3 InterpolatedViewDirection = linearDepth * (
						m_pVertexOut[v0]->viewDirection / w0 * weights.x +
						m_pVertexOut[v1]->viewDirection / w1 * weights.y +
						m_pVertexOut[v2]->viewDirection / w2 * weights.z);

					const float remappedDepth = std::clamp(Remap(nonlinearDepth, 0.985f, 1.0f), 0.0f, 1.0f);
					const ColorRGB InterpolatedColor = { remappedDepth, remappedDepth, remappedDepth };

					const Vertex_Out vertex
					{
						{0,0,0,0},
						InterpolatedColor,
						interpolatedUV,
						InterpolatedNormal,
						InterpolatedTangent,
						InterpolatedViewDirection
					};

					PixelShading(pixelIndex, vertex);
				}
			}
		}

	}

	void MeshSoftware::PixelShading(const int pixelIndex, const Vertex_Out& vertex_out) const
	{
		Vector3 SampeldNormal{ vertex_out.normal };
		ColorRGB sampleDDiffuseColor{ 1,1,1, };
		float PhongSpecular{ m_PhongSpecular };
		float PhongExponent{ m_Shininess };

		if (m_pMaterial->pDiffuse)
			sampleDDiffuseColor = m_pMaterial->pDiffuse->Sample(vertex_out.uv);

		if (m_pMaterial->pSpecular)
			PhongSpecular *= m_pMaterial->pSpecular->Sample(vertex_out.uv).r; //only one is needed can be r,g or b

		if (m_pMaterial->pGloss)
			PhongExponent *= m_pMaterial->pGloss->Sample(vertex_out.uv).r;	//only one is needed can be r,g or b

		if (m_pMaterial->pNormal && m_NormalMapActive)
		{
			Vector3 binormal = Vector3::Cross(vertex_out.normal, vertex_out.tangent);

			Matrix tangentSpaceAxis{
				vertex_out.tangent,
				binormal,
				vertex_out.normal,
				Vector3::Zero
			};

			ColorRGB sampleNormalColor = m_pMaterial->pNormal->Sample(vertex_out.uv);
			Vector3 sampledNormalMapped{
				2.f * sampleNormalColor.r - 1.f,
				2.f * sampleNormalColor.g - 1.f,
				2.f * sampleNormalColor.b - 1.f
			};

			SampeldNormal = tangentSpaceAxis.TransformPoint(sampledNormalMapped);
		}

		ColorRGB LambertDiffuse = sampleDDiffuseColor * m_DiffuseReflectance / PI;

		const float observedArea = std::max(0.0f, Vector3::Dot(SampeldNormal, -m_LightDirection));

		//phong
		Vector3 reflect{ Vector3::Reflect(m_LightDirection, SampeldNormal) };
		float cosAlpa{ std::max(0.f,Vector3::Dot(reflect,vertex_out.viewDirection)) };
		ColorRGB PhongColor = colors::White * PhongSpecular * pow(cosAlpa, PhongExponent);

		ColorRGB finalColor{};

		if (m_DepthToggle)
		{
			finalColor = vertex_out.color;
		}
		else
		{
			switch (m_CurrentLightingMode)
			{
			case LightingMode::Diffuse:
				finalColor = LambertDiffuse;
				break;
			case LightingMode::ObservedArea:
				finalColor += colors::White * observedArea;
				break;
			case LightingMode::Specular:
				finalColor = colors::White * PhongColor;
				break;
			case LightingMode::Combined:
				finalColor = PhongColor + LambertDiffuse * observedArea;
				break;
			}
		}

		finalColor.MaxToOne();

		m_pBackBufferPixels[pixelIndex] = SDL_MapRGB(m_pBackBuffer->format,
			static_cast<uint8_t>(finalColor.r * 255),
			static_cast<uint8_t>(finalColor.g * 255),
			static_cast<uint8_t>(finalColor.b * 255));
	}


	float MeshSoftware::Remap(float value, float fromMin, float fromMax, float toMin, float toMax) const
	{
		return (value - fromMin) / (fromMax - fromMin) * (toMax - toMin) + toMin;
	}
	void MeshSoftware::ResetVertices() const
	{
		for (size_t i = 0; i < m_pVertexOut.size(); ++i)
		{
			m_pVertexOut[i]->position = { m_modelVertices[i].Position.x, m_modelVertices[i].Position.y, m_modelVertices[i].Position.z, 1.f };
			m_pVertexOut[i]->color = m_modelVertices[i].Color;
			m_pVertexOut[i]->uv = m_modelVertices[i].uv;
			m_pVertexOut[i]->normal = m_modelVertices[i].normal;
			m_pVertexOut[i]->tangent = m_modelVertices[i].tangent;
		}
	}
}
