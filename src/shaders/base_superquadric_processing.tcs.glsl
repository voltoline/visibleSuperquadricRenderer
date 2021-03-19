#version 400

layout(vertices = 1) out;

layout(std140) uniform base_superquadric_grid
{
    uint size;

} grid_info;

void main()
{
    float size_grid = grid_info.size;
    gl_TessLevelInner[0] = size_grid;
    gl_TessLevelInner[1] = size_grid;
    gl_TessLevelOuter[0] = size_grid;
    gl_TessLevelOuter[1] = size_grid;
    gl_TessLevelOuter[2] = size_grid;
    gl_TessLevelOuter[3] = size_grid;
}
