#ifndef CAR_MAIN_CAR_H
#define CAR_MAIN_CAR_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Model.h"

struct CarCameraPose {
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;
};

struct CarBoundingBox {
    glm::vec3 min;
    glm::vec3 max;
};

struct CarDoor {
    std::vector<std::string> meshNames;
    glm::vec3 hingeLocal = glm::vec3(0.0f);
    float openAngle = glm::radians(70.0f);
    float currentAngle = 0.0f;
    bool isOpen = false;
};

struct CarWheel {
    std::vector<std::string> meshNames;
    glm::vec3 pivotLocal = glm::vec3(0.0f);
    float rotation = 0.0f;
    float steerAngle = 0.0f;
    bool steerable = false;
};

class CarAnimator {
public:
    void updateDoor(CarDoor& door, float dt);
    void updateWheels(std::vector<CarWheel>& wheels, float speed, float steerAngle, float dt);
};

class Car {
public:
    explicit Car(const std::string& assetPath);

    void update(float dt);
    void draw(Shader& shader);
    void handleInput(GLFWwindow* window, float dt);
    void setTransform(const glm::vec3& pos, float yawRadians, const glm::vec3& scale);
    void setCameraPosition(const glm::vec3& cameraPosition);
    void autoScaleToFit(float targetLength);
    void alignToGround(float groundY = 0.0f);

    void setDriverSeatOffset(const glm::vec3& offset) { driverSeatOffset_ = offset; }
    void setDoorMeshNames(const std::vector<std::string>& meshNames);
    void setDoorHingeLocal(const glm::vec3& hingeLocal);
    void setWheelMeshNames(const std::vector<std::string>& meshNames, bool steerable, const glm::vec3& pivotLocal);
    void setWorldBounds(const glm::vec3& minBounds, const glm::vec3& maxBounds);
    bool isDriverDoorOpen() const { return driverDoor_.isOpen; }

    CarCameraPose getDriverSeatCameraPose() const;
    CarBoundingBox getCarBoundingBoxOrOBB() const;

private:
    glm::mat4 getModelMatrix() const;
    glm::mat4 getWheelTransform(const CarWheel& wheel, const CarMesh& mesh) const;
    glm::mat4 getDoorTransform(const CarDoor& door) const;

    CarModel model_;
    CarAnimator animator_;

    glm::vec3 position_ = glm::vec3(0.0f);
    float yaw_ = 0.0f;
    glm::vec3 scale_ = glm::vec3(1.0f);

    float speed_ = 0.0f;
    float steeringAngle_ = 0.0f;
    float maxSpeed_ = 40.0f;
    float acceleration_ = 12.0f;
    float brakeDecel_ = 20.0f;
    float steeringSpeed_ = glm::radians(60.0f);
    float maxSteerAngle_ = glm::radians(30.0f);
    float drag_ = 2.5f;

    glm::vec3 cameraPosition_ = glm::vec3(0.0f);
    glm::vec3 driverSeatOffset_ = glm::vec3(-0.6f, 3.2f, 0.8f);
    glm::vec3 localOffset_ = glm::vec3(0.0f);

    CarDoor driverDoor_;
    std::vector<CarWheel> wheels_;

    glm::vec3 worldMinBounds_ = glm::vec3(-1000.0f);
    glm::vec3 worldMaxBounds_ = glm::vec3(1000.0f);

    float autoScaleTarget_ = 0.0f;
};

#endif
