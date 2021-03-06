#include "PlayerCamera.hpp"
#include <iostream>
#include <cstdlib>

extern GLFWwindow* window;

PlayerCamera::PlayerCamera() {
}

PlayerCamera::~PlayerCamera() {

}

PlayerCamera::PlayerCamera(glhf::Program prog, Player *player) {
	_prog = prog;
	_player = player;
	_projection = glm::perspective(45.0f, 1024.0f/800.0f, 0.1f, 100.0f);
	_cameraID = glGetUniformLocation(_prog.getId(), "viewProjection");

	_offsetCameraBoost.y = -player->getRadius()*1.55f;
	_offsetCameraBoost.z = player->getRadius()*5.0f;
}

void PlayerCamera::update(double dt) {
	glm::vec3 posPlayer = _player->getPos();
	glm::vec3 posPlayerCamera = posPlayer;

	_offsetCameraNormal.y = -_player->getRadius()*4 + std::min(0.5f * _player->getSpeed(), _player->getRadius()*1.0f);
	_offsetCameraNormal.z = -0.3 + std::min(1.75f * _player->getSpeed(), _player->getRadius()*7.f);

	glm::vec3 direction(0, 0, 8);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ) {
		_timeMove += dt;
		if (_timeMove > DURATION_BOOST_ANIM)
			_timeMove = DURATION_BOOST_ANIM;

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			direction *= -1;
			_offsetCameraNormal.z = 1.25 + std::min(1.75f * _player->getSpeed(), _player->getRadius()*7.f);
			_offsetCameraBoost.z = _player->getRadius()*11.0f;
		}

		move(dt);

		posPlayerCamera = glm::vec3(posPlayerCamera.x + ((std::rand() % 3 + 1 - 3/2.f)/800.f), posPlayerCamera.y + ((std::rand() % 3 + 1 - 3/2.f)/800.f), posPlayerCamera.z);
	} else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		direction *= -1;
		_offsetCameraNormal.z = 1.25 + std::min(1.75f * _player->getSpeed(), _player->getRadius()*7.f);
		move(dt);
	} else {
		_timeMove -= dt;
		if (_timeMove < 0)
			_timeMove = 0;
		move(dt);
	}

	posPlayerCamera += _offsetCamera;

	glm::mat4 view = glm::lookAt(
		posPlayerCamera,
		posPlayer + direction,
		glm::normalize(glm::vec3(-posPlayer.x,-posPlayer.y,0)));

	glm::mat4 camera = _projection * view;
	glUniformMatrix4fv(_cameraID, 1, GL_FALSE, &camera[0][0]);
}

void PlayerCamera::move(double dt) {
	for (int i = 1; i < 3; ++i) {
		float c = _offsetCameraBoost[i] - _offsetCameraNormal[i];
		float b = _offsetCameraNormal[i];
		float d = DURATION_BOOST_ANIM;
		float t = _timeMove / (d / 2);

		t = _timeMove/d;
		t -= 1;
		_offsetCamera[i] = c*(t*t*t+1)+b;
	}
	_offsetCamera[0] = cos(_player->getAngle()) * _offsetCamera[1];
	_offsetCamera[1] = sin(_player->getAngle()) * _offsetCamera[1];
}
