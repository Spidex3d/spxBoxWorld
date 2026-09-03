#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera
{
public:

    Camera(
        glm::vec3 position = glm::vec3(0.0f, 2.0f, 5.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f
    );

    glm::mat4 GetViewMatrix() const;

    glm::mat4 GetProjectionMatrix(float aspectRatio) const;

    void ProcessKeyboard(
        Camera_Movement direction,
        float deltaTime
    );

    void ProcessMouseMovement(
        float xOffset,
        float yOffset,
        bool constrainPitch = true
    );

    void ProcessMouseScroll(float yOffset);

    glm::vec3 GetPosition() const
    {
        return Position;
    }

    glm::vec3 GetFront() const
    {
        return Front;
    }

private:

    void UpdateCameraVectors();

private:

    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed = 5.0f;
    float MouseSensitivity = 0.1f;
    float Zoom = 45.0f;

    float NearPlane = 0.1f;
    float FarPlane = 1000.0f;
};

