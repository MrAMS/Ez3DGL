#pragma once

#include <cstddef>
#include <cstdlib>
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


class LightBase{
public:
    glm::vec3 ambient=glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 diffuse=glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 specular=glm::vec3(1.0f, 1.0f, 1.0f);

    LightBase(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular):ambient(ambient), diffuse(diffuse), specular(specular) {

    }
    LightBase(){

    }
};

class LightDir : public LightBase {
public:
    glm::vec3 direction;
    LightDir(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction):
        LightBase(ambient, diffuse, specular), direction(direction) {
    }
    LightDir(glm::vec3 direction):direction(direction) {

    }
};

class LightPoint : public LightBase{
public:
    glm::vec3 position;

    float constant = 1.f;
    float linear = 0.001f;
    float quadratic = 0.049f;

    LightPoint(
        glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
        glm::vec3 position,
        float constant, float linear, float quadratic):
            LightBase(ambient, diffuse, specular),
            position(position), constant(constant), linear(linear), quadratic(quadratic) {}
    LightPoint(
        glm::vec3 position,
        glm::vec3 color):
            LightBase(color, color, color),
            position(position) {}
};

class LightSpot : public LightPoint{
public:
    glm::vec3 direction;
    float inner_degree;
    float outer_degree;

    LightSpot(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
        glm::vec3 position, glm::vec3 direction,
        float constant, float linear, float quadratic,
        float inner_degree, float outer_degree):
            LightPoint(ambient, diffuse, specular, position, constant, linear, quadratic),
            direction(direction),
            inner_degree(inner_degree), outer_degree(outer_degree) {
                assert_with_info(inner_degree<outer_degree, "inner degree must be less than outer degree");
            }
    LightSpot(glm::vec3 position, glm::vec3 direction,
        glm::vec3 color,
        float inner_degree, float outer_degree):
            LightPoint(position, color),
            inner_degree(inner_degree), outer_degree(outer_degree){

            }
};

class Shader{
public:
    float tmp_material_shininess=32;

    void setup_shader(){
        assert_with_info(shader==nullptr, "shader is already setup");
        shader = new shader_t("../shader/fragpos_normal_texcoord_lightpos.vs", "../shader/multiple_lights.fs", "view", "projection", "model");
    }
    void set_lights(const std::vector<LightDir>& dir_lights, const std::vector<LightPoint>& point_lights, const std::vector<LightSpot>& spot_lights){
        shader->use();
        shader->set_uniform("dir_light_num", dir_lights.size());
        shader->set_uniform("point_light_num", point_lights.size());
        shader->set_uniform("spot_light_num", spot_lights.size());
        for(int i=0; i<dir_lights.size(); i++){
            std::string key = "lights_dir[" + std::to_string(i) + "]";
            shader->set_uniform(std::string(key+".ambient").c_str(),    dir_lights[i].ambient);
            shader->set_uniform(std::string(key+".diffuse").c_str(),    dir_lights[i].diffuse);
            shader->set_uniform(std::string(key+".specular").c_str(),   dir_lights[i].specular);
            shader->set_uniform(std::string(key+".direction").c_str(),  dir_lights[i].direction);
        }
        for(int i=0; i<point_lights.size(); i++){
            std::string key = "lights_point[" + std::to_string(i) + "]";
            shader->set_uniform(std::string(key+".ambient").c_str(),    point_lights[i].ambient);
            shader->set_uniform(std::string(key+".diffuse").c_str(),    point_lights[i].diffuse);
            shader->set_uniform(std::string(key+".specular").c_str(),   point_lights[i].specular);
            shader->set_uniform(std::string(key+".position").c_str(),   point_lights[i].position);

            shader->set_uniform(std::string(key+".constant").c_str(),   point_lights[i].constant);
            shader->set_uniform(std::string(key+".linear").c_str(),     point_lights[i].linear);
            shader->set_uniform(std::string(key+".quadratic").c_str(),  point_lights[i].quadratic);
        }
        for(int i=0; i<spot_lights.size(); i++){
            std::string key = "lights_spot[" + std::to_string(i) + "]";
            shader->set_uniform(std::string(key+".ambient").c_str(),    spot_lights[i].ambient);
            shader->set_uniform(std::string(key+".diffuse").c_str(),    spot_lights[i].diffuse);
            shader->set_uniform(std::string(key+".specular").c_str(),   spot_lights[i].specular);
            shader->set_uniform(std::string(key+".direction").c_str(),  spot_lights[i].direction);
            shader->set_uniform(std::string(key+".position").c_str(),   spot_lights[i].position);

            shader->set_uniform(std::string(key+".constant").c_str(),   spot_lights[i].constant);
            shader->set_uniform(std::string(key+".linear").c_str(),     spot_lights[i].linear);
            shader->set_uniform(std::string(key+".quadratic").c_str(),  spot_lights[i].quadratic);

            shader->set_uniform(std::string(key+".cutoff").c_str(),     glm::cos(glm::radians(spot_lights[i].inner_degree)));
            shader->set_uniform(std::string(key+".cutoff_outer").c_str(),glm::cos(glm::radians(spot_lights[i].outer_degree)));
        }

    }
    void bind(const std::vector<Texture>& textures, const camera_t* camera, const model_t* model){
        assert_with_info(shader!=nullptr, "forget to setup shader");
        shader->use();
        shader->clear_texture();
        int diffuse_cnt = 0, specular_cnt = 0;
        for(const auto& texture: textures)
            switch (texture.type) {
                case Texture::Type::Diffuse:{
                    std::string diffuse_key = "material.diffuse[" + std::to_string(diffuse_cnt) + "]";
                    shader->bind_texture(diffuse_key.c_str(), texture.tex);
                    diffuse_cnt+=1;
                    break;
                }   
                case Texture::Type::Specula:{
                    std::string specular_key = "material.specular[" + std::to_string(specular_cnt) + "]";
                    shader->bind_texture(specular_key.c_str(), texture.tex);
                    specular_cnt+=1;
                    break;
                }
                default: panic_with_info("unsupport texture type");
            }
        // shader->set_uniform("diffuse_num", diffuse_cnt);
        // shader->set_uniform("specular_num", spaiTexecular_cnt);
        shader->set_uniform("material.shininess", tmp_material_shininess);
        shader->set_uniform("viewPos", camera->position);
        shader->update_camera(camera);
        shader->update_model(model);
    }
    ~Shader(){
        if(shader!=nullptr)
            delete shader;
    }
    friend class Lights;
private:
    shader_t* shader=nullptr;
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

