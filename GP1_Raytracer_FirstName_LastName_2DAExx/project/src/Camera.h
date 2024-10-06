#pragma once
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Maths.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle) :
			origin{ _origin },
			fovAngle{ _fovAngle }
		{
		}


		Vector3 origin{};
		float fovAngle{ 90.f };

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{ 0.f };
		float totalYaw{ 0.f };

		const float MoveSpeedKey{5.f};
		const float MoveSpeedMouse{ 0.01f };
		const float RotatedSpeed{ 0.01f };

		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			Vector3 right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			Vector3 up = Vector3::Cross(forward, right).Normalized();

			Matrix result{
				right,
				up,
				forward,
				origin
			};

			return result;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			Vector3 InputVector{};

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			const bool MouseRightPressed{ (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0 };
			const bool MouseLeftPressed{ (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0 };

			if (MouseRightPressed && MouseLeftPressed)
			{
				InputVector.y -= mouseY * MoveSpeedMouse;
			}
			else if (!MouseRightPressed && MouseLeftPressed)
			{
				InputVector.z -= mouseY * MoveSpeedMouse;
				totalYaw -= mouseX * RotatedSpeed;
			}
			else if (MouseRightPressed && !MouseLeftPressed)
			{
				totalPitch -= mouseY * RotatedSpeed;
				totalYaw -= mouseX * RotatedSpeed;
			}

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);


			if (pKeyboardState[SDL_SCANCODE_S])
				InputVector.z -= 1;

			if (pKeyboardState[SDL_SCANCODE_W])
				InputVector.z += 1;

			if (pKeyboardState[SDL_SCANCODE_A])
				InputVector.x -= 1;

			if (pKeyboardState[SDL_SCANCODE_D])
				InputVector.x += 1;


			/*const Matrix yawRotation{ Matrix::CreateRotationY(totalYaw) };
			const Matrix pitchRotation{ Matrix::CreateRotationX(totalPitch) };


			const Matrix pitchYawRotation{pitchRotation * yawRotation};*/


			const Matrix pitchYawRotation
			{
				Vector3{cosf(totalYaw), 0, sinf(totalYaw)},
				Vector3{sinf(totalYaw) * sinf(totalPitch), cosf(totalPitch), -sinf(totalPitch) * cosf(totalYaw)},
				Vector3{-cosf(totalPitch) * sinf(totalYaw), sinf(totalPitch), cosf(totalPitch) * cosf(totalYaw)},
				Vector3::Zero
			};

			forward = pitchYawRotation.TransformVector(Vector3::UnitZ);

			InputVector = pitchYawRotation.TransformVector(InputVector);

			origin += InputVector * deltaTime * MoveSpeedKey;
		}
	};
}
