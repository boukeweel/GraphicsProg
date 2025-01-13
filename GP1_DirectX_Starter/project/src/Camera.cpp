#include "Camera.h"

#include "SDL_keyboard.h"
#include "SDL_mouse.h"
#include "Timer.h"

namespace dae
{
	Camera::Camera(const Vector3& _origin, float _fovAngle, float _aspectRatio)
	{
		Initialize(_origin, _fovAngle, _aspectRatio);
	}

	void Camera::Initialize(Vector3 _origin, float _fovAngle, float _aspectRatio)
	{
		m_FovAngle = _fovAngle;
		m_Fov = tanf((m_FovAngle * TO_RADIANS) / 2.f);
		m_AspectRatio = _aspectRatio;
		m_Origin = _origin;
	}

	void Camera::CalculateViewMatrix()
	{
		m_InvViewMatrix = Matrix::CreateLookAtLH(m_Origin, m_Forward, m_Up);
		m_ViewMatrix = m_InvViewMatrix.Inverse();
	}

	void Camera::CalculateProjectionMatrix()
	{
		const float A{ m_FarClippingPlane / (m_FarClippingPlane - m_NearClippingPlane) };
		const float B{ -(m_FarClippingPlane * m_NearClippingPlane) / (m_FarClippingPlane - m_NearClippingPlane) };

		m_ProjectionMatrix = Matrix::CreatePerspectiveFovLH(m_Fov, m_AspectRatio, A, B);
	}

	void Camera::CombineViewProjectionMatrix()
	{
		m_ViewProjectionMatrix = m_InvViewMatrix * m_ProjectionMatrix;
	}

	void Camera::Update(const Timer* pTimer)
	{
		const float deltaTime = pTimer->GetElapsed();

		Vector3 InputVector{};

		//Keyboard Input
		const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);


		if (pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN])
			InputVector.z -= 1;

		if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_UP])
			InputVector.z += 1;

		if (pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT])
			InputVector.x -= 1;

		if (pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT])
			InputVector.x += 1;

		if (pKeyboardState[SDL_SCANCODE_Q])
			InputVector.y -= 1;

		if (pKeyboardState[SDL_SCANCODE_E])
			InputVector.y += 1;

		//Mouse Input
		int mouseX{}, mouseY{};
		const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

		const bool MouseRightPressed{ (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0 };
		const bool MouseLeftPressed{ (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0 };

		if (MouseRightPressed && MouseLeftPressed)
		{
			InputVector.y -= mouseY * m_MoveSpeedMouse;
		}
		else if (!MouseRightPressed && MouseLeftPressed)
		{
			InputVector.z -= mouseY * m_MoveSpeedMouse;
			m_TotalYaw -= mouseX * m_RotatedSpeed;
		}
		else if (MouseRightPressed && !MouseLeftPressed)
		{
			m_TotalPitch -= mouseY * m_RotatedSpeed;
			m_TotalYaw -= mouseX * m_RotatedSpeed;
		}

		const Matrix pitchYawRotation
		{
			Vector3{cosf(m_TotalYaw), 0, sinf(m_TotalYaw)},
			Vector3{sinf(m_TotalYaw) * sinf(m_TotalPitch), cosf(m_TotalPitch), -sinf(m_TotalPitch) * cosf(m_TotalYaw)},
			Vector3{-cosf(m_TotalPitch) * sinf(m_TotalYaw), sinf(m_TotalPitch), cosf(m_TotalPitch) * cosf(m_TotalYaw)},
			Vector3::Zero
		};

		m_Forward = pitchYawRotation.TransformVector(Vector3::UnitZ);
		m_Right = Vector3::Cross(Vector3::UnitY, m_Forward).Normalized();
		m_Up = Vector3::Cross(m_Forward, m_Right).Normalized();

		InputVector = pitchYawRotation.TransformVector(InputVector);

		m_Origin += InputVector * deltaTime * m_MoveSpeedKey;

		//Update Matrices
		CalculateViewMatrix();
		CalculateProjectionMatrix();
		CombineViewProjectionMatrix();
	}


}
