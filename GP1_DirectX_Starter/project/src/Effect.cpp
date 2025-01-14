#include "Effect.h"
#include <iostream>
#include <sstream>

#include "Texture.h"


#define SafeRelease(p) { if (p) { p->Release(); p = nullptr; } }

namespace dae
{
	dae::Effect::Effect(ID3D11Device* pDevice, const std::wstring& path)
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
		
		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("g_DiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
			std::wcout << L"m_pDiffuseMapVariable not valid \n";
	}

	Effect::~Effect()
	{
		SafeRelease(m_pEffect)
	}

	ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetfile)
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
				ss << "EffectLoader: failed to create Effect From File!\n Path: " << assetfile;
				std::wcout << ss.str() << "\n";
				return nullptr;
			}
		}

		return pEffect;
	}

	void Effect::SetSampleState(int state) const
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
			std::cout << "ANISOTROPIC" << std::endl;
			samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			samplerDesc.MaxAnisotropy = 16;             // Maximum anisotropy
			break;
		case 1: // Point Filtering
			std::cout << "POINT" << std::endl;
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.MaxAnisotropy = 1;             // Not used for point filtering, but set to 1
			break;
		case 2: // Linear Filtering
			std::cout << "LINEAR" << std::endl;
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



	void Effect::SetViewProjectionMatrix(const Matrix& viewProjectionMatrix) const
	{
		if(!m_pWorldViewProjectionMatrixVar->IsValid())
			return;

		m_pWorldViewProjectionMatrixVar->SetMatrix(reinterpret_cast<const float*>(&viewProjectionMatrix));
	}
	void Effect::SetDiffuseMap(const Texture* pDiffuseTexture) const
	{
		if (m_pDiffuseMapVariable)
			m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetShaderResource());
	}
}
