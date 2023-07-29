#include <vector>
#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>

#include "Log.h" 


#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "objloader.h"


static AABB load_obj(const std::string &filename, const std::string &base_dir, std::vector<Mesh> &meshes)
{
    std::stringstream ss;
    std::vector<tinyobj::material_t> materials;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;

    AABB aabb;

    std::string err;
    if ( !tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), base_dir.c_str()) )
    {
        throw std::runtime_error("Failed to load model : " + filename);
        return aabb;
    }
    if (!err.empty())
        Log("{}\n", err.c_str());

    if (materials.size() == 0)
    {
        Log("No material found in 3D model : {}", filename);
        meshes.resize(1);
    }
    else
    {
        meshes.resize(materials.size());
    }

    //< is this macro: FLT_MAX OS dependent ?
    //< should always prefer os independent ones
    glm::vec3 pmin(FLT_MAX), pmax(-FLT_MAX);

    // Loop over shapes
    for (const auto &shape : shapes)
    {
        size_t index_offset = 0, face = 0;

        //< Loop over all faces(polygon) in a mesh. vertex number of face maybe variable
        //< for triangle num_face_vertices = 3
        for (const auto &num_face_vertex : shape.mesh.num_face_vertices)
        {
            int mat = -1;
            if (materials.size() == 0)
            {
                mat = 0;
            }
            else
            {
                mat = shape.mesh.material_ids[face];
            }
            // Loop over triangles in the face.
            for (size_t v = 0; v < num_face_vertex; ++v)
            {
                tinyobj::index_t index = shape.mesh.indices[index_offset + v];

                meshes.at(mat).vertices.emplace_back(); //< use "material id" to index mesh : mesh <--> material id is one-one map
                auto &vert = meshes.at(mat).vertices.back();
                vert.position =
                {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                };
                pmin = glm::min(vert.position, pmin);
                pmax = glm::max(vert.position, pmax);
                if (~index.normal_index) //< -1 == 0xFFFFFFFF, it is equal to if (index.normal_index != -1)
                {
                    vert.normal =
                    {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                    vert.normal = glm::normalize(vert.normal);
                    Log("Normal = {:8.4},{:8.4},{:8.4}", vert.normal.x, vert.normal.y, vert.normal.z);
                }
                else
                {
                    throw std::runtime_error("No normal channel found in vertex stream.");
                    return aabb;
                }

            }
            index_offset += num_face_vertex;
            face++;
        }
    }

    aabb.Extend(pmin);
    aabb.Extend(pmax);

    ss << "After binning," << meshes.size() << " meshes constructed(each mesh contains only one material)" << "\n"
       << "Mesh file : " << filename << "\n"
       << aabb.str();
    Log("{}", ss.str());

#if 0
    //< Normalize position attribute to [-1, 1]
    glm::vec3 extent3 = pmax - pmin;
    float extent = glm::max(extent3.x, glm::max(extent3.y, extent3.z)) * 0.5f;
    float inv_extent = 1.0f / extent;
    glm::vec3 center = (pmax + pmin) * 0.5f;

    for (auto &m : meshes)
        for (auto &v : m.vertices)
            v.position = (v.position - center) * inv_extent;

    Log("Load {} meshes from file {}.\t Position attribute normalized to [-1, 1]", meshes.size(), filename);

#endif

    return aabb;
}

///////////////////////////// Class MeshBin //////////////////////////////////////////////

    MeshBin::MeshBin(const std::string &filename)
        : m_max_object_num(256)
    {
        m_aabb = load_obj(filename, "Model/", m_meshes);
        create_vaos();
    }

    void MeshBin::create_vaos()
    {
        for (int i = 0; i < m_meshes.size(); ++i)
        {
            GLenum errorCheckValue = glGetError();

            m_vb_size[m_object_num] = m_meshes[i].vertices.size() * sizeof(PNVertex);
            m_vertex_num[m_object_num] = m_meshes[i].vertices.size();

            const size_t vertexStride = sizeof(PNVertex);
            const size_t normalOffset = sizeof(m_meshes[i].vertices[0].position);

            glGenVertexArrays(1, &m_vao_id[m_object_num]);
            glBindVertexArray(m_vao_id[m_object_num]);

            glGenBuffers(1, &m_vbo_id[m_object_num]);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id[m_object_num]);
            glBufferData(GL_ARRAY_BUFFER, m_vb_size[m_object_num], m_meshes[i].vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, vertexStride, 0);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexStride, (GLvoid*)normalOffset);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);

            errorCheckValue = glGetError();
            if (errorCheckValue != GL_NO_ERROR)
            {
                fprintf(stderr, "Error: Could not create a VBO: %s\n", gluErrorString(errorCheckValue));
            }
            m_object_num++;
        }
    }
