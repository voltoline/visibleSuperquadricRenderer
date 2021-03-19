#version 400

in ivec2 processing_start_coord;

out ivec2 processing_start_coord_gs;

void main()
{
    processing_start_coord_gs = processing_start_coord;
}
