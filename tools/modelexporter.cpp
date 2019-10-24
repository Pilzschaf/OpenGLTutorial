#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Position {
    float x, y, z;
};

struct Position2D {
    float x, y;
};

struct Material {
    Position diffuse;
    Position specular;
    Position emissive;
    float shininess;
    aiString diffuseMapName;
    aiString normalMapName;
};

struct BMFMaterial {
    Position diffuse;
    Position specular;
    Position emissive;
    float shininess;
};

struct Mesh {
    std::vector<Position> positions;
    std::vector<Position> normals;
    std::vector<Position> tangents;
    std::vector<Position2D> uvs;
    std::vector<uint32_t> indices;
    int materialIndex;
};

std::vector<Mesh> meshes;
std::vector<Material> materials;

void processMesh(aiMesh* mesh, const aiScene* scene) {
    Mesh m;
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Position position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        m.positions.push_back(position);

        Position normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        m.normals.push_back(normal);

        Position tangent;
        tangent.x = mesh->mTangents[i].x;
        tangent.y = mesh->mTangents[i].y;
        tangent.z = mesh->mTangents[i].z;
        m.tangents.push_back(tangent);

        Position2D uv;
        assert(mesh->mNumUVComponents > 0);
        uv.x = mesh->mTextureCoords[0][i].x;
        uv.y = mesh->mTextureCoords[0][i].y;
        m.uvs.push_back(uv);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        assert(face.mNumIndices == 3);
        for(unsigned int j = 0; j < face.mNumIndices; j++) {
            m.indices.push_back(face.mIndices[j]);
        }
    }

    m.materialIndex = mesh->mMaterialIndex;
    meshes.push_back(m);
}

void processNode(aiNode* node, const aiScene* scene) {

    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

char* getFilename(char* filename) {
    int len = strlen(filename);
    char* lastSlash = filename;
    for(int i = 0; i < len; i++) {
        if(filename[i] == '/' || filename[i] == '\\') {
            lastSlash = filename+i+1;
        }
    }
    return lastSlash;
}

void processMaterials(const aiScene* scene) {
    for(uint32_t i = 0; i < scene->mNumMaterials; i++) {
        Material mat = {};
        aiMaterial* material = scene->mMaterials[i];

        aiColor3D diffuse(0.0f, 0.0f, 0.0f);
        if(AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse)) {
            // No diffuse color
        }
        mat.diffuse = {diffuse.r, diffuse.g, diffuse.b};

        aiColor3D specular(0.0f, 0.0f, 0.0f);
        if(AI_SUCCESS != material->Get(AI_MATKEY_COLOR_SPECULAR, specular)) {
            // No specular color
        }
        mat.specular = {specular.r, specular.g, specular.b};

        aiColor3D emissive(0.0f, 0.0f, 0.0f);
        if(AI_SUCCESS != material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive)) {
            // No emissive color
        }
        mat.emissive = {emissive.r, emissive.g, emissive.b};

        float shininess = 0.0f;
        if(AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, shininess)) {
            // No shininess
        }
        mat.shininess = shininess;

        float shininessStrength = 1.0f;
        if(AI_SUCCESS != material->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength)) {
            // No shininessStrength
        }
        mat.specular.x *= shininessStrength;
        mat.specular.y *= shininessStrength;
        mat.specular.z *= shininessStrength;

        uint32_t numDiffuseMaps = material->GetTextureCount(aiTextureType_DIFFUSE);
        uint32_t numNormalMaps = material->GetTextureCount(aiTextureType_NORMALS);
        assert(numDiffuseMaps > 0);
        material->GetTexture(aiTextureType_DIFFUSE, 0, &mat.diffuseMapName);
        assert(numNormalMaps > 0);
        material->GetTexture(aiTextureType_NORMALS, 0, &mat.normalMapName);

        materials.push_back(mat);
    }
}

int main(int argc, char** argv) {
    if(argc <= 0) {
        return 1;
    }
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " <modelfilename>" << std::endl;
        return 1;
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(argv[argc-1], aiProcess_PreTransformVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_CalcTangentSpace);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE, !scene->mRootNode) {
        std::cout << "Error while loading model with assimp: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    processMaterials(scene);
    processNode(scene->mRootNode, scene);

    std::string filename = std::string(getFilename(argv[argc-1]));
    std::string filenameWithoutExtension = filename.substr(0, filename.find_last_of('.'));
    std::string outputFilename = filenameWithoutExtension + ".bmf";

    std::ofstream output(outputFilename, std::ios::out | std::ios::binary);
    std::cout << "Writing bmf file..." << std::endl;

    // Materials
    uint64_t numMaterials = materials.size();
    output.write((char*)&numMaterials, sizeof(uint64_t));
    for(Material material : materials) {
        output.write((char*)&material, sizeof(BMFMaterial));
        const char* pathPrefix = "models/";
        // Diffuse map
        uint64_t diffuesMapNameLength = material.diffuseMapName.length + 7;
        output.write((char*)&diffuesMapNameLength, sizeof(uint64_t));
        output.write(pathPrefix, 7);
        output.write((char*)&material.diffuseMapName.data, material.diffuseMapName.length);

        // Normal map
        uint64_t normalMapNameLength = material.normalMapName.length + 7;
        output.write((char*)&normalMapNameLength, sizeof(uint64_t));
        output.write(pathPrefix, 7);
        output.write((char*)&material.normalMapName.data, material.normalMapName.length);
    }

    // Meshes
    uint64_t numMeshes = meshes.size();
    output.write((char*)&numMeshes, sizeof(uint64_t));
    for(Mesh& mesh : meshes) {
        uint64_t numVertices = mesh.positions.size();
        uint64_t numIndices = mesh.indices.size();
        uint64_t materialIndex = mesh.materialIndex;

        output.write((char*)&materialIndex, sizeof(uint64_t));
        output.write((char*)&numVertices, sizeof(uint64_t));
        output.write((char*)&numIndices, sizeof(uint64_t));
        for(uint64_t i = 0; i < numVertices; i++) {
            output.write((char*)&mesh.positions[i].x, sizeof(float));
            output.write((char*)&mesh.positions[i].y, sizeof(float));
            output.write((char*)&mesh.positions[i].z, sizeof(float));

            output.write((char*)&mesh.normals[i].x, sizeof(float));
            output.write((char*)&mesh.normals[i].y, sizeof(float));
            output.write((char*)&mesh.normals[i].z, sizeof(float));

            output.write((char*)&mesh.tangents[i].x, sizeof(float));
            output.write((char*)&mesh.tangents[i].y, sizeof(float));
            output.write((char*)&mesh.tangents[i].z, sizeof(float));

            output.write((char*)&mesh.uvs[i].x, sizeof(float));
            output.write((char*)&mesh.uvs[i].y, sizeof(float));
        }
        for(uint64_t i = 0; i < numIndices; i++) {
            output.write((char*)&mesh.indices[i], sizeof(uint32_t));
        }
    }
    
    output.close();
}