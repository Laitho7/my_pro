#include "Mesh.h"

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

namespace {
    constexpr const char* kUniformBaseColor = "material.baseColorFactor";
    constexpr const char* kUniformEmissive = "material.emissiveFactor";
    constexpr const char* kUniformMetallic = "material.metallicFactor";
    constexpr const char* kUniformRoughness = "material.roughnessFactor";
    constexpr const char* kUniformOpacity = "material.opacity";

    struct TextureBinding {
        std::string type;
        std::string samplerUniform;
        std::string flagUniform;
    };

    const std::vector<TextureBinding> kTextureBindings = {
        {"texture_albedo", "material.baseColorMap", "material.hasBaseColorMap"},
        {"texture_normal", "material.normalMap", "material.hasNormalMap"},
        {"texture_metallic_roughness", "material.metallicRoughnessMap", "material.hasMetallicRoughnessMap"},
        {"texture_ao", "material.aoMap", "material.hasAoMap"},
        {"texture_emissive", "material.emissiveMap", "material.hasEmissiveMap"},
        {"texture_specular", "material.specularMap", "material.hasSpecularMap"},
        {"texture_glossiness", "material.glossinessMap", "material.hasGlossinessMap"}
    };
}

CarMesh::CarMesh(std::string name,
    const std::vector<CarVertex>& vertices,
    const std::vector<unsigned int>& indices,
    const std::vector<CarTexture>& textures,
    const CarMaterial& material,
    const glm::mat4& localTransform,
    bool transparent,
    float opacity)
    : name_(std::move(name)),
    vertices_(vertices),
    indices_(indices),
    textures_(textures),
    material_(material),
    localTransform_(localTransform),
    transparent_(transparent),
    opacity_(opacity) {
    if (!vertices_.empty()) {
        aabbMin_ = vertices_[0].position;
        aabbMax_ = vertices_[0].position;
        for (const auto& vertex : vertices_) {
            aabbMin_ = glm::min(aabbMin_, vertex.position);
            aabbMax_ = glm::max(aabbMax_, vertex.position);
        }
    }
    else {
        aabbMin_ = glm::vec3(0.0f);
        aabbMax_ = glm::vec3(0.0f);
    }
    setupMesh();
}

void CarMesh::draw(Shader& shader) const {
    shader.setVec4(kUniformBaseColor, material_.baseColorFactor);
    shader.setVec3(kUniformEmissive, material_.emissiveFactor);
    shader.setFloat(kUniformMetallic, material_.metallicFactor);
    shader.setFloat(kUniformRoughness, material_.roughnessFactor);
    shader.setFloat(kUniformOpacity, opacity_);

    unsigned int textureUnit = 0;
    for (const auto& binding : kTextureBindings) {
        auto it = std::find_if(textures_.begin(), textures_.end(), [&](const CarTexture& texture) {
            return texture.type == binding.type;
            });
        if (it != textures_.end()) {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, it->id);
            shader.setInt(binding.samplerUniform, static_cast<int>(textureUnit));
            shader.setInt(binding.flagUniform, 1);
            textureUnit++;
        }
        else {
            shader.setInt(binding.flagUniform, 0);
        }
    }

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices_.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void CarMesh::setupMesh() {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(CarVertex), vertices_.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CarVertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CarVertex), (void*)offsetof(CarVertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CarVertex), (void*)offsetof(CarVertex, texCoords));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(CarVertex), (void*)offsetof(CarVertex, tangent));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(CarVertex), (void*)offsetof(CarVertex, bitangent));

    glBindVertexArray(0);
}