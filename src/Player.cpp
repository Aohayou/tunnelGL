#include "Player.hpp"
#include <iostream>
#include <GLFW/glfw3.h>
#include <sstream>
#include "PrintText.hpp"
#include "TunnelSection.hpp"

extern GLFWwindow* window;

Player::Player() {

}

Player::Player(glhf::Program prog) {
	//build the player object with prog
	_pos = glm::vec3(0, 0, 0);
	_speed = glm::vec3(0, 0, 0.07);
	_obj.setProgram(prog);
	_obj.load("vaisseau.obj");
	_obj.initVao();
	_obj.setTexture("vaisseau.tga", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 10.0);
	_obj.scale(_radius * 2 / 0.65);
}

Player::~Player() {
	_obj.draw();
}

void Player::reset(){
	_pos = glm::vec3(0, 0, 0);
	_speed = glm::vec3(0, 0, 0.07);
	_score = 0;
}

void Player::update(double dt, Tunnel *tunnel) {
	//Speed without friction on Z, but not on x and y
	float tiltDiff = 0;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ) { //left
		_angleSpeed += dt/8 * _speed.z *1000/80;
		tiltDiff += M_PI / 256;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ) { //right	
		_angleSpeed -= dt/8 * _speed.z *1000/80;
		tiltDiff -= M_PI / 256;
	}
	float oldTilt = _tilt;
	_tilt += tiltDiff;
	if (_tilt > M_PI / 32) _tilt = M_PI / 32;
	if (_tilt < -M_PI / 32) _tilt = -M_PI / 32;
	_tilt *= FRICTION;
	
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ) { //boost
		_speed.z += dt/400.0;
		_score += dt * _speed.z * 2000;
	} else {
		_score += dt * _speed.z * 1000;
	}

	_pos.z += _speed.z;

	if (tunnel->isHole(_angle, _pos.z) == SAFE && tunnel->isHole(_angle + _angleSpeed * 3, _pos.z) == OBSTACLE) {
		_angleSpeed = 0;
	}

	_angle += _angleSpeed;

	_pos.x = cos(_angle) * (tunnel->getRadius() - _radius);
	_pos.y = sin(_angle) * (tunnel->getRadius() - _radius);
	
	_obj.rotate(_angleSpeed + (oldTilt - _tilt), 0, 0, 1);
	_obj.setTranslation(_pos.x, _pos.y, _pos.z);

	_angleSpeed *= FRICTION;
}

void Player::draw() {
	_obj.draw();
}

void Player::drawText() {
	std::ostringstream strs;
	strs << std::floor(_score);
	printText2D(strs.str().c_str(), 400, 30, 50);

	std::ostringstream strs2;
	strs2 << std::floor(getSpeed() * 1000) << "km/h";
	printText2D(strs2.str().c_str(), 30, 750, 40);
}

glm::vec3 Player::getPos() {
	return _pos;
}

float Player::getRadius() {
	return _radius;
}

float Player::getSpeed() {
	return _speed.z;
}

float Player::getAngle() {
	return _angle;
}

int Player::getScore() {
	return (int) _score;
}