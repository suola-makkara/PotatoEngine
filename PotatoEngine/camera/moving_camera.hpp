#pragma once

#include "camera.hpp"
#include "event.hpp"

class MovingCamera : public Camera
{
public:
	MovingCamera(const glm::vec3& pos = glm::vec3(0), float aspect = 1.0f);

	~MovingCamera() = default;

	glm::vec3 getDirection() const override;
	
	glm::vec3 getPosition() const override;

	glm::mat4 getViewMat() const override;

	glm::mat4 getProjMat() const override;

	Ray castRay(const glm::vec2& ndc) const override;

	void update(float dt) override;

	void handleEvent(const Event & e) override;

	float getAspect() const;

	float near = 0.1f;
	float far = 100.0f;
	float fov = 45.0f;

	float mouseSensitiviy = 0.01f;
	float movementSpeed = 0.5f;
private:
	float aspect;

	glm::vec3 pos;

	glm::ivec3 movement = glm::ivec3(0);

	float pitch = 0.0f;
	float yaw = 0.0f;

	bool mouseDown = false;
	glm::vec2 prevMousePos{};

	static const glm::vec3 up;
};