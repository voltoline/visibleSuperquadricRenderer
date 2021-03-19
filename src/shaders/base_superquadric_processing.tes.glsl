#version 400

layout(quads, equal_spacing, cw) in;

out vec2 superquadric_coord;

const float PI = 3.14159265;
const float eps = PI / 1000000.0;

void main()
{
    float new_coord_x = gl_TessCoord.x * 2 * PI;
    float new_coord_y = gl_TessCoord.y * PI;


    if(gl_TessCoord.x == 0.0)
        new_coord_x = eps;

    else if(gl_TessCoord.x == 0.25 || gl_TessCoord.x == 0.5 || gl_TessCoord.x == 0.75)
        new_coord_x -= eps;

    else if(gl_TessCoord.x == 1.0)
        new_coord_x = eps;


    if(gl_TessCoord.y == 0.0)
        new_coord_y = eps;

    else if(gl_TessCoord.y == 0.5 || gl_TessCoord.y == 1.0)
        new_coord_y -= eps;


    if(gl_TessCoord.y == 0.0 || gl_TessCoord.y == 1.0)
        new_coord_x = eps;

    superquadric_coord = vec2(new_coord_x , new_coord_y);
}
