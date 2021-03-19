#version 400

layout(points) in;
layout(points, max_vertices = 5) out;

uniform isamplerBuffer rendering_data_buffer;
uniform uint fixed_grid_size;

out uint rendering_data;

void main()
{
    int buffer_size = textureSize(rendering_data_buffer);

    int max_pixel_count = 0;
    int num_visible_voxel = 0;

    for(int i = 0; i < 10; i++)
    {
        ivec4 rendering_data = texelFetch(rendering_data_buffer, i);
        int visible_voxel_count = rendering_data.x;
        int pixel_count = rendering_data.y;

        max_pixel_count = max(max_pixel_count, pixel_count);

        num_visible_voxel += visible_voxel_count;
    }

    uint base_superquadric_grid_size = uint(3 + sqrt((max_pixel_count) / (2 * sqrt(max_pixel_count) / 8.f)));
    if(base_superquadric_grid_size > 30)
        base_superquadric_grid_size = 30;

    if(fixed_grid_size > 0)
        base_superquadric_grid_size = fixed_grid_size;

    rendering_data = base_superquadric_grid_size;

    EmitVertex();
    EndPrimitive();

    rendering_data = base_superquadric_grid_size * base_superquadric_grid_size * 2 * 3;

    EmitVertex();
    EndPrimitive();

    rendering_data = num_visible_voxel;

    EmitVertex();
    EndPrimitive();

    rendering_data = 0;

    EmitVertex();
    EndPrimitive();

    rendering_data = 0;

    EmitVertex();
    EndPrimitive();
}
