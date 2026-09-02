#include "Scene/camera.h"
// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------
Camera::Camera(
    glm::vec3 position,
    glm::vec3 up,
    float yaw,
    float pitch)
    :
    Position(position),
    Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    WorldUp(up),
    Yaw(yaw),
    Pitch(pitch)
{
    UpdateCameraVectors();
}


// ------------------------------------------------------------
// View Matrix
// ------------------------------------------------------------
glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(
        Position,
        Position + Front,
        Up
    );
}


// ------------------------------------------------------------
// Projection Matrix
// ------------------------------------------------------------
glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(
        glm::radians(Zoom),
        aspectRatio,
        NearPlane,
        FarPlane
    );
}


// ------------------------------------------------------------
// Keyboard Movement
// ------------------------------------------------------------
void Camera::ProcessKeyboard(
    Camera_Movement direction,
    float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;

    if (direction == FORWARD)
    {
        Position += Front * velocity;
    }

    if (direction == BACKWARD)
    {
        Position -= Front * velocity;
    }

    if (direction == LEFT)
    {
        Position -= Right * velocity;
    }

    if (direction == RIGHT)
    {
        Position += Right * velocity;
    }

    // Use WorldUp here rather than camera Up.
    // This gives us true vertical movement in the world.
    if (direction == UP)
    {
        Position += WorldUp * velocity;
    }

    if (direction == DOWN)
    {
        Position -= WorldUp * velocity;
    }
}


// ------------------------------------------------------------
// Mouse Look
// ------------------------------------------------------------
void Camera::ProcessMouseMovement(
    float xOffset,
    float yOffset,
    bool constrainPitch)
{
    xOffset *= MouseSensitivity;
    yOffset *= MouseSensitivity;

    Yaw += xOffset;
    Pitch += yOffset;

    // Stop the camera flipping upside down
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
        {
            Pitch = 89.0f;
        }

        if (Pitch < -89.0f)
        {
            Pitch = -89.0f;
        }
    }

    UpdateCameraVectors();
}


// ------------------------------------------------------------
// Mouse Wheel / Field Of View
// ------------------------------------------------------------
void Camera::ProcessMouseScroll(float yOffset)
{
    Zoom -= yOffset;

    if (Zoom < 20.0f)
    {
        Zoom = 20.0f;
    }

    if (Zoom > 90.0f)
    {
        Zoom = 90.0f;
    }
}


// ------------------------------------------------------------
// Update Camera Direction Vectors
// ------------------------------------------------------------
void Camera::UpdateCameraVectors()
{
    glm::vec3 front;

    front.x =
        cos(glm::radians(Yaw)) *
        cos(glm::radians(Pitch));

    front.y =
        sin(glm::radians(Pitch));

    front.z =
        sin(glm::radians(Yaw)) *
        cos(glm::radians(Pitch));

    Front = glm::normalize(front);

    Right =
        glm::normalize(
            glm::cross(Front, WorldUp)
        );

    Up =
        glm::normalize(
            glm::cross(Right, Front)
        );
}