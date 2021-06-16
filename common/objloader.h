#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <glm/glm.hpp>


bool loadQuadOBJ(const char * path, std::vector<glm::vec3> &out_vertices, std::vector<glm::vec3> &out_normals);

struct SimpleVertex
{
    glm::vec3 position, normal;
};

struct Mesh
{
    std::vector<SimpleVertex> vertices;
};

bool load_obj(const std::string &filename, const std::string &base_dir, std::vector<Mesh> &meshes);

#endif
