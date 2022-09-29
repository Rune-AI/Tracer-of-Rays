#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};

		Vector3 forward{Vector3::UnitZ};
		//Vector3 forward = Vector3(0.266f, -0.453f, 0.860f);
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};

		bool updateOBN{ true };


		Matrix CalculateCameraToWorld()
		{
			if (!updateOBN)
			{
				return cameraToWorld;
			}

			//todo: W2
			/*assert(false && "Not Implemented Yet");
			return {};*/

			//No Roll
			Matrix roatationMatrix = Matrix::CreateRotationX(totalPitch * TO_RADIANS) * Matrix::CreateRotationY(totalYaw * TO_RADIANS);

			forward = roatationMatrix.GetAxisZ();
			right = roatationMatrix.GetAxisX();
			up = roatationMatrix.GetAxisY();

			// This is for when we have a static forward
			/*Vector3 right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			Vector3 up = Vector3::Cross(forward, right).Normalized();*/

			//ONB
			cameraToWorld = Matrix(
				Vector4(right,0),
				Vector4(up,0),
				Vector4(forward,0),
				Vector4(origin, 1)
			);

			updateOBN = false;
			return cameraToWorld;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			

			//todo: W2
			//assert(false && "Not Implemented Yet");

			const float movementSpeed{1.f};
			if (pKeyboardState[SDL_SCANCODE_LEFT] == true)
			{
				fovAngle -= movementSpeed * deltaTime;
				fovAngle = fmax(1.f, fovAngle);
			}
			if (pKeyboardState[SDL_SCANCODE_RIGHT] == true)
			{
				fovAngle += movementSpeed * deltaTime;
				fovAngle = fmin(179.f, fovAngle);
			}

			if (pKeyboardState[SDL_SCANCODE_W])
			{
				origin += forward * movementSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin -= forward * movementSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += right * movementSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin -= right * movementSpeed * deltaTime;
			}


			if ((mouseState & SDL_BUTTON_LMASK) != 0)
			{
				if ((mouseState & SDL_BUTTON_RMASK) != 0)
				{
					// up and down
					origin.y -= mouseY / 50.f;
					origin.x += mouseX / 50.f;
				}
				else
				{
					//I don't really want naything here for just left clicking tbh
					//But this doesn't run automatically
				}
			}

			if ((mouseState & SDL_BUTTON_RMASK) != 0)
			{
				if (!(mouseState & SDL_BUTTON_LMASK) != 0)
				{
					updateOBN = true;
					//rotate yaw
					totalYaw += mouseX;
					//rotate pitch
					totalPitch -= mouseY;
				}
			}
		}
	};
}
