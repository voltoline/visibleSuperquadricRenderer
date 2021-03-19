#version 400

layout(points) in;
layout(points, max_vertices = 1) out;

uniform isamplerBuffer visible_voxel_buffer;

out int num_visible_voxels;
out int max_voxel_pixel;

void main()
{
    int buffer_size = textureSize(visible_voxel_buffer);

    int max_pixel_count = 0;
    int visible_voxel_count = 0;
    bool flag_exit_for = false;

    for(int i = 0; i < buffer_size && !flag_exit_for; i++)
    {
        ivec4 voxel_info = texelFetch(visible_voxel_buffer, gl_PrimitiveIDIn + i * 10);
        int voxel_index = voxel_info.x;
        int pixel_count = voxel_info.y;

        max_pixel_count = max(max_pixel_count, pixel_count);

        if(any(equal(ivec2(voxel_index, pixel_count), ivec2(-1))))
        {
            flag_exit_for = true;

        } else {

            if(pixel_count > 0)
                visible_voxel_count++;
        }
    }

    num_visible_voxels = visible_voxel_count;
    max_voxel_pixel = max_pixel_count;

    EmitVertex();
    EndPrimitive();
}
