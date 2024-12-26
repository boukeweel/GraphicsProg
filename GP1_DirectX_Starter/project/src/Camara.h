#pragma once
#include <corecrt_math.h>

#include "MathHelpers.h"
#include "Matrix.h"
#include "Timer.h"
#include "Vector3.h"

namespace dae
{
	class Camara
	{
	public:
		Camara() = default;
		~Camara() = default;

		Camara(const Vector3 origin, float fovAngle, float aspectRatio);
		void Initialize(float _fovAngle = 90.f, Vector3 _origin = { 0.f,0.f,0.f }, float _aspectRatio = 0);

		void Update(const Timer* pTimer);

		Matrix GetViewMatrix()
		{
			return m_ViewMatrix;
		}
		Matrix GetProjectMatrix()
		{
			return m_ProjectionMatrix;
		}
		Matrix GetViewPorjectionMatrix()
		{
			return m_ViewMatrix * m_ProjectionMatrix;
		}

	private:
		void CalculateViewMatrix();

		void CalculateProjectionMatrix();

		Vector3 m_Origin{};
		float M_FovAngle{ 90.f };
		float m_Fov{ tanf((M_FovAngle * TO_RADIANS) / 2.f) };
		float m_AspectRatio{};

		float m_NearClippingPlane = .1f;
		float m_FarClippingPlane = 1000.f;

		Vector3 m_Forward{ Vector3::UnitZ };
		Vector3 m_Up{ Vector3::UnitY };
		Vector3 m_Right{ Vector3::UnitX };

		float m_TotalPitch{};
		float m_TotalYaw{};

		const float m_MoveSpeedKey{ 5.f };
		const float m_MoveSpeedMouse{ 0.5f };
		const float m_RotatedSpeed{ 0.01f };

		Matrix m_InvViewMatrix{};
		Matrix m_ViewMatrix{};

		Matrix m_ProjectionMatrix;
	};
}

