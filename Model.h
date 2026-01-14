#ifndef CAR_MODEL_H
#define CAR_MODEL_H

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "Mesh.h"

class CarModel {
public:
    explicit CarModel(const std::string& path);

    const std::vector<CarMesh>& getMeshes() const { return meshes_; }
    glm::vec3 getAabbMin() const { return aabbMin_; }
    glm::vec3 getAabbMax() const { return aabbMax_; }

private:
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform);
    CarMesh processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& nodeTransform);
    std::vector<CarTexture> loadMaterialTextures(aiMaterial* material, aiTextureType type, const std::string& typeName,
        bool srgb);
    static bool isTransparentMaterial(const aiMaterial& material, float* outOpacity);
    static glm::mat4 convertMatrix(const aiMatrix4x4& from);

    unsigned int textureFromFile(const std::string& path, bool srgb);

    std::vector<CarMesh> meshes_;
    std::string directory_;
    std::vector<CarTexture> texturesLoaded_;
    glm::vec3 aabbMin_ = glm::vec3(0.0f);
    glm::vec3 aabbMax_ = glm::vec3(0.0f);
};

#endif