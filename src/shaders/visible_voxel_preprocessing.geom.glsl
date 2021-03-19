#version 400

layout(points) in;
layout(points, max_vertices = 8) out;

in ivec2 processing_start_coord_gs[];

uniform ivec3 volume_dim;
uniform vec3 voxel_dim;
uniform ivec2 processing_block_range;
uniform sampler2D visible_voxel_tex;

uniform mat4 reorient_matrix;
uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;

out vec4 visible_voxel_cood;

void main()
{
    ivec2 tex_dim = textureSize(visible_voxel_tex, 0);
    ivec2 new_processing_block_range = min(processing_block_range, tex_dim - processing_start_coord_gs[0]);

    ivec3 current_voxel_coord = ivec3(-1);
    uint count = 0;

    for(uint j = 0; j < new_processing_block_range.y; j++)
    {
        for(uint i = 0; i < new_processing_block_range.x; i++)
        {
            vec3 normalized_voxel_coord = texelFetch(visible_voxel_tex, ivec2(processing_start_coord_gs[0].x + i, processing_start_coord_gs[0].y + j), 0).xyz;

            if(any(equal(normalized_voxel_coord, vec3(-1))))
                continue;

            ivec3 voxel_coord = ivec3(normalized_voxel_coord * volume_dim);
            voxel_coord = min(voxel_coord, volume_dim - 1);

            if(all(equal(current_voxel_coord, voxel_coord)))
            {
                count++;

            } else {

                if(count > 0)
                {
                    gl_Position = projection_matrix * model_view_matrix * reorient_matrix * vec4(current_voxel_coord * voxel_dim + (voxel_dim * 0.5), 1.0);
                    gl_Position = vec4(clamp(gl_Position.xy, vec2(-0.999, -0.999), vec2(0.999, 0.999)), gl_Position.zw);
                    visible_voxel_cood = vec4(current_voxel_coord, (int(count) * 0.001));

                    EmitVertex();
                    EndPrimitive();
                }

                current_voxel_coord = voxel_coord;
                count = 1;
            }
        }
    }

    if(count > 0)
    {
        gl_Position = projection_matrix * model_view_matrix * reorient_matrix * vec4(current_voxel_coord * voxel_dim + (voxel_dim * 0.5), 1.0);
        gl_Position = vec4(clamp(gl_Position.xy, vec2(-0.999, -0.999), vec2(0.999, 0.999)), gl_Position.zw);
        visible_voxel_cood = vec4(current_voxel_coord, (int(count) * 0.001));

        EmitVertex();
        EndPrimitive();
    }
}
