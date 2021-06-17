#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <glm/glm.hpp>

#include <GL/glew.h>

#include <vector>

bool loadQuadOBJ(const char * path, std::vector<glm::vec3> &out_vertices, std::vector<glm::vec3> &out_normals);

struct SimpleVertex
{
    glm::vec3 position, normal;
};

struct Mesh
{
    std::vector<SimpleVertex> vertices;

};

class MeshBin
{
private:
    //const GLuint m_max_object_num = 256;
    const GLuint m_max_object_num;
    size_t m_object_num{ 0 };
    std::vector<GLuint> m_vao_id{m_max_object_num, 0};
    std::vector<GLuint> m_vbo_id{m_max_object_num, 0};
    std::vector<size_t> m_vb_size{m_max_object_num, 0};
    std::vector<size_t> m_vertex_num{m_max_object_num, 0};

    std::vector<Mesh> m_meshes; //< binned meshes

public:
    MeshBin() = delete;

    MeshBin(const std::string &filename);

    size_t size() const
    {
        return m_object_num;
    }

    size_t vertex_num(int index) const
    {
        return m_vertex_num[index];
    }

    GLuint vao(int index) const
    {
        return m_vao_id[index];
    }

private:

    void create_vaos();
};


bool load_obj(const std::string &filename, const std::string &base_dir, std::vector<Mesh> &meshes);

#endif
