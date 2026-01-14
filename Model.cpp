#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <cctype>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace {
    bool nameMatchesGlass(const std::string& name) {
        std::string lower;
        lower.reserve(name.size());
        for (char c : name) {
            lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }
        return lower.find("glass") != std::string::npos ||
            lower.find("window") != std::string::npos ||
            lower.find("mirror") != std::string::npos;
    }
}

CarModel::CarModel(const std::string& path) {
    loadModel(path);
}

void CarModel::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory_ = path.substr(0, path.find_last_of('/'));
    meshes_.clear();
    processNode(scene->mRootNode, scene, glm::mat4(1.0f));

    if (!meshes_.empty()) {
        aabbMin_ = meshes_[0].getAabbMin();
        aabbMax_ = meshes_[0].getAabbMax();
        for (const auto& mesh : meshes_) {
            aabbMin_ = glm::min(aabbMin_, mesh.getAabbMin());
            aabbMax_ = glm::max(aabbMax_, mesh.getAabbMax());
        }
    }
}

void CarModel::processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform) {
    glm::mat4 nodeTransform = parentTransform * convertMatrix(node->mTransformation);

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes_.push_back(processMesh(mesh, scene, nodeTransform));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, nodeTransform);
    }
}

CarMesh CarModel::processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& nodeTransform) {
    std::vector<CarVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<CarTexture> textures;

    vertices.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        CarVertex vertex{};
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->HasNormals()) {
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        if (mesh->mTextureCoords[0]) {
            vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            vertex.texCoords = glm::vec2(0.0f);
        }
        if (mesh->HasTangentsAndBitangents()) {
            vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            vertex.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        }
        vertices.push_back(vertex);
    }

    indices.reserve(mesh->mNumFaces * 3);
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    CarMaterial material{};
    bool transparent = false;
    float opacity = 1.0f;
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
        aiColor4D baseColor;
        if (aiGetMaterialColor(mat, AI_MATKEY_BASE_COLOR, &baseColor) == AI_SUCCESS) {
            material.baseColorFactor = glm::vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a);
        }
        else if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &baseColor) == AI_SUCCESS) {
            material.baseColorFactor = glm::vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a);
        }

        aiColor4D emissive;
        if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &emissive) == AI_SUCCESS) {
            material.emissiveFactor = glm::vec3(emissive.r, emissive.g, emissive.b);
        }

        float metallic = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_METALLIC_FACTOR, &metallic) == AI_SUCCESS) {
            material.metallicFactor = metallic;
        }

        float roughness = 1.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_ROUGHNESS_FACTOR, &roughness) == AI_SUCCESS) {
            material.roughnessFactor = roughness;
        }

        float alpha = 1.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &alpha) == AI_SUCCESS) {
            opacity = alpha;
        }

        transparent = isTransparentMaterial(*mat, &opacity);
        aiString matName;
        if (mat->Get(AI_MATKEY_NAME, matName) == AI_SUCCESS) {
            if (nameMatchesGlass(matName.C_Str())) {
                transparent = true;
                opacity = std::min(opacity, 0.5f);
            }
        }

        auto baseColorMaps = loadMaterialTextures(mat, aiTextureType_BASE_COLOR, "texture_albedo", true);
        auto diffuseMaps = loadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_albedo", true);
        auto normalMaps = loadMaterialTextures(mat, aiTextureType_NORMALS, "texture_normal", false);
        auto metallicMaps = loadMaterialTextures(mat, aiTextureType_METALNESS, "texture_metallic_roughness", false);
        auto roughnessMaps = loadMaterialTextures(mat, aiTextureType_DIFFUSE_ROUGHNESS, "texture_metallic_roughness", false);
        auto aoMaps = loadMaterialTextures(mat, aiTextureType_AMBIENT_OCCLUSION, "texture_ao", false);
        auto emissiveMaps = loadMaterialTextures(mat, aiTextureType_EMISSIVE, "texture_emissive", true);
        auto specularMaps = loadMaterialTextures(mat, aiTextureType_SPECULAR, "texture_specular", false);
        auto glossMaps = loadMaterialTextures(mat, aiTextureType_SHININESS, "texture_glossiness", false);

        textures.insert(textures.end(), baseColorMaps.begin(), baseColorMaps.end());
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
        textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
        textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
        textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        textures.insert(textures.end(), glossMaps.begin(), glossMaps.end());
    }

    std::string meshName = mesh->mName.C_Str();
    if (meshName.empty()) {
        meshName = "mesh_" + std::to_string(meshes_.size());
    }

    return CarMesh(meshName, vertices, indices, textures, material, nodeTransform, transparent, opacity);
}

std::vector<CarTexture> CarModel::loadMaterialTextures(aiMaterial* material, aiTextureType type,
    const std::string& typeName, bool srgb) {
    std::vector<CarTexture> textures;
    for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
        aiString str;
        material->GetTexture(type, i, &str);
        std::string texturePath = str.C_Str();

        auto loaded = std::find_if(texturesLoaded_.begin(), texturesLoaded_.end(), [&](const CarTexture& texture) {
            return texture.path == texturePath && texture.type == typeName;
            });
        if (loaded != texturesLoaded_.end()) {
            textures.push_back(*loaded);
            continue;
        }

        CarTexture texture;
        texture.id = textureFromFile(directory_ + "/" + texturePath, srgb);
        texture.type = typeName;
        texture.path = texturePath;
        texture.srgb = srgb;
        textures.push_back(texture);
        texturesLoaded_.push_back(texture);
    }
    return textures;
}

bool CarModel::isTransparentMaterial(const aiMaterial& material, float* outOpacity) {
    float opacity = 1.0f;
    if (material.Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
        if (outOpacity) {
            *outOpacity = opacity;
        }
        return opacity < 0.98f;
    }

    int blend = 0;
    if (material.Get(AI_MATKEY_BLEND_FUNC, blend) == AI_SUCCESS) {
        return blend != 0;
    }

    return false;
}

glm::mat4 CarModel::convertMatrix(const aiMatrix4x4& from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

unsigned int CarModel::textureFromFile(const std::string& path, bool srgb) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width = 0;
    int height = 0;
    int nrComponents = 0;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = GL_RGB;
        GLenum internalFormat = GL_RGB;
        if (nrComponents == 1) {
            format = GL_RED;
            internalFormat = GL_RED;
        }
        else if (nrComponents == 3) {
            format = GL_RGB;
            internalFormat = srgb ? GL_SRGB : GL_RGB;
        }
        else if (nrComponents == 4) {
            format = GL_RGBA;
            internalFormat = srgb ? GL_SRGB_ALPHA : GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}