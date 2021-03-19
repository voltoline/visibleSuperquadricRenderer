#version 400

layout(points) in;
layout(points, max_vertices = 10) out;

in ivec2 processing_start_coord_gs[];

uniform ivec3 volume_dim;
uniform ivec2 last_processing_block_coord;
uniform ivec2 processing_block_range;
uniform sampler2D visible_voxel_tex;
uniform samplerBuffer fa_buffer;
uniform float fa_threshold;

out int voxel_index;
out int pixel_count;

void main()
{
    ivec2 tex_dim = textureSize(visible_voxel_tex, 0);
    ivec2 new_processing_block_range = min(processing_block_range, tex_dim - processing_start_coord_gs[0]);

    for(uint j = 0; j < new_processing_block_range.y; j++)
    {
        for(uint i = 0; i < new_processing_block_range.x; i++)
        {
            vec4 normalized_voxel_coord = texelFetch(visible_voxel_tex, ivec2(processing_start_coord_gs[0].x + i, processing_start_coord_gs[0].y + j), 0);

            if(all(equal(normalized_voxel_coord, vec4(0))))
                continue;

            voxel_index = int(normalized_voxel_coord.x + normalized_voxel_coord.y * volume_dim.x + normalized_voxel_coord.z * volume_dim.y * volume_dim.x);
            pixel_count = int(normalized_voxel_coord.a * 1000);

            float fa_value = texelFetch(fa_buffer, voxel_index).a;
            if(fa_value > fa_threshold)
            {
                EmitVertex();
                EndPrimitive();
            }
        }
    }

    if(all(equal(last_processing_block_coord, processing_start_coord_gs[0])))
    {
        for(uint i = 0; i < 10; i++)
        {
            voxel_index = -1;
            pixel_count = -1;

            EmitVertex();
            EndPrimitive();
        }
    }
}
