#version 400

in vec4 frag_color;
in vec3 frag_normal;

uniform vec3 Ambient;
uniform vec3 LightColor;
uniform vec3 LightDirection; // direction toward the light
uniform vec3 HalfVector; // surface orientation for shiniest spots
uniform float Shininess; // exponent for sharping highlights
uniform float Strength; // extra factor to adjust shininess

layout (location = 0) out vec4 out_color;

void main()
{
    vec3 normal = normalize(frag_normal);
    // compute cosine of the directions, using dot products,
    // to see how much light would be reflected
    float diffuse = max(0.0, dot(normal, LightDirection));
    float specular = max(0.0, dot(normal, HalfVector));
    // surfaces facing away from the light (negative dot products)
    // won’t be lit by the directional light
    if (diffuse == 0.0)
        specular = 0.0;
    else
        specular = pow(specular, Shininess); // sharpen the highlight

    vec3 scatteredLight = Ambient + LightColor * diffuse;
    vec3 reflectedLight = LightColor * specular * Strength;
    // don’t modulate the underlying color with reflected light,
    // only with scattered light
    vec3 rgb = min(frag_color.rgb * scatteredLight + reflectedLight, vec3(1.0));
    out_color = vec4(rgb, frag_color.a);
}


