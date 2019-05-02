#pragma once
#include <vector>
#include <fstream>

#include "libs/glm/glm.hpp"
#include "shader.h"
#include "vertex_buffer.h"
#include "index_buffer.h"

struct Material {
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emissive;
    float shininess;
};

class Mesh {
public:
    Mesh(std::vector<Vertex>& vertices, uint64 numVertices, std::vector<uint32>& indices, uint64 numIndices, Material material, Shader* shader) {
        this->material = material;
        this->shader = shader;
        this->numIndices = numIndices;

        vertexBuffer = new VertexBuffer(vertices.data(), numVertices);
        indexBuffer = new IndexBuffer(indices.data(), numIndices, sizeof(indices[0]));

        diffuseLocation = GLCALL(glGetUniformLocation(shader->getShaderId(), "u_diffuse"));
        specularLocation = GLCALL(glGetUniformLocation(shader->getShaderId(), "u_specular"));
        emissiveLocation = GLCALL(glGetUniformLocation(shader->getShaderId(), "u_emissive"));
        shininessLocation = GLCALL(glGetUniformLocation(shader->getShaderId(), "u_shininess"));
    }
    ~Mesh() {
        delete vertexBuffer;
        delete indexBuffer;
    }
    inline void render() {
        vertexBuffer->bind();
        indexBuffer->bind();
        glUniform3fv(diffuseLocation, 1, (float*)&material.diffuse.data);
        glUniform3fv(specularLocation, 1, (float*)&material.specular.data);
        glUniform3fv(emissiveLocation, 1, (float*)&material.emissive.data);
        glUniform1f(shininessLocation, material.shininess);
        GLCALL(glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0));
    }
private:
    VertexBuffer* vertexBuffer;
    IndexBuffer* indexBuffer;
    Shader* shader;
    Material material;
    uint64 numIndices = 0;
    int diffuseLocation;
    int specularLocation;
    int emissiveLocation;
    int shininessLocation;
};

class Model {
public:
    void init(const char* filename, Shader* shader) {
        uint64 numMeshes = 0;
        std::ifstream input = std::ifstream(filename, std::ios::in | std::ios::binary);
        if(!input.is_open()) {
            std::cout << "File not found" << std::endl;
            return;
        }

        input.read((char*)&numMeshes, sizeof(uint64));
        
        for(uint64 i = 0; i < numMeshes; i++) {
            Material material;
            std::vector<Vertex> vertices;
            uint64 numVertices = 0;
            std::vector<uint32> indices;
            uint64 numIndices = 0;

            input.read((char*)&material, sizeof(Material));
            input.read((char*)&numVertices, sizeof(uint64));
            input.read((char*)&numIndices, sizeof(uint64));

            for(uint64 i = 0; i < numVertices; i++) {
                Vertex vertex;
                input.read((char*)&vertex.position.x, sizeof(float));
                input.read((char*)&vertex.position.y, sizeof(float));
                input.read((char*)&vertex.position.z, sizeof(float));
                input.read((char*)&vertex.normal.x, sizeof(float));
                input.read((char*)&vertex.normal.y, sizeof(float));
                input.read((char*)&vertex.normal.z, sizeof(float));
                vertices.push_back(vertex);
            }
            for(uint64 i = 0; i < numIndices; i++) {
                uint32 index;
                input.read((char*)&index, sizeof(uint32));
                indices.push_back(index);
            }

            Mesh* mesh = new Mesh(vertices, numVertices, indices, numIndices, material, shader);
            meshes.push_back(mesh);
        }
    }

    void render() {
        for(Mesh* mesh : meshes) {
            mesh->render();
        }
    }

    ~Model() {
        for(Mesh* mesh : meshes) {
            delete mesh;
        }
    }
private:
    std::vector<Mesh*> meshes;
};