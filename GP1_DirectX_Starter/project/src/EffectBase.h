#pragma once
#include "pch.h"

namespace dae {
	class Texture;

	enum class CullMode
	{
		Front,
		Back,
		None,
	};

	class EffectBase {
	public:
		EffectBase(ID3D11Device* pDevice, const std::wstring& path);
		virtual ~EffectBase();

		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

		ID3DX11Effect* GetEffect(){	return m_pEffect; }

		ID3DX11EffectTechnique* GetTechnique(){ return m_pTechnique; }

		void SetSampleState(int state) const;

		void SetViewProjectionMatrix(const Matrix& viewProjectionMatrix) const;
		void SetMeshWorldMatrix(const Matrix& worldMatrix) const;
		void SetCamaraOrigin(const Vector3& origin) const;
		void SetLightingDirection(const Vector3& direction) const;

		void SetDiffuseMap(const Texture* pDiffuseTexture) const;
		void SetNormalMap(const Texture* pNormalTexture) const;
		void SetSpecularMap(const Texture* pSpecularTexture) const;
		void SetGlossMap(const Texture* pGlossTexture) const;

		void CreateRasterizerStates(ID3D11RasterizerState** pRSNone, ID3D11RasterizerState** pRSFront, ID3D11RasterizerState** pRSBack);
		void SetCullMode(ID3D11DeviceContext* pDeviceContext, CullMode cullMode);

	protected:
		ID3DX11Effect* m_pEffect{};
	private:

		ID3D11Device* m_pDevice{};
		
		ID3DX11EffectTechnique* m_pTechnique{};

		ID3DX11EffectSamplerVariable* m_pSampleStateVariable{};

		ID3DX11EffectMatrixVariable* m_pWorldViewProjectionMatrixVar{};
		ID3DX11EffectMatrixVariable* m_pMeshWorldMatrix{};

		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pGlossMapVariable{};

		ID3D11RasterizerState* m_pRSNone{nullptr};
		ID3D11RasterizerState* m_pRSFront{ nullptr };
		ID3D11RasterizerState* m_pRSBack{ nullptr };

		ID3DX11EffectVectorVariable* m_pLightDirectionVariable{};
		ID3DX11EffectVectorVariable* m_pCameraOriginVariable{};
	};
}
