#pragma once
#include "pch.h"

namespace dae {

	class Effect {
	public:
		Effect(ID3D11Device* pDevice, const std::wstring& path);
		~Effect();

		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

		const ID3DX11Effect* GetEffect()
		{
			return m_pEffect;
		}

		const ID3DX11EffectTechnique* GetTechnique()
		{
			return m_pTechnique;
		}

	private:

		ID3DX11Effect* m_pEffect{};
		ID3DX11EffectTechnique* m_pTechnique{};
	};
}
