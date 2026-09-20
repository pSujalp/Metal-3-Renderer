#include "Model.h"
#include <cstdio>
#include <cstring>

Model::Model(const std::string &path, MTL::Device *metalDevice)
{
    ufbx_load_opts opts = {};
    opts.target_axes = ufbx_axes_right_handed_y_up;
    opts.target_unit_meters = 1.0f;

    ufbx_error error;
    ufbx_scene *scene = ufbx_load_file(path.c_str(), &opts, &error);
    if (!scene)
    {
        fprintf(stderr, "ufbx failed to load '%s': %s\n", path.c_str(), error.description.data);
        return;
    }

    for (ufbx_node *node : scene->nodes)
    {
        ufbx_mesh *mesh = node->mesh;
        if (!mesh) continue;

        for (ufbx_mesh_part &part : mesh->material_parts)
        {
            if (part.num_triangles == 0) continue;

            std::vector<VertexData> vertices;
            std::vector<uint32_t> tri_indices(mesh->max_face_triangles * 3);

            for (uint32_t face_index : part.face_indices)
            {
                ufbx_face face = mesh->faces[face_index];
                uint32_t num_tris = ufbx_triangulate_face(tri_indices.data(), tri_indices.size(), mesh, face);
                for (size_t i = 0; i < num_tris * 3; i++)
                {
                    uint32_t index = tri_indices[i];
                    VertexData v;
                    memset(&v, 0, sizeof(v)); 

                    ufbx_vec3 p = ufbx_transform_position(&node->geometry_to_world, mesh->vertex_position[index]);
                    v.position = float3{(float)p.x, (float)p.y, (float)p.z};
                    if (mesh->vertex_uv.exists)
                    {
                        ufbx_vec2 uv = mesh->vertex_uv[index];
                        v.texCoords = float2{(float)uv.x, (float)uv.y};
                    }
                    vertices.push_back(v);
                }
            }

            ufbx_vertex_stream streams[1] = {
                {vertices.data(), vertices.size(), sizeof(VertexData)},
            };
            std::vector<uint32_t> indices(part.num_triangles * 3);

            size_t num_vertices = ufbx_generate_indices(
                streams, 1, indices.data(), indices.size(), nullptr, nullptr);
            vertices.resize(num_vertices);

            meshes.emplace_back(new Mesh(vertices, indices, metalDevice));
        }
    }

    ufbx_free_scene(scene);
}

Model::~Model()
{
    for (Mesh *m : meshes) delete m;
}