#include "Model.h"

Model::Model(const std::string &str, MTL::Device *metalDevice)
{
    ufbx_scene *scene = ufbx_load_file(str.c_str(), NULL, NULL);

    for (ufbx_mesh *mesh : scene->meshes)
    {

        for (ufbx_mesh_part &part : mesh->material_parts)
        {

            std::vector<VertexData> vertices;
            std::vector<uint32_t> tri_indices;
            tri_indices.resize(mesh->max_face_triangles * 3);

            for (uint32_t face_index : part.face_indices)
            {
                ufbx_face face = mesh->faces[face_index];

                uint32_t num_tris = ufbx_triangulate_face(
                    tri_indices.data(), tri_indices.size(), mesh, face);

                for (size_t i = 0; i < num_tris * 3; i++)
                {
                    uint32_t index = tri_indices[i];

                    VertexData v;

                    v.position = float3{(float)mesh->vertex_position[index].x, (float)mesh->vertex_position[index].y, (float)mesh->vertex_position[index].z};
                    v.texCoords = float2{(float)mesh->vertex_position[index].x, (float)mesh->vertex_position[index].y};
                    vertices.push_back(v);
                }
            }

            ufbx_vertex_stream streams[1] = {
                {vertices.data(), vertices.size(), sizeof(VertexData)},
            };
            std::vector<uint32_t> indices;
            indices.resize(part.num_triangles * 3);

            size_t num_vertices = ufbx_generate_indices(
                streams, 1, indices.data(), indices.size(), nullptr, nullptr);

            vertices.resize(num_vertices);

            Mesh * meshy = new Mesh(vertices,indices,metalDevice);

            meshes.emplace_back(meshy);
        }
    }

    ufbx_free_scene(scene);
}


Model::~Model(){
    
}