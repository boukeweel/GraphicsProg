#include "EffectOpaque.h"


namespace dae
{
	EffectOpaque::EffectOpaque(ID3D11Device* devicePtr, const std::wstring& effectFileName)
		:EffectBase(devicePtr,effectFileName)
	{

		m_pUseNormalMapVariable = m_pEffect->GetVariableByName("g_UseNormalMap")->AsScalar();
		if (!m_pUseNormalMapVariable->IsValid())
			std::wcout << L"m_pUseNormalMapVariable not valid \n";

		m_pLightIntensity = m_pEffect->GetVariableByName("g_LightIntensity")->AsScalar();
		if (!m_pLightIntensity->IsValid())
			std::wcout << L"m_pLightIntensity not valid \n";

		m_pPhongExponent = m_pEffect->GetVariableByName("g_PhongExponent")->AsScalar();
		if (!m_pPhongExponent->IsValid())
			std::wcout << L"m_pPhongExponent not valid \n";

		m_pPhongSpecular = m_pEffect->GetVariableByName("g_PhongSpecular")->AsScalar();
		if (!m_pPhongSpecular->IsValid())
			std::wcout << L"m_pPhongSpecular not valid \n";

		m_pAmbientColor = m_pEffect->GetVariableByName("g_AmbientColor")->AsVector();
		if (!m_pAmbientColor->IsValid())
			std::wcout << L"m_pAmbientColor not valid \n";

	}

	void EffectOpaque::SetUseNormalMap(bool useNormalMap) const
	{
		if(!m_pUseNormalMapVariable->IsValid())
			return;

		m_pUseNormalMapVariable->SetRawValue(&useNormalMap, 0, sizeof(bool));
	}

	void EffectOpaque::SetLightIntensity(float lightIntensity) const
	{
		if(!m_pLightIntensity->IsValid())
			return;

		m_pLightIntensity->SetFloat(lightIntensity);
	}

	void EffectOpaque::SetPhongExponent(float phongExponent) const
	{
		if(!m_pPhongExponent->IsValid())
			return;

		m_pPhongExponent->SetFloat(phongExponent);
	}

	void EffectOpaque::SetPhongSpecular(float phongSpecular) const
	{
		if(!m_pPhongSpecular->IsValid())
			return;

		m_pPhongSpecular->SetFloat(phongSpecular);
	}

	void EffectOpaque::SetAmbientColor(const Vector3& ambientColor) const
	{
		if(!m_pAmbientColor->IsValid())
			return;

		m_pAmbientColor->SetFloatVector(reinterpret_cast<const float*>(&ambientColor));
	}
}

