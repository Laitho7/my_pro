#ifndef CAR_MESH_H
#define CAR_MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Shader.h"

struct CarTexture {
    unsigned int id = 0;
    std::string type;
    std::string path;
    bool srgb = false;
};

struct CarMaterial {
    glm::vec4 baseColorFactor = glm::vec4(1.0f);
    glm::vec3 emissiveFactor = glm::vec3(0.0f);
    float metallicFactor = 0.0f;
    float roughnessFactor = 1.0f;
    float alphaCutoff = 0.5f;
};

struct CarVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

class CarMesh {
public:
    CarMesh(std::string name,
        const std::vector<CarVertex>& vertices,
        const std::vector<unsigned int>& indices,
        const std::vector<CarTexture>& textures,
        const CarMaterial& material,
        const glm::mat4& localTransform,
        bool transparent,
        float opacity);

    void draw(Shader& shader) const;

    const std::string& getName() const { return name_; }
    const glm::mat4& getLocalTransform() const { return localTransform_; }
    const glm::vec3& getAabbMin() const { return aabbMin_; }
    const glm::vec3& getAabbMax() const { return aabbMax_; }
    bool isTransparent() const { return transparent_; }
    float getOpacity() const { return opacity_; }

private:
    void setupMesh();

    std::string name_;
    std::vector<CarVertex> vertices_;
    std::vector<unsigned int> indices_;
    std::vector<CarTexture> textures_;
    CarMaterial material_;
    glm::mat4 localTransform_;
    bool transparent_;
    float opacity_;
    glm::vec3 aabbMin_;
    glm::vec3 aabbMax_;

    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;
    unsigned int ebo_ = 0;
};

#endif