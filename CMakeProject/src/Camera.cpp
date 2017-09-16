#include "Camera.h"
Camera::Camera() {
	cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	//cameraDirection = glm::normalize(cameraPos - cameraTarget);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	viewmat = glm::mat4(
		glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, -3.0f, 1.0f)
	);
}

Camera::~Camera() {

}

glm::mat4 Camera::updateViewmat() {
	viewmat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	return viewmat;
}

void Camera::move(int moveDirection, GLdouble dtime) {
	switch (moveDirection)
	{
	case 0:
		cameraPos += (static_cast<float>(dtime) * 2) * cameraFront;
		break;
	case 1:
		cameraPos -= (static_cast<float>(dtime) * 2) * cameraFront;
		break;
	case 2:
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * (static_cast<float>(dtime) * 2);
		break;
	case 3:
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * (static_cast<float>(dtime) * 2);
		break;
	default:
		break;
	}
}

void Camera::processMouseMovement(float mouseXOffset, float mouseYOffset) {
	mouseXOffset *= sensitivity;
	mouseYOffset *= sensitivity;

	yaw += mouseXOffset;
	pitch += mouseYOffset;

	//Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}
	updateCameraVectors();
}

void Camera::updateCameraVectors() {
	//Calculate the new cameraFront vector
	glm::vec3 newCameraFront;
	newCameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newCameraFront.y = sin(glm::radians(pitch));
	newCameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(newCameraFront);
	//std::cout << "(" << newCameraFront.x << ", " << newCameraFront.y << ", " << newCameraFront.z << ")";
	//Also re-calculate the cameraRight and cameraUp vector
	cameraRight = glm::normalize(glm::cross(cameraFront, worldUp)); //normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movements
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}