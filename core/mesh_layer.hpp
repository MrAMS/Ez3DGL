#pragma once

#include <cstddef>
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

class Shader{
public:
    float tmp_material_shininess=32;

    void setup_shader(){
        assert_with_info(shader==nullptr, "shader is already setup");
        shader = new shader_t("../shader/fragpos_normal_texcoord_lightpos.vs", "../shader/multiple_lights.fs", "view", "projection", "model");
    }
    // TODO update_lights
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
    friend class LightDir;
    friend class LightPoint;
    friend class LightSpot;
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

class LightBase{
public:
    glm::vec3 ambient=glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 diffuse=glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 specular=glm::vec3(1.0f, 1.0f, 1.0f);

    LightBase(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular):ambient(ambient), diffuse(diffuse), specular(specular) {

    }

    virtual void apply_shader(Shader* shader) const = 0;
};

class LightDir : public LightBase {
public:
    glm::vec3 direction;
    LightDir(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction):
        LightBase(ambient, diffuse, specular), direction(direction) {

    }
    void apply_shader(Shader* shader) const override{
        shader->shader->use();

        shader->shader->set_uniform("light_dir.ambient", ambient);
        shader->shader->set_uniform("light_dir.diffuse", diffuse);
        shader->shader->set_uniform("light_dir.specular", specular);

        shader->shader->set_uniform("light_dir.direction", direction);
    }
};

class LightPoint : public LightBase{
public:
    glm::vec3 position;

    float constant = 1.f;
    float linear = 0.0014f;
    float quadratic = 0.000007f;

    LightPoint(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
        glm::vec3 position,
        float constant, float linear, float quadratic):
            LightBase(ambient, diffuse, specular), position(position), constant(constant), linear(linear), quadratic(quadratic) {
    }
    void apply_shader(Shader* shader) const override{
        shader->shader->use();

        shader->shader->set_uniform("light_point.ambient", ambient);
        shader->shader->set_uniform("light_point.diffuse", diffuse);
        shader->shader->set_uniform("light_point.specular", specular);

        shader->shader->set_uniform("light_point.position", position);

        shader->shader->set_uniform("light_point.constant", constant);
        shader->shader->set_uniform("light_point.linear", linear);
        shader->shader->set_uniform("light_point.quadratic", quadratic);
    }
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
    void apply_shader(Shader* shader) const override{
        shader->shader->use();

        auto cutoff = glm::cos(glm::radians(inner_degree));
        auto cutoff_outer = glm::cos(glm::radians(outer_degree));
        shader->shader->set_uniform("light_spot.ambient", ambient);
        shader->shader->set_uniform("light_spot.diffuse", diffuse);
        shader->shader->set_uniform("light_spot.specular", specular);

        shader->shader->set_uniform("light_spot.position", position);
        shader->shader->set_uniform("light_spot.direction", direction);

        shader->shader->set_uniform("light_spot.constant", constant);
        shader->shader->set_uniform("light_spot.linear", linear);
        shader->shader->set_uniform("light_spot.quadratic", quadratic);

        shader->shader->set_uniform("light_spot.cutoff", cutoff);
        shader->shader->set_uniform("light_spot.cutoff_outer", cutoff_outer);
    }
private:

};


/**
 * @brief 光源对象,支持定向光源,点光源,聚光
 * 
 */
class Lights{
public:
    std::vector<LightDir> dir_light;
    // note that the size has limited
    std::vector<LightPoint> point_lights;
    std::vector<LightSpot> spot_light;
    void apply_shader(Shader* shader) const{
        for(const auto& dir : dir_light){
            dir.apply_shader(shader);
        }
        for(const auto& point : point_lights){
            point.apply_shader(shader);
        }
        for(const auto& spot : spot_light){
            spot.apply_shader(shader);
        }
    }
};

}

