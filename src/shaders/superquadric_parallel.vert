#version 400

in vec4 position;
in int voxel_index;

uniform samplerBuffer glyph_color_tbo;
uniform samplerBuffer tensor_attr_tbo;
uniform samplerBuffer tensor_transformation;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform float voxel_factor;

out vec4 frag_color;
out vec3 frag_normal;

const float rad_180 = 3.1415926535897932384626433832795; // 180º
const float rad_90 = 1.570796327; //90º
const float rad_270 = 4.71238898; //270º
const float rad_360 = rad_180 * 2;

void main(void)
{
    float theta = position.x;
    float phi = position.y;

    float cos_theta = cos(position.x);
    float cos_phi = cos(position.y);
    float sin_theta = sin(position.x);
    float sin_phi = sin(position.y);

    float sgn_cos_theta = sign(cos_theta);
    float sgn_sin_theta = sign(sin_theta);
    float sgn_sin_phi = sign(sin_phi);
    float sgn_cos_phi = sign(cos_phi);

    float abs_cos_theta = abs(cos_theta);
    float abs_sin_theta = abs(sin_theta);
    float abs_sin_phi = abs(sin_phi);
    float abs_cos_phi = abs(cos_phi);

    frag_color = texelFetch(glyph_color_tbo, voxel_index);
    vec4 aux = texelFetch(tensor_attr_tbo, voxel_index);
    float alpha = aux.x;
    float beta = aux.y;
    int mode = int(aux.z);

    vec4 tensor_col_01 = texelFetch(tensor_transformation, voxel_index * 4);
    vec4 tensor_col_02 = texelFetch(tensor_transformation, voxel_index * 4 + 1);
    vec4 tensor_col_03 = texelFetch(tensor_transformation, voxel_index * 4 + 2);
    vec4 tensor_col_04 = texelFetch(tensor_transformation, voxel_index * 4 + 3);

    mat4 tensor_new = mat4(tensor_col_01, tensor_col_02, tensor_col_03, tensor_col_04);

    mat4 scale = mat4(voxel_factor, 0.f, 0.f, 0.f,
                      0.f, voxel_factor, 0.f, 0.f,
                      0.f, 0.f, voxel_factor, 0.f,
                      0.f, 0.f, 0.f, 1.f);

    vec4 vertex;
    vec3 normal;

    float cos_theta_alpha_plus_1  = sgn_cos_theta * pow(abs_cos_theta, (alpha + 1));
    float cos_theta_alpha_minus_1 = sgn_cos_theta * pow(abs_cos_theta, (alpha - 1));
    float sin_theta_alpha_plus_1  = sgn_sin_theta * pow(abs_sin_theta, (alpha + 1));
    float sin_theta_alpha_minus_1 = sgn_sin_theta * pow(abs_sin_theta, (alpha - 1));
    float sin_phi_beta_plus_1     = sgn_sin_phi * pow(abs_sin_phi, (beta + 1));
    float cos_phi_beta_minus_1    = sgn_cos_phi * pow(abs_cos_phi, (beta - 1));
    float sin_phi_2beta_minus_1   = sgn_sin_phi * pow(abs_sin_phi, (2 * beta - 1));

    float vertex_term_01 = sgn_cos_theta * pow(abs_cos_theta, alpha) * sgn_sin_phi * pow(abs_sin_phi, beta);
    float vertex_term_02 = sgn_sin_theta * pow(abs_sin_theta, alpha) * sgn_sin_phi * pow(abs_sin_phi, beta);
    float vertex_term_03 = sgn_cos_phi * pow(abs_cos_phi, beta);

    float normal_term_01 = alpha * beta * cos_theta * cos_phi_beta_minus_1 * sin_theta_alpha_minus_1 * sin_phi_beta_plus_1;
    float normal_term_02 = alpha * beta * cos_theta_alpha_minus_1 * cos_phi_beta_minus_1 * sin_theta * sin_phi_beta_plus_1;
    float normal_term_03 = alpha * beta * cos_theta_alpha_plus_1 * cos_phi * sin_theta_alpha_minus_1 * sin_phi_2beta_minus_1 +
                           alpha * beta * cos_theta_alpha_minus_1 * cos_phi * sin_theta_alpha_plus_1 * sin_phi_2beta_minus_1;

    if(mode == 1)
    {
        vertex = vec4(vertex_term_01,
                      vertex_term_02,
                      vertex_term_03, 1.0);

        normal = vec3(0.f);

        normal = vec3(normal_term_01,
                      normal_term_02,
                      normal_term_03);

        if(phi < rad_90)
        {
            if(theta > rad_90 && theta <= rad_180)
                 normal = vec3(normal.x, -normal.y, -normal.z);

            else if(theta > rad_180 && theta <= rad_270)
                 normal = vec3(-normal.x, -normal.y, normal.z);

            else if(theta > rad_270 && theta <= rad_360)
                 normal = vec3(-normal.x, normal.y, -normal.z);

        } else {

            if(theta > 0.0 && theta <= rad_90)
                normal = vec3(-normal.x, -normal.y, normal.z);

            else if(theta > rad_90 && theta <= rad_180)
                 normal = vec3(-normal.x, normal.y, -normal.z);

            else if(theta > rad_180 && theta <= rad_270)
                 normal = vec3(normal.x, normal.y, normal.z);

            else if(theta > rad_270 && theta <= rad_360)
                 normal = vec3(normal.x, -normal.y, -normal.z);

        }

    } else {

        vertex = vec4(vertex_term_03,
                     -vertex_term_02,
                      vertex_term_01, 1.0);

        normal = vec3(0.f);

        normal = vec3(normal_term_03,
                     -normal_term_02,
                      normal_term_01);

        if(phi < rad_90)
        {
            if(theta > rad_90 && theta <= rad_180)
                normal = vec3(-normal.x, -normal.y, normal.z);

            else if(theta > rad_180 && theta <= rad_270)
                normal = vec3(normal.x, -normal.y, -normal.z);

            else if(theta > rad_270 && theta <= rad_360)
                normal = vec3(-normal.x, normal.y, -normal.z);

        } else {

            if(theta > 0.0 && theta <= rad_90)
                normal = vec3(normal.x, -normal.y, -normal.z);

            else if(theta > rad_90 && theta <= rad_180)
                normal = vec3(-normal.x, normal.y, -normal.z);

            else if(theta > rad_270 && theta <= rad_360)
                normal = vec3(-normal.x, -normal.y, normal.z);
        }
    }

    mat4 final_matrix = modelViewMatrix * tensor_new * scale;
    gl_Position = projectionMatrix * final_matrix * vertex;
    frag_normal = normalize(transpose(inverse(mat3(final_matrix))) * (normal));
}
