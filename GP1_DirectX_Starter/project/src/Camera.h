#pragma once
#include "MathHelpers.h"
#include "Matrix.h"

namespace dae
{
	class Timer;
	struct Vector3;

	class Camera
	{
	public:
		Camera(const Vector3& _origin, float _fovAngle, float _aspectRatio);

		void Initialize(Vector3 _origin = { 0.f,0.f,0.f }, float _fovAngle = 90.f, float _aspectRatio = 0);
		void Update(const Timer* pTimer);

		[[nodiscard]] const Matrix& GetViewMatrix() const { return m_ViewMatrix; }
		[[nodiscard]] const Matrix& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		[[nodiscard]] const Matrix& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
		[[nodiscard]] const Vector3& GetOrigin() const { return m_Origin; }

		/*void SetFovAngle(float fovAngle);
		void SetPosition(Vector3 position, bool teleport = true);
		void SetNearClipping(float value);
		void SetFarClipping(float value);
		void SetPitch(float pitch);
		void SetYaw(float yaw);
		void ChangeFovAngle(float fovAngleChange);*/

		void CalculateViewMatrix();
		void CalculateProjectionMatrix();
		void CombineViewProjectionMatrix();
	private:
		Vector3 m_Origin;
		float m_FovAngle{ 90.f };
		float m_Fov{ tanf((m_FovAngle * TO_RADIANS) / 2.f) };
		float m_AspectRatio{};

		float m_NearClippingPlane = .1f;
		float m_FarClippingPlane = 1000.f;

		Vector3 m_Forward{ Vector3::UnitZ };
		Vector3 m_Up{ Vector3::UnitY };
		Vector3 m_Right{ Vector3::UnitX };

		float m_TotalPitch{};
		float m_TotalYaw{};

		const float m_MoveSpeedKey{ 100.f };
		const float m_MoveSpeedMouse{ 100.f };
		const float m_RotatedSpeed{ 0.01f };

		Matrix m_InvViewMatrix{};
		Matrix m_ViewMatrix{};
		Matrix m_ProjectionMatrix{};
		Matrix m_ViewProjectionMatrix{};

	};
}

