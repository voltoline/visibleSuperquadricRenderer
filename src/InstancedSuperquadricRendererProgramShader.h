#ifndef INSTANCED_SUPERQUADRIC_RENDERER_PROGRAM_SHADER_H
#define INSTANCED_SUPERQUADRIC_RENDERER_PROGRAM_SHADER_H

#include "BaseSuperquadricRendererProgramShader.h"

class BaseSuperquadricTess;

/**
 * @ingroup DWI_Volume_Processing
 *
 * @brief Derived class of BaseSuperquadricRendererProgramShader
 *
 * Render the superquadric glyphs of the visible voxels using instanced rendering of a tesselated base superquadric (triangle mesh)
 *
 * After the construction of a triangular mesh of the basic individual superquadrics
 * function within each voxel, we apply the idea presented in "Visualizing Diffusion
 * Tensor Images of the Mouse Spinal Cord", by Laidlaw et al., VIS'98, to re-scale uniformly
 * the glyphs with Scale matrix, such that its occupancy in the voxel is maximal. Then,
 * the Rotation is applied to show the principal diffusion directions.
 *
 * To place spatially the superquadrics tensor glyphs inside the corresponding voxels of a
 * scanned DWI volume, we further apply a displacement vector:
 * ((i+0.5)*scale_factor[0],(j+0.5)*scale_factor[1],(k+0.5)*scale_factor[2]), where (i,j,k)
 * are the (integer) coordinates of a voxel relative to the DWI volume.
 *
 * Shaders that meke up the m_visibleVoxelFinalProcessingShaderProgram
 * Vertex shader: superquadric_parallel.vert
 * Geometry shader: superquadric_parallel.frag
 *
 */
class InstancedSuperquadricRendererProgramShader: public BaseSuperquadricRendererProgramShader
{
public:
    InstancedSuperquadricRendererProgramShader(const unsigned short *volume_dim, unsigned int color_coded_fa_tbo, unsigned int tensor_attributes_tbo, unsigned int tensor_transformation_tbo);

    /**
     * @brief Initialize the OpenGL rendering parameters.
     *
     */
    void initialize();

    /**
     * @brief Overrided method to render the superquadric with instanced rendering
     *
     */
    void execute();

    /**
     * @brief set the buffer object containing the data of the indirect rendering command. (overrided method)
     *
     * @param renderingDataBuferID
     */
    void setRenderingCommandBufferID(unsigned int renderingDataBuferID);

private:
    BaseSuperquadricTess *m_baseSuperquadricTess;   /**< GPU program to tesselate the superquadrics parametric function */
    unsigned int m_baseSuperquadricBufferID;        /**< buffer object of the tesselated base superquadrics */
};

#endif // INSTANCED_SUPERQUADRIC_RENDERER_PROGRAM_SHADER_H
