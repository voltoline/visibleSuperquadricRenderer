#version 400

in vec4 visible_voxel_cood;

out vec4 color;

void main(void)
{
    color = visible_voxel_cood;
}
