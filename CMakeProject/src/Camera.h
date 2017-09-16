#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <libheaders.h>
#include <iostream>

class Camera
{
public: 
	Camera();
	virtual ~Camera();

	glm::mat4 updateViewmat();
	void move(int moveDirection, GLdouble dtime);
	void processMouseMovement(float mouseXOffset, float mouseYOffset);
	void updateCameraVectors();

private:
	glm::vec3 cameraPos;		//The vector in world space that points to the camera's position
	glm::vec3 cameraTarget; //The vector that points to the camera's target
	glm::vec3 cameraDirection;  //Represents the positive z-axis of camera space (NOT SURE. NEED TO CHECK)
	glm::vec3 worldUp; //A vector that points upwards in world space 
	glm::vec3 cameraRight; //Represents the positive x-axis of camera space
	glm::vec3 cameraUp; //Represents the positive y-axis of camera space
	glm::vec3 cameraFront;

	float pitch = 0.0f;
	float yaw = -90.0f;
	float speed = 2.5f;
	float sensitivity = 0.1f;
	float zoom = 45.0f; //YET TO BE IMPLEMENTED
	bool constrainPitch = true; //whether the Pitch should be constrained or not.

	glm::mat4 viewmat;
	
	


};

#endif