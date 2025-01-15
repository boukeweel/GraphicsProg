#pragma once
#include "pch.h"

namespace dae
{
	class Texture;
}

namespace dae {

	class Effect {
	public:
		Effect(ID3D11Device* pDevice, const std::wstring& path);
		~Effect();

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

	private:

		ID3D11Device* m_pDevice{};
		ID3DX11Effect* m_pEffect{};
		ID3DX11EffectTechnique* m_pTechnique{};

		ID3DX11EffectSamplerVariable* m_pSampleStateVariable{};

		ID3DX11EffectMatrixVariable* m_pWorldViewProjectionMatrixVar{};
		ID3DX11EffectMatrixVariable* m_pMeshWorldMatrix{};

		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pGlossMapVariable{};

		ID3DX11EffectVectorVariable* m_pLightDirectionVariable{};
		ID3DX11EffectVectorVariable* m_pCameraOriginVariable{};

	};
}
