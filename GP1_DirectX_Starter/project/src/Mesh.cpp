#include "Mesh.h"
#include "Utils.h"

namespace dae
{
	Mesh::Mesh(ID3D11Device* pDevice, Effect* pEffect, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Material* pMaterial)
		:m_pEffects{pEffect},m_modelVertices{std::move(vertices)},m_Indices{indices},m_IndicesCount{ static_cast<UINT>(m_Indices.size()) },m_pMaterial{pMaterial}
	{
		InitializeMesh(pDevice);
	}

	Mesh::Mesh(ID3D11Device* pDevice, Effect* pEffect, const std::string& objName, Material* pMaterial)
		:m_pEffects{pEffect},m_pMaterial(pMaterial)
	{
		Utils::ParseOBJ(objName, m_modelVertices, m_Indices);

		m_IndicesCount = static_cast<UINT>(m_Indices.size());

		InitializeMesh(pDevice);
	}

	Mesh::~Mesh()
	{
		delete m_pMaterial;

		SafeRelease(m_pIndexBuffer)
		SafeRelease(m_pVertexBuffer)
		SafeRelease(m_pInputLayout)
	}

	void Mesh::Render(ID3D11DeviceContext* pDeviceContext, const Matrix& viewProjectionMatrix) const
	{
		m_pEffects->SetViewProjectionMatrix(viewProjectionMatrix);
		m_pEffects->SetMeshWorldMatrix(m_WorldMatrix);

		m_pEffects->SetDiffuseMap(m_pMaterial->pDiffuse);

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

	void Mesh::InitializeMesh(ID3D11Device* pDevice)
	{
		HRESULT result{};

		static constexpr uint32_t numElements{ 3 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "COLOR";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName = "TEXCOORD";
		vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


		D3DX11_PASS_DESC passDesc{};
		ID3DX11EffectTechnique* pTechnique = m_pEffects->GetTechnique();
		pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

		result = pDevice->CreateInputLayout(
			vertexDesc,
			numElements,
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,
			&m_pInputLayout);

		if(FAILED(result))
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

		if(FAILED(result))
			return;

	}

	void Mesh::SetPosition(Vector3 translate)
	{
		m_Position = translate;
		UpdateWorldMatrix();
	}

	void Mesh::SetScale(Vector3 scale)
	{
		m_Scale = scale;
		UpdateWorldMatrix();
	}

	void Mesh::SetYawRotation(float yaw)
	{
		m_YawRotation = yaw;
		UpdateWorldMatrix();
	}

	void Mesh::AddYawRotation(float yawDelta)
	{
		m_YawRotation += yawDelta;
		UpdateWorldMatrix();
	}



	void Mesh::UpdateWorldMatrix()
	{
		const Matrix translationMatrix = Matrix::CreateTranslation(m_Position);
		const Matrix scaleMatrix = Matrix::CreateScale(m_Scale);
		const Matrix rotationMatrix = Matrix::CreateRotationY(m_YawRotation);

		m_WorldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	}

}
