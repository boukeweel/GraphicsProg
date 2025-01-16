#pragma once
#include "Mesh.h"

namespace dae {

	class MeshDirectX : public Mesh {

	public:
		MeshDirectX(ID3D11Device* pDevice, EffectBase* pEffect, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Material* pMaterial);
		MeshDirectX(ID3D11Device* pDevice, EffectBase* pEffect, const std::string& objName, Material* pMaterial);

		~MeshDirectX() override;

		void Render(ID3D11DeviceContext* pDeviceContext, const Matrix& viewProjectionMatrix) const;
	private:
		void InitializeMesh(ID3D11Device* pDevice);

		EffectBase* m_pEffects{ nullptr };

		ID3D11Buffer* m_pVertexBuffer{ nullptr };
		ID3D11Buffer* m_pIndexBuffer{ nullptr };
		ID3D11InputLayout* m_pInputLayout{ nullptr };
	};
}