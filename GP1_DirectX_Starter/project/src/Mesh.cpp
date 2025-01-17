#include "Mesh.h"
#include "Utils.h"
#include "Texture.h"

namespace dae
{
	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Material* pMaterial)
		:m_modelVertices{std::move(vertices)},m_Indices{indices},m_IndicesCount{ static_cast<UINT>(m_Indices.size()) },m_pMaterial{pMaterial}
	{
	}

	Mesh::Mesh(const std::string& objName, Material* pMaterial)
		:m_pMaterial(pMaterial)
	{
		Utils::ParseOBJ(objName, m_modelVertices, m_Indices);

		m_IndicesCount = static_cast<UINT>(m_Indices.size());
	}

	Mesh::~Mesh()
	{
		delete m_pMaterial->pDiffuse;
		delete m_pMaterial->pNormal;
		delete m_pMaterial->pSpecular;
		delete m_pMaterial->pGloss;
		delete m_pMaterial;
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

	void Mesh::CycleCullMode()
	{
		switch (m_CullMode)
		{
		case CullMode::None:
			m_CullMode = CullMode::Front;
			std::cout << "Current CullMode: Front\n";
			break;
		case CullMode::Front:
			m_CullMode = CullMode::Back;
			std::cout << "Current CullMode: Back\n";
			break;
		case CullMode::Back:
			m_CullMode = CullMode::None;
			std::cout << "Current CullMode: None\n";
			break;
		}
	}

	void Mesh::UpdateWorldMatrix()
	{
		const Matrix translationMatrix = Matrix::CreateTranslation(m_Position);
		const Matrix scaleMatrix = Matrix::CreateScale(m_Scale);
		const Matrix rotationMatrix = Matrix::CreateRotationY(m_YawRotation);

		m_WorldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	}

}
