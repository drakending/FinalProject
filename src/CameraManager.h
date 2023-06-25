#pragma once
#include<memory>
#include<base/camera.h>
#include<base/input.h>
namespace Game {
	class CameraManager
	{
	public:
		Camera* camera;
		glm::vec3 rotateCenter;
		float rotateLength;
		float moveVel = 0.1;
		float rotateVel = 0.003;
		float scrollVel = 0.04;
		float rotateAngle = 0;
		void registerCamera(std::shared_ptr<Camera> camera) {
			this->camera = camera.get();
		}
		bool wanderMode;
		bool rotateMode;
		void handleInput(Input& input)
		{
			if (input.keyboard.keyStates[GLFW_KEY_W] != GLFW_RELEASE) {
				camera->transform.position += moveVel * camera->transform.getFront();
			}
			if (input.keyboard.keyStates[GLFW_KEY_A] != GLFW_RELEASE) {
				camera->transform.position -= moveVel * camera->transform.getRight();
			}
			if (input.keyboard.keyStates[GLFW_KEY_S] != GLFW_RELEASE) {
				camera->transform.position -= moveVel * camera->transform.getFront();
			}
			if (input.keyboard.keyStates[GLFW_KEY_D] != GLFW_RELEASE) {
				camera->transform.position += moveVel * camera->transform.getRight();
			}
			static auto forward_c_state = GLFW_RELEASE;
			if (input.keyboard.keyStates[GLFW_KEY_C] == GLFW_PRESS) {
				if (forward_c_state == GLFW_RELEASE)
					wanderMode = !wanderMode;
			}
			forward_c_state = input.keyboard.keyStates[GLFW_KEY_C];

			if (input.mouse.scroll.yOffset != 0)
			{
				PerspectiveCamera* cam = dynamic_cast<PerspectiveCamera*>(camera);
				if (cam->fovy + scrollVel * input.mouse.scroll.yOffset < 1.2 && cam->fovy + scrollVel * input.mouse.scroll.yOffset>0.2)
					cam->fovy += scrollVel * input.mouse.scroll.yOffset;
			}


			if (wanderMode)
			{
				if (input.mouse.move.xNow != input.mouse.move.xOld) {

					float mouse_movement_in_x_direction = input.mouse.move.xNow - input.mouse.move.xOld;
					float angle = -rotateVel * mouse_movement_in_x_direction;
					glm::quat q(cos(angle / 2), 0, sin(angle / 2), 0);
					camera->transform.rotation = q * camera->transform.rotation;
					// -----------------------------------------------------------------------------
				}
				if (input.mouse.move.yNow != input.mouse.move.yOld) {
					float mouse_movement_in_y_direction = input.mouse.move.yNow - input.mouse.move.yOld;
					float angle = -rotateVel * mouse_movement_in_y_direction;
					glm::quat q(cos(angle / 2), sin(angle / 2), 0, 0);
					camera->transform.rotation = camera->transform.rotation * q;
				}
			}
			if (rotateMode)
			{
				camera->transform.position = glm::vec3(rotateCenter.x + rotateLength * sin(glm::radians(rotateAngle)),
					rotateCenter.y,
					rotateCenter.z + rotateLength * cos(glm::radians(rotateAngle)));
				camera->transform.lookAt(rotateCenter);
				rotateAngle += 1;
			}
			input.forwardState();
		}
	};
}