    void draw(Shader* shader, const camera_t* camera, const model_t* model) const{
        assert_with_info(vert!=nullptr, "forget to setup vertices");
        shader->bind(textures, camera, model);
        vert->draw_element(GL_TRIANGLES);
    }
private:
    vertices_t* vert=nullptr;
};

class Model{
public:
    Model()=default;
    Model(std::string path){
        setup_model(path);
    }
    void setup_model(std::string path){
        assert_with_info(model_path.empty(), "model is already setup");
        model_path = path;
        load_model(path);
        for(auto& mesh: meshes){
            mesh.setup_vertices();
        }
    }
    void draw(Shader* shader, const camera_t* camera, const model_t* model){
        assert_with_info(!model_path.empty(), "forget to setup model");
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
    std::string model_path;
    std::string directory;
    void load_model(std::string path){
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);    

        assert_with_info(!(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode), "ERROR::ASSIMP::%s", import.GetErrorString());

        directory = path.substr(0, path.find_last_of('/'));

        process_node(scene->mRootNode, scene);
    }
    std::vector<Texture> process_texture(aiMaterial * mat, aiTextureType type, const aiScene *scene){
        Texture::Type tex_type;
        switch (type) {
            case aiTextureType_DIFFUSE: tex_type = Texture::Type::Diffuse; break;
            case aiTextureType_SPECULAR: tex_type = Texture::Type::Specula; break;
            default: panic_with_info("unknown texture type %d", type);
        }
        std::vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            printf("texture %d %d\n", int(tex_type), i);
            aiString filename;
            mat->GetTexture(type, i, &filename);
            std::string filepath = directory + '/' + std::string(filename.C_Str());

            bool skip = false;
            for(const auto& texture: loaded_textures)
                if(texture.tex->file_name!=nullptr && filename.C_Str() == texture.tex->file_name && texture.type == tex_type){
                    skip = true;
                    textures.push_back(texture);
                }

            if(skip) continue;
            auto aitexture = scene->GetEmbeddedTexture(filename.C_Str());
            texture_t* tex=nullptr;
            if(aitexture!=nullptr){
                auto size = aitexture->mHeight == 0 ? aitexture->mWidth : aitexture->mHeight * aitexture->mWidth;
                tex = new texture_t(reinterpret_cast<unsigned char*>(aitexture->pcData), size);
            }else{
                tex = new texture_t(filepath.c_str());
            }
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
        if(mesh->mMaterialIndex>=0){
            auto tex_diff = process_texture(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE, scene);
            res.textures.insert(res.textures.end(), tex_diff.begin(), tex_diff.end());
            auto tex_spe = process_texture(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_SPECULAR, scene);
            res.textures.insert(res.textures.end(), tex_spe.begin(), tex_spe.end());
        }
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

}

