#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <sys/socket.h>
#include <vector>
#include "vertices_layer.hpp"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "utils/debug.hpp"

namespace Ez3DGL{

struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

struct Texture {
    enum class Type {
        Diffuse,
        Specula
    };
    Type type;
    texture_t* tex=nullptr;
};

class Mesh{
public:
    std::vector<Vertex> vertex_data;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    void setup_vertices(){
        assert_with_info(vert==nullptr, "vertices is already setup");
        vert = new vertices_t(vertex_data.size(), {3, 3, 2}, (float*)&vertex_data[0], indices.size(), indices.data());
    }
    ~Mesh(){
        if(vert!=nullptr)
            delete vert;
    }

    void draw(shader_t* shader, const camera_t* camera, const model_t* model) const{
        assert_with_info(vert!=nullptr, "forget to setup vertices");
        shader->use();
        for(const auto& texture: textures)
            switch (texture.type) {
                case Texture::Type::Diffuse:
                    shader->bind_texture("material.diffuse", texture.tex);
                    break;
                case Texture::Type::Specula:
                    shader->bind_texture("material.specular", texture.tex);
                    break;
                default: panic_with_info("unsupport texture type");
            }
        shader->update_camera(camera);
        shader->update_model(model);
        
        vert->draw_element(GL_TRIANGLES);
    }
private:
    vertices_t* vert=nullptr;
};

class Model{
public:
    Model(std::string path){
        load_model(path);
        for(auto& mesh: meshes){
            mesh.setup_vertices();
        }
    }
    void draw(shader_t* shader, const camera_t* camera, const model_t* model){
        for(const auto& mesh: meshes){
            mesh.draw(shader, camera, model);
        }
    }
    ~Model(){
        for(const auto& texture: loaded_textures)
            delete texture.tex;
    }
private:
    std::vector<Mesh> meshes;
    std::vector<Texture> loaded_textures;
    std::string directory;
    void load_model(std::string path){
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);    

        assert_with_info(!(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode), "ERROR::ASSIMP::%s", import.GetErrorString());

        directory = path.substr(0, path.find_last_of('/'));

        process_node(scene->mRootNode, scene);
    }
    std::vector<Texture> process_texture(aiMaterial * mat, aiTextureType type){
        Texture::Type tex_type;
        switch (type) {
            case aiTextureType_DIFFUSE: tex_type = Texture::Type::Diffuse; break;
            case aiTextureType_SPECULAR: tex_type = Texture::Type::Specula; break;
            default: panic_with_info("unknown texture type %d", type);
        }
        std::vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString filename;
            mat->GetTexture(type, i, &filename);
            std::string filepath = directory + '/' + std::string(filename.C_Str());

            bool skip = false;
            for(const auto& texture: loaded_textures)
                if(filename.C_Str() == texture.tex->file_name && texture.type == tex_type){
                    skip = true;
                    textures.push_back(texture);
                }

            if(skip) continue;
            auto tex = new texture_t(filepath.c_str());
            textures.emplace_back(Texture{tex_type, tex});
            loaded_textures.emplace_back(Texture{tex_type, tex});
        }
        return textures;
    }
    Mesh process_mesh(aiMesh *mesh, const aiScene *scene){
        Mesh res;
        // 处理顶点数据
        for(unsigned int i = 0; i < mesh->mNumVertices; i++){
            res.vertex_data.emplace_back(Vertex{
                glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
                glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
                mesh->mTextureCoords[0] ?
                    glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) :
                    glm::vec2(0.0f, 0.0f)
            });
        }
        // 处理索引数据
        for(unsigned int i = 0; i < mesh->mNumFaces; i++){
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                res.indices.push_back(face.mIndices[j]);
        }
        // 处理材质数据
        auto tex_diff = process_texture(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE);
        res.textures.insert(res.textures.end(), tex_diff.begin(), tex_diff.end());
        auto tex_spe = process_texture(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_SPECULAR);
        res.textures.insert(res.textures.end(), tex_spe.begin(), tex_spe.end());
        return std::move(res);
    }
    void process_node(aiNode *node, const aiScene *scene){
        // 处理节点所有的网格（如果有的话）
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
            meshes.push_back(process_mesh(mesh, scene));         
        }
        // 接下来对它的子节点重复这一过程
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            process_node(node->mChildren[i], scene);
        }
    }
};

/**
 * @brief 光源对象,支持定向光源,点光源,聚光
 * 
 */
class Light{
public:

    light_dir_t dir_light;
    // note that the size has limited
    std::vector<light_point_t> point_lights;
    light_spot_t spot_light;


};

}

