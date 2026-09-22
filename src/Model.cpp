#include "Model.h"
#include <cstdio>
#include <cstring>

Model::Model(const std::string &path, MTL::Device *metalDevice)
{
    ufbx_load_opts opts = {};
    opts.target_axes = ufbx_axes_right_handed_y_up;
    opts.target_unit_meters = 1.0f;

    std::string mat_name;

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
        if (!mesh)
            continue;

        for (ufbx_mesh_part &part : mesh->material_parts)
        {
            if (part.num_triangles == 0)
                continue;

            std::vector<VertexData> vertices;
            std::vector<uint32_t> tri_indices(mesh->max_face_triangles * 3);

            for (uint32_t face_index : part.face_indices)
            {
                ufbx_face face = mesh->faces[face_index];
                uint32_t num_tris = ufbx_triangulate_face(tri_indices.data(), tri_indices.size(), mesh, face);

                auto *material = mesh->materials.data[part.index];


                const ufbx_material_list materiallist = mesh->materials;

                if(materiallist.count > 0) loadTextures(materiallist,metalDevice);


                mat_name = material->name.data;

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

            size_t num_vertices = ufbx_generate_indices(streams, 1, indices.data(), indices.size(), nullptr, nullptr);
            vertices.resize(num_vertices);

            meshes.emplace_back(new Mesh(vertices, indices,mat_name,metalDevice));
        }
    }

    ufbx_free_scene(scene);
}

Model::~Model()
{
    for (Mesh *m : meshes)
        delete m;
}

void Model::loadTextures(const ufbx_material_list materiallist, MTL::Device * metalDevice)
{

    for (const auto &mat : materiallist)
    {
        if (pbr_textures_map.find(mat->name.data) != pbr_textures_map.end())
            continue;

        const ufbx_material_texture_list materiallist_textures = mat->textures;
        PBRMaterial pbrmat;
        PBR_COLOR pbr_color;


        if(mat->pbr.base_color.texture_enabled == false){
            pbr_color.base_color = float4{(float)mat->pbr.base_color.value_vec3.x , (float)mat->pbr.base_color.value_vec3.y ,(float) mat->pbr.base_color.value_vec3.z , (float) 1.0f};
            pbr_color_map[mat->name.data] = std::move(pbr_color);
            continue;
        }

        
        for (const auto &tex : materiallist_textures)
        {
            if (tex.texture->content.data && tex.texture->content.size > 0)
            {
                Texture *texture = new Texture((unsigned char *)tex.texture->content.data, (size_t)tex.texture->content.size, metalDevice);
                if (strcmp(tex.texture->element.name.data, "base_color_texture") == 0)
                    pbrmat.Albedo_texture = texture;
                if (strcmp(tex.texture->element.name.data, "normalmap_texture") == 0)
                    pbrmat.Normal_texture = texture;
                if (strcmp(tex.texture->element.name.data, "metallic_texture") == 0)
                    pbrmat.Metallic_texture = texture;
                if (strcmp(tex.texture->element.name.data, "roughness_texture") == 0)
                    pbrmat.Roughness_texture = texture;
                if (strcmp(tex.texture->element.name.data, "specular_texture") == 0)
                    pbrmat.Specular_Texture = texture;
                
              std::cout << tex.texture->element.name.data << std::endl;
            }

            pbr_textures_map[mat->name.data] = std::move(pbrmat);
            
        }
    }
}


void Model::Draw(MTL::RenderCommandEncoder * encoder, MTL::RenderPipelineState * Rpso, MTL::DepthStencilState * DSO,  MTL::Buffer * transformationBuffer){
    for(const auto &i : meshes){
        i->Draw(encoder,Rpso,DSO,this->pbr_textures_map[i->mat_name],this->pbr_color_map[i->mat_name],transformationBuffer);
    }
}
