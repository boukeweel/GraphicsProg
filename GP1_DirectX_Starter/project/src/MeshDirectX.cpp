#include "MeshDirectX.h"

namespace dae
{
	MeshDirectX::MeshDirectX(ID3D11Device* pDevice, EffectBase* pEffect, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Material* pMaterial)
		:Mesh(vertices,indices,pMaterial),m_pEffects{pEffect}
	{
		InitializeMesh(pDevice);
	}

	MeshDirectX::MeshDirectX(ID3D11Device* pDevice, EffectBase* pEffect, const std::string& objName, Material* pMaterial)
		:Mesh(objName,pMaterial),m_pEffects{pEffect}
	{
		InitializeMesh(pDevice);
	}

	MeshDirectX::~MeshDirectX()
	{
		SafeRelease(m_pIndexBuffer)
		SafeRelease(m_pVertexBuffer)
		SafeRelease(m_pInputLayout)
	}

	void MeshDirectX::Render(ID3D11DeviceContext* pDeviceContext, const Matrix& viewProjectionMatrix) const
	{
		m_pEffects->SetViewProjectionMatrix(viewProjectionMatrix);
		m_pEffects->SetMeshWorldMatrix(m_WorldMatrix);

		m_pEffects->SetDiffuseMap(m_pMaterial->pDiffuse);
		m_pEffects->SetNormalMap(m_pMaterial->pNormal);
		m_pEffects->SetSpecularMap(m_pMaterial->pSpecular);
		m_pEffects->SetGlossMap(m_pMaterial->pGloss);

		m_pEffects->SetCullingMode(m_CullMode);

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pDeviceContext->IASetInputLayout(m_pInputLayout);

		constexpr UINT stride = sizeof(Vertex);
		constexpr UINT offset = 0;

		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		D3DX11_TECHNIQUE_DESC techDesc{};
		m_pEffects->GetTechnique()->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			m_pEffects->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_IndicesCount, 0, 0);
		}
	}


	void MeshDirectX::InitializeMesh(ID3D11Device* pDevice)
	{
		HRESULT result{};

		static constexpr uint32_t numElements{ 5 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]
		{
		{"POSITION", 0,DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR", 0,DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0,DXGI_FORMAT_R32G32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL", 0,DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TANGENT", 0,DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
		};

		D3DX11_PASS_DESC passDesc{};
		ID3DX11EffectTechnique* pTechnique = m_pEffects->GetTechnique();
		pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

		result = pDevice->CreateInputLayout(
			vertexDesc,
			numElements,
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,
			&m_pInputLayout);

		if (FAILED(result))
			return;

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(m_modelVertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData{};

		initData.pSysMem = m_modelVertices.data();

		result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
		if (FAILED(result))
			return;

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * m_IndicesCount;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		initData.pSysMem = m_Indices.data();
		result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);

		if (FAILED(result))
			return;

	}
	
}
