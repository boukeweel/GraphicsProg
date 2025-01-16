#pragma once

#include "EffectBase.h"

namespace dae
{
	class EffectOpaque final : public EffectBase
	{
	public:
		EffectOpaque(ID3D11Device* devicePtr, const std::wstring& effectFileName);

		void SetUseNormalMap(bool useNormalMap) const;
		void SetLightIntensity(float lightIntensity) const;
		void SetPhongExponent(float phongExponent) const;
		void SetPhongSpecular(float phongSpecular) const;
		void SetAmbientColor(const Vector3& ambientColor) const;

	private:
		ID3DX11EffectVariable* m_pUseNormalMapVariable{};

		ID3DX11EffectScalarVariable* m_pLightIntensity{};
		ID3DX11EffectScalarVariable* m_pPhongExponent{};
		ID3DX11EffectScalarVariable* m_pPhongSpecular{};

		ID3DX11EffectVectorVariable* m_pAmbientColor{};
	};
}
