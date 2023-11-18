#include "camera.h"

Camera::Camera(glm::vec3 _position,
			   glm::vec3 _up, float _yaw, float _pitch) :
	position(_position), worldUp(_up), yaw(_yaw), pitch(_pitch),
	front(glm::vec3(0.0f, 0.0f, 1.0f)),
	movementSpeed(SPEED), mouseSensitivity(SENSITIVITY),
	zoom(ZOOM), ortho(ORTHO), near(NEAR), far(FAR),
	projectionType(PERSPECTIVE) {
	updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ,
			   float upX, float upY, float upZ,
			   float _yaw, float _pitch) :
	position(posX, posY, posZ), worldUp(upX, upY, upZ),
	front(glm::vec3(0.0f, 0.0f, 1.0f)),
	movementSpeed(SPEED), mouseSensitivity(SENSITIVITY),
	zoom(ZOOM), ortho(ORTHO), near(NEAR), far(FAR),
	projectionType(PERSPECTIVE) {
	updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float viewWidth, float viewHeight) {
	if (projectionType == PERSPECTIVE)	// 投影
		return glm::perspective(glm::radians(zoom), viewWidth / viewHeight,
								near, far);
	else if (projectionType == ORTHOGONAL) {	// 正交
		float scale = 1.0f / ortho;
		float right = viewWidth / viewHeight * scale;
		float top = viewHeight / viewWidth * scale;
		return glm::ortho(-right, right, -top, top, near, far);
	}

	return glm::mat4();
}

void Camera::reset() {
	position = POSITION;
	up = UP_DIR;
	yaw = YAW;
	pitch = PITCH;
	movementSpeed = SPEED;
	mouseSensitivity = SENSITIVITY;
	zoom = ZOOM;
	ortho = ORTHO;
	near = NEAR;
	far = FAR;
	updateCameraVectors();
}

void Camera::processKeyboard(CameraMovement direction,
							 float deltaTime) {
	float velocity = movementSpeed * deltaTime;
	if (direction == FORWARD)
		position += front * velocity;
	if (direction == BACKWARD)
		position -= front * velocity;
	if (direction == LEFT)
		position -= right * velocity;
	if (direction == RIGHT)
		position += right * velocity;
	if (direction == UP)
		position += up * velocity;
	if (direction == DOWN)
		position -= up * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset,
								  bool constrainPitch) {
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;
	yaw += xoffset;
	pitch += yoffset;

	// 限制视角
	if (constrainPitch) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
	zoom -= (float)yoffset;
	if (zoom < ZOOM_MIN)
		zoom = ZOOM_MIN;
	if (zoom > ZOOM_MAX)
		zoom = ZOOM_MAX;
}

void Camera::processRotationUpdate() {
	updateCameraVectors();
}

void Camera::updateCameraVectors() {
	glm::vec3 _front;
	_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	_front.y = sin(glm::radians(pitch));
	_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(_front);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}
