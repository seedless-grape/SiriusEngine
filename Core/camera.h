/* ��ͷ */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include <vector>

// ��ͷ�ƶ�����ö����
enum CameraMovement {
	FORWARD, BACKWARD,
	LEFT, RIGHT,
	UP, DOWN
};

// ��ͷͶӰ����ö����
enum CameraProjection {
	PERSPECTIVE,	// ͸��ͶӰ
	ORTHOGONAL		// ����ͶӰ
};

// ��ͷ��ʼֵ
const glm::vec3 POSITION(0.0f, 0.0f, 3.0f);	// λ������
const glm::vec3 UP_DIR(0.0f, 1.0f, 0.0f);	// �ϳ���
const float		PITCH = 0.0f;				// ������	
const float		YAW = -90.0f;				// ƫ����
const float		SPEED = 2.5;				// ��ͷ�ƶ��ٶ�
const float		SENSITIVITY = 0.06f;		// ���������
const float		ZOOM = 45.0f;				// �ӳ���
const float		ZOOM_MIN = 1.0f;			// ��С�ӳ���
const float		ZOOM_MAX = 90.0f;			// ����ӳ���
const float		ORTHO = 0.5f;				// ������Χ
const float		ORTHO_MIN = 0.0f;			// ���������Χ
const float		ORTHO_MAX = 1.0f;			// ��С������Χ
const float		NEAR = 0.1f;				// ��ƽ��
const float		FAR = 100.0f;				// Զƽ��

// ��ͷ��
class Camera {
public:
	// ��ͷ����
	glm::vec3 position; // ��ͷλ��
	glm::vec3 front;	// ��ͷǰ����
	glm::vec3 up;		// ��ͷ�ϳ���
	glm::vec3 right;	// ��ͷ�ҳ���
	glm::vec3 worldUp;	// ��ͷ����λ���ϳ���

	// ��ͷ�Ƕ�
	float yaw;		// ������
	float pitch;	// ƫ����

	// ��ͷѡ��
	float movementSpeed;	// ��ͷ�ƶ��ٶ�
	float mouseSensitivity; // ���������
	
	// ��ͷ�ӽ�
	float zoom;		// ��ͷ�ӳ���
	float ortho;	// ������Χ
	float near;		// ��ƽ��
	float far;		// Զƽ��

	// ��ͷͶӰ����
	CameraProjection projectionType;

public:
	// ���캯��
	Camera(glm::vec3 _position = POSITION, glm::vec3 _up = UP_DIR,
		   float _yaw = YAW, float _pitch = PITCH);

	Camera(float posX, float posY, float posZ,
		   float upX, float upY, float upZ,
		   float _yaw = YAW, float _pitch = PITCH);

	// ���ؾ�ͷ��ͼview����
	// ��ȡ��ͼ����
	glm::mat4 getViewMatrix();

	// ��ȡͶӰ����
	glm::mat4 getProjectionMatrix(float viewWidth, float viewHeight);

	// ���ó�ʼ��ͷ
	void reset();

	// ������̼�����Ϣ
	void processKeyboard(CameraMovement direction, float deltaTime);

	// ��������ƶ���Ϣ
	void processMouseMovement(float xoffset, float yoffset,
							  bool constrainPitch = true);

	// ������������Ϣ
	void processMouseScroll(float yoffset);

	// ����ͷ��ת����
	void processRotationUpdate();

private:
	// �������б������¼����������
	void updateCameraVectors();
};

#endif // !CAMERA_H_
