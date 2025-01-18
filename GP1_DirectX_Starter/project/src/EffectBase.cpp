#include "EffectBase.h"
#include <iostream>
#include <sstream>
#include "Texture.h"

namespace dae
{
	dae::EffectBase::EffectBase(ID3D11Device* pDevice, const std::wstring& path)
	: m_pDevice{pDevice}
	{
		m_pEffect = LoadEffect(pDevice, path);

		m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
		if (!m_pTechnique->IsValid())
			std::wcout << L"Technique not Valid\n";

		m_pSampleStateVariable = m_pEffect->GetVariableByName("g_TextureSampler")->AsSampler();
		if (!m_pSampleStateVariable->IsValid())
			std::wcout << L"m_pSampleStateVariable not valid \n";

		m_pWorldViewProjectionMatrixVar = m_pEffect->GetVariableByName("g_WorldViewProjection")->AsMatrix();
		if (!m_pWorldViewProjectionMatrixVar->IsValid())
			std::wcout << L"m_pWorldViewProjectionMatrixVar not valid \n";

		m_pMeshWorldMatrix = m_pEffect->GetVariableByName("g_MeshWorldMatrix")->AsMatrix();
		if (!m_pMeshWorldMatrix->IsValid())
			std::wcout << L"m_pMeshWorldMatrix not valid \n";

		m_pCameraOriginVariable = m_pEffect->GetVariableByName("g_CamaraOrigin")->AsVector();
		if (!m_pCameraOriginVariable->IsValid())
			std::wcout << L"m_pCameraOriginVariable not valid \n";

		m_pLightDirectionVariable = m_pEffect->GetVariableByName("g_LightDirection")->AsVector();
		if (!m_pLightDirectionVariable->IsValid())
			std::wcout << L"m_pLightDirectionVariable not valid \n";

		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("g_DiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
			std::wcout << L"m_pDiffuseMapVariable not valid \n";

		m_pNormalMapVariable = m_pEffect->GetVariableByName("g_NormalMap")->AsShaderResource();
		m_pSpecularMapVariable = m_pEffect->GetVariableByName("g_SpecularMap")->AsShaderResource();
		m_pGlossMapVariable = m_pEffect->GetVariableByName("g_GlossMap")->AsShaderResource();

		CreateRasterizerStates(&m_pRSNone, &m_pRSFront, &m_pRSBack);
	}

	EffectBase::~EffectBase()
	{
		SafeRelease(m_pEffect)
	}

	ID3DX11Effect* EffectBase::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetfile)
	{
		HRESULT result;
		ID3D10Blob* pErrorBlob{ nullptr };
		ID3DX11Effect* pEffect;

		DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		result = D3DX11CompileEffectFromFile(assetfile.c_str(),
			nullptr,
			nullptr,
			shaderFlags,
			0,
			pDevice,
			&pEffect,
			&pErrorBlob);

		if (FAILED(result))
		{
			if(pErrorBlob != nullptr)
			{
				const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());
				std::wstringstream ss;
				for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
				{
					ss << pErrors[i];
				}

				OutputDebugStringW(ss.str().c_str());
				pErrorBlob->Release();
				pErrorBlob = nullptr;

				std::wcout << ss.str() << "\n";
			}
			else
			{
				std::wstringstream ss;
				ss << "EffectLoader: failed to create EffectBase From File!\n Path: " << assetfile;
				std::wcout << ss.str() << "\n";
				return nullptr;
			}
		}

