#include "Car.h"

#include <algorithm>
#include <cfloat>
#include <glm/gtc/matrix_transform.hpp>

namespace {
    float clamp(float value, float minValue, float maxValue) {
        return std::max(minValue, std::min(value, maxValue));
    }

    bool meshNameMatches(const std::string& name, const std::vector<std::string>& tags) {
        for (const auto& tag : tags) {
            if (name.find(tag) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
}

void CarAnimator::updateDoor(CarDoor& door, float dt) {
    float target = door.isOpen ? door.openAngle : 0.0f;
    float speed = glm::radians(90.0f);
    if (door.currentAngle < target) {
        door.currentAngle = std::min(door.currentAngle + speed * dt, target);
    }
    else if (door.currentAngle > target) {
        door.currentAngle = std::max(door.currentAngle - speed * dt, target);
    }
}

void CarAnimator::updateWheels(std::vector<CarWheel>& wheels, float speed, float steerAngle, float dt) {
    for (auto& wheel : wheels) {
        wheel.rotation += speed * dt * 0.5f;
        if (wheel.steerable) {
            wheel.steerAngle = steerAngle;
        }
        else {
            wheel.steerAngle = 0.0f;
        }
    }
}

Car::Car(const std::string& assetPath) : model_(assetPath) {
    setDoorMeshNames({ "door", "Door", "driver" });
}

void Car::update(float dt) {
    animator_.updateDoor(driverDoor_, dt);
    animator_.updateWheels(wheels_, speed_, steeringAngle_, dt);

    speed_ -= speed_ * drag_ * dt;
    glm::vec3 forward = glm::vec3(std::sin(yaw_), 0.0f, std::cos(yaw_));
    position_ += forward * speed_ * dt;

    position_.x = clamp(position_.x, worldMinBounds_.x, worldMaxBounds_.x);
    position_.y = clamp(position_.y, worldMinBounds_.y, worldMaxBounds_.y);
    position_.z = clamp(position_.z, worldMinBounds_.z, worldMaxBounds_.z);
}

void Car::draw(Shader& shader) {
    glm::mat4 modelMatrix = getModelMatrix();
    std::vector<const CarMesh*> opaque;
    std::vector<const CarMesh*> transparent;

    for (const auto& mesh : model_.getMeshes()) {
        if (mesh.isTransparent()) {
            transparent.push_back(&mesh);
        }
        else {
            opaque.push_back(&mesh);
        }
    }

    for (const auto* mesh : opaque) {
        glm::mat4 local = mesh->getLocalTransform();
        if (meshNameMatches(mesh->getName(), driverDoor_.meshNames)) {
            local *= getDoorTransform(driverDoor_);
        }
        for (const auto& wheel : wheels_) {
            if (meshNameMatches(mesh->getName(), wheel.meshNames)) {
                local *= getWheelTransform(wheel, *mesh);
            }
        }
        shader.setMat4("model", modelMatrix * local);
        mesh->draw(shader);
    }

    std::sort(transparent.begin(), transparent.end(), [&](const CarMesh* a, const CarMesh* b) {
        glm::vec3 centerA = (a->getAabbMin() + a->getAabbMax()) * 0.5f;
        glm::vec3 centerB = (b->getAabbMin() + b->getAabbMax()) * 0.5f;
        glm::vec3 worldA = glm::vec3(modelMatrix * glm::vec4(centerA, 1.0f));
        glm::vec3 worldB = glm::vec3(modelMatrix * glm::vec4(centerB, 1.0f));
        float distA = glm::length(worldA - cameraPosition_);
        float distB = glm::length(worldB - cameraPosition_);
        return distA > distB;
        });

    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    for (const auto* mesh : transparent) {
        glm::mat4 local = mesh->getLocalTransform();
        if (meshNameMatches(mesh->getName(), driverDoor_.meshNames)) {
            local *= getDoorTransform(driverDoor_);
        }
        for (const auto& wheel : wheels_) {
            if (meshNameMatches(mesh->getName(), wheel.meshNames)) {
                local *= getWheelTransform(wheel, *mesh);
            }
        }
        shader.setMat4("model", modelMatrix * local);
        mesh->draw(shader);
    }

    glDepthMask(GL_TRUE);
}

void Car::handleInput(GLFWwindow* window, float dt) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        speed_ = clamp(speed_ + acceleration_ * dt, -maxSpeed_, maxSpeed_);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        speed_ = clamp(speed_ - brakeDecel_ * dt, -maxSpeed_, maxSpeed_);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        steeringAngle_ = clamp(steeringAngle_ + steeringSpeed_ * dt, -maxSteerAngle_, maxSteerAngle_);
        yaw_ += steeringAngle_ * dt * (speed_ / maxSpeed_);
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        steeringAngle_ = clamp(steeringAngle_ - steeringSpeed_ * dt, -maxSteerAngle_, maxSteerAngle_);
        yaw_ += steeringAngle_ * dt * (speed_ / maxSpeed_);
    }
    else {
        steeringAngle_ *= 0.9f;
    }

    static bool lastToggle = false;
    bool toggle = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
    if (toggle && !lastToggle) {
        driverDoor_.isOpen = !driverDoor_.isOpen;
    }
    lastToggle = toggle;
}

void Car::setTransform(const glm::vec3& pos, float yawRadians, const glm::vec3& scale) {
    position_ = pos;
    yaw_ = yawRadians;
    scale_ = scale;
}

void Car::setCameraPosition(const glm::vec3& cameraPosition) {
    cameraPosition_ = cameraPosition;
}

void Car::autoScaleToFit(float targetLength) {
    if (targetLength <= 0.0f) {
        return;
    }
    glm::vec3 min = model_.getAabbMin();
    glm::vec3 max = model_.getAabbMax();
    glm::vec3 size = max - min;
    float maxAxis = std::max(size.x, std::max(size.y, size.z));
    if (maxAxis > 0.0f) {
        float scale = targetLength / maxAxis;
        scale_ = glm::vec3(scale);
    }
    autoScaleTarget_ = targetLength;
}

void Car::alignToGround(float groundY) {
    glm::vec3 min = model_.getAabbMin();
    localOffset_.y = groundY - min.y;
}

void Car::setDoorMeshNames(const std::vector<std::string>& meshNames) {
    driverDoor_.meshNames = meshNames;
}

void Car::setDoorHingeLocal(const glm::vec3& hingeLocal) {
    driverDoor_.hingeLocal = hingeLocal;
}

void Car::setWheelMeshNames(const std::vector<std::string>& meshNames, bool steerable, const glm::vec3& pivotLocal) {
    wheels_.push_back(CarWheel{ meshNames, pivotLocal, 0.0f, 0.0f, steerable });
}

void Car::setWorldBounds(const glm::vec3& minBounds, const glm::vec3& maxBounds) {
    worldMinBounds_ = minBounds;
    worldMaxBounds_ = maxBounds;
}

CarCameraPose Car::getDriverSeatCameraPose() const {
    glm::mat4 modelMatrix = getModelMatrix();
    glm::vec3 worldPos = glm::vec3(modelMatrix * glm::vec4(driverSeatOffset_, 1.0f));
    glm::vec3 forward = glm::normalize(glm::vec3(std::sin(yaw_), 0.0f, std::cos(yaw_)));
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    return { worldPos, forward, up };
}

CarBoundingBox Car::getCarBoundingBoxOrOBB() const {
    glm::vec3 min = model_.getAabbMin();
    glm::vec3 max = model_.getAabbMax();

    glm::vec3 corners[8] = {
        {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {min.x, max.y, min.z},
        {max.x, max.y, min.z},
        {min.x, min.y, max.z},
        {max.x, min.y, max.z},
        {min.x, max.y, max.z},
        {max.x, max.y, max.z}
    };

    glm::mat4 modelMatrix = getModelMatrix();
    glm::vec3 worldMin(FLT_MAX);
    glm::vec3 worldMax(-FLT_MAX);

    for (const auto& corner : corners) {
        glm::vec3 world = glm::vec3(modelMatrix * glm::vec4(corner, 1.0f));
        worldMin = glm::min(worldMin, world);
        worldMax = glm::max(worldMax, world);
    }

    return { worldMin, worldMax };
}

glm::mat4 Car::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position_);
    model = glm::rotate(model, yaw_, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, scale_);
    model = glm::translate(model, localOffset_);
    return model;
}

glm::mat4 Car::getWheelTransform(const CarWheel& wheel, const CarMesh& mesh) const {
    glm::vec3 pivot = wheel.pivotLocal;
    if (pivot == glm::vec3(0.0f)) {
        pivot = (mesh.getAabbMin() + mesh.getAabbMax()) * 0.5f;
    }

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, pivot);
    transform = glm::rotate(transform, wheel.steerAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, wheel.rotation, glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::translate(transform, -pivot);
    return transform;
}

glm::mat4 Car::getDoorTransform(const CarDoor& door) const {
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, door.hingeLocal);
    transform = glm::rotate(transform, door.currentAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::translate(transform, -door.hingeLocal);
    return transform;
}