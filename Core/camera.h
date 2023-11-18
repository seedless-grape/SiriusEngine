/* 镜头 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include <vector>

// 镜头移动类型枚举体
enum CameraMovement {
	FORWARD, BACKWARD,
	LEFT, RIGHT,
	UP, DOWN
};

// 镜头投影类型枚举体
enum CameraProjection {
	PERSPECTIVE,	// 透视投影
	ORTHOGONAL		// 正交投影
};

// 镜头初始值
const glm::vec3 POSITION(0.0f, 0.0f, 3.0f);	// 位置坐标
const glm::vec3 UP_DIR(0.0f, 1.0f, 0.0f);	// 上朝向
const float		PITCH = 0.0f;				// 俯仰角	
const float		YAW = -90.0f;				// 偏航角
const float		SPEED = 2.5;				// 镜头移动速度
const float		SENSITIVITY = 0.06f;		// 鼠标灵敏度
const float		ZOOM = 45.0f;				// 视场角
const float		ZOOM_MIN = 1.0f;			// 最小视场角
const float		ZOOM_MAX = 90.0f;			// 最大视场角
const float		ORTHO = 0.5f;				// 正交范围
const float		ORTHO_MIN = 0.0f;			// 最大正交范围
const float		ORTHO_MAX = 1.0f;			// 最小正交范围
const float		NEAR = 0.1f;				// 近平面
const float		FAR = 100.0f;				// 远平面

// 镜头类
class Camera {
public:
	// 镜头属性
	glm::vec3 position; // 镜头位置
	glm::vec3 front;	// 镜头前朝向
	glm::vec3 up;		// 镜头上朝向
	glm::vec3 right;	// 镜头右朝向
	glm::vec3 worldUp;	// 镜头世界位置上朝向

	// 镜头角度
	float yaw;		// 俯仰角
	float pitch;	// 偏航角

	// 镜头选项
	float movementSpeed;	// 镜头移动速度
	float mouseSensitivity; // 鼠标灵敏度
	
	// 镜头视角
	float zoom;		// 镜头视场角
	float ortho;	// 正交范围
	float near;		// 近平面
	float far;		// 远平面

	// 镜头投影类型
	CameraProjection projectionType;

public:
	// 构造函数
	Camera(glm::vec3 _position = POSITION, glm::vec3 _up = UP_DIR,
		   float _yaw = YAW, float _pitch = PITCH);

	Camera(float posX, float posY, float posZ,
		   float upX, float upY, float upZ,
		   float _yaw = YAW, float _pitch = PITCH);

	// 返回镜头视图view矩阵
	// 获取视图矩阵
	glm::mat4 getViewMatrix();

	// 获取投影矩阵
	glm::mat4 getProjectionMatrix(float viewWidth, float viewHeight);

	// 重置初始镜头
	void reset();

	// 处理键盘键入信息
	void processKeyboard(CameraMovement direction, float deltaTime);

	// 处理鼠标移动信息
	void processMouseMovement(float xoffset, float yoffset,
							  bool constrainPitch = true);

	// 处理鼠标滚轮信息
	void processMouseScroll(float yoffset);

	// 处理镜头旋转更新
	void processRotationUpdate();

private:
	// 根据现有变量重新计算相机朝向
	void updateCameraVectors();
};

#endif // !CAMERA_H_