		return pEffect;
	}

	void EffectBase::SetSampleState(int state) const
	{
		if (!m_pSampleStateVariable->IsValid())
			return;

		D3D11_SAMPLER_DESC samplerDesc{};
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MipLODBias = 0.0f;                  // No bias
		samplerDesc.MinLOD = 0.0f;                      // Minimum LOD
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;         // Maximum LOD

		switch (state)
		{
		case 0: // Anisotropic Filtering
			samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			samplerDesc.MaxAnisotropy = 16;             // Maximum anisotropy
			break;
		case 1: // Point Filtering
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.MaxAnisotropy = 1;             // Not used for point filtering, but set to 1
			break;
		case 2: // Linear Filtering
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.MaxAnisotropy = 1;             // Not used for linear filtering, but set to 1
			break;
		default:
			std::cout << "Wrong state change" << std::endl;
			return; // Exit early on invalid state
		}

		ID3D11SamplerState* newSampleState = nullptr;
		const HRESULT result = m_pDevice->CreateSamplerState(&samplerDesc, &newSampleState);

		if (FAILED(result))
		{
			std::cerr << "Failed to create sampler state. HRESULT: " << std::hex << result << std::endl;
			return;
		}

		m_pSampleStateVariable->SetSampler(0, newSampleState);

		if (newSampleState)
			newSampleState->Release();
	}



	void EffectBase::SetViewProjectionMatrix(const Matrix& viewProjectionMatrix) const
	{
		if(!m_pWorldViewProjectionMatrixVar->IsValid())
			return;

		m_pWorldViewProjectionMatrixVar->SetMatrix(reinterpret_cast<const float*>(&viewProjectionMatrix));
	}

	void EffectBase::SetMeshWorldMatrix(const Matrix& WorldMatrix) const
	{
		if (!m_pMeshWorldMatrix->IsValid())
			return;

		m_pMeshWorldMatrix->SetMatrix(reinterpret_cast<const float*>(&WorldMatrix));
	}

	void EffectBase::SetCamaraOrigin(const Vector3& origin) const
	{
		if(!m_pCameraOriginVariable->IsValid())
			return;

		m_pCameraOriginVariable->SetFloatVector(reinterpret_cast<const float*>(&origin));
	}

	void EffectBase::SetLightingDirection(const Vector3& direction) const
	{
		if(!m_pLightDirectionVariable->IsValid())
			return;

		m_pLightDirectionVariable->SetFloatVector(reinterpret_cast<const float*>(&direction));
	}

	void EffectBase::SetDiffuseMap(const Texture* pDiffuseTexture) const
	{
		if (!m_pDiffuseMapVariable->IsValid() && pDiffuseTexture == nullptr)
			return;

		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetShaderResource());
	}

	void EffectBase::SetNormalMap(const Texture* pNormalTexture) const
	{
		if(!m_pNormalMapVariable->IsValid() && pNormalTexture == nullptr)
			return;

		m_pNormalMapVariable->SetResource(pNormalTexture->GetShaderResource());
	}

	void EffectBase::SetSpecularMap(const Texture* pSpecularTexture) const
	{
		if(!m_pSpecularMapVariable->IsValid() && pSpecularTexture == nullptr)
			return;

		m_pSpecularMapVariable->SetResource(pSpecularTexture->GetShaderResource());
	}

	void EffectBase::SetGlossMap(const Texture* pGlossTexture) const
	{
		if(!m_pGlossMapVariable->IsValid() && pGlossTexture == nullptr)
			return;

		m_pGlossMapVariable->SetResource(pGlossTexture->GetShaderResource());
	}

	void EffectBase::CreateRasterizerStates(ID3D11RasterizerState** pRSNone, ID3D11RasterizerState** pRSFront, ID3D11RasterizerState** pRSBack)
	{
		D3D11_RASTERIZER_DESC rasterDesc{};
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.DepthClipEnable = true;

		// None
		rasterDesc.CullMode = D3D11_CULL_NONE;
		m_pDevice->CreateRasterizerState(&rasterDesc, pRSNone);

		// Front
		rasterDesc.CullMode = D3D11_CULL_FRONT;
		m_pDevice->CreateRasterizerState(&rasterDesc, pRSFront);

		// Back
		rasterDesc.CullMode = D3D11_CULL_BACK;
		m_pDevice->CreateRasterizerState(&rasterDesc, pRSBack);
	}

	void EffectBase::SetCullMode(ID3D11DeviceContext* pDeviceContext, CullMode cullMode)
	{
		switch (cullMode)
		{
		case CullMode::None:
			pDeviceContext->RSSetState(m_pRSNone);
			break;
		case CullMode::Front:
			pDeviceContext->RSSetState(m_pRSFront);
			break;
		case CullMode::Back:
			pDeviceContext->RSSetState(m_pRSBack);
			break;
		}
	}

}
