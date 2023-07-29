#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <glm/glm.hpp>

#include <GL/glew.h>

#include <vector>

#include "aabb.h" 

bool loadQuadOBJ(const char * path, std::vector<glm::vec3> &out_vertices, std::vector<glm::vec3> &out_normals);

struct PNVertex
{
    glm::vec3 position, normal;
};

struct Mesh
{
    std::vector<PNVertex> vertices;
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

    AABB m_aabb;

public:
    MeshBin() = delete;

    MeshBin(const std::string &filename);

    ~MeshBin()
    {
        for (int i = 0; i < m_object_num; i++)
        {
            glDeleteBuffers(1, &m_vbo_id[i]);
            glDeleteVertexArrays(1, &m_vao_id[i]);
        }
    }

    glm::vec3 Center() const
    {
        return m_aabb.Center();
    }

    float LogestDim() const
    {
        return m_aabb.LongestEdge();
    }

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


#endif
