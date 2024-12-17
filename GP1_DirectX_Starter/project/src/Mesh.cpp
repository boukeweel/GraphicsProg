#include "Mesh.h"

namespace dae
{
	Mesh::Mesh(ID3D11Device* pDevice, std::vector<VS_INPUT> vertices, std::vector<uint32_t> indices):
		m_pDevice{ pDevice }, m_Vertices(std::move(vertices)), m_Indices{ std::move(indices) }
	{

	}

	Mesh::~Mesh()
	{
		if(m_pDevice)
		{
			m_pDevice->Release();
			m_pDevice = nullptr;
		}
	}


}
