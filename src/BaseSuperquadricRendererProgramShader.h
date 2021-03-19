#ifndef BASE_SUPERQUADRIC_RENDERER_PROGRAM_SHADER_H
#define BASE_SUPERQUADRIC_RENDERER_PROGRAM_SHADER_H

#include <glm/glm.hpp>
#include <iostream>

class vmtkManagementShader;

/**
 * @ingroup DWI_Volume_Processing
 *
 * @brief Base class to render the superquadric glyphs of the visibles voxels.
 *
 * Shaders that meke up the m_visibleVoxelFinalProcessingShaderProgram
 * Vertex shader and Geometry shader: A derived class must provide the file path of these shaders
 *
 */
class BaseSuperquadricRendererProgramShader
{
public:
    BaseSuperquadricRendererProgramShader(const unsigned short *volume_dim, unsigned int color_coded_fa_tbo, unsigned int tensor_attributes_tbo, unsigned int tensor_transformation_tbo);

    /**
     * @brief Initialize the OpenGL rendering parameters.
     *
     */
    virtual void initialize();

    /**
     * @brief Virtual method to render the superquadric.
     *
     */
    virtual void execute() = 0;

    /**
     * @brief set the visible voxels as a buffer object
     *
     * @param visibleVoxelDataBufferID
     */
    virtual void setVisibleVoxelDataBufferID(unsigned int visibleVoxelDataBufferID);

    /**
     * @brief set the buffer object containing the data of the indirect rendering command.
     *
     * @param renderingDataBuferID
     */
    virtual void setRenderingCommandBufferID(unsigned int renderingDataBuferID);

    /**
     * @brief Set the modelview transformation.
     *
     * @param modelView 4x4 transformation matrix.
     */
    void setModelViewMatrix(glm::mat4 modelView);

    /**
     * @brief Set projection matrix.
     *
     * @param projection 4x4 projection matrix.
     */
    void setProjectionMatrix(glm::mat4 projection);

protected:
    /**
     * @brief Set the attibute buffer of glyph tensor instances.
     *
     */
    void setInstancedVertexAttribBuffer();

    vmtkManagementShader* m_superquadric_program; /**< superquadrics rendering program on the GPU */

    std::string m_vertexShaderPath;     /**< file path of the vertex shader */
    std::string m_fragmentShaderPath;   /**< file path of the fragment shader */

    unsigned int m_indirectRenderingCommandBufferID;    /**< buffer object containing the data of the indirect rendering command */

    unsigned int m_colorCodedFaTBO;                 /**< texture buffer object id of the color-coded FA volume / voxel=(R, G, B, Fa) */
    unsigned int m_tensorAttributesTBO;             /**< texture buffer object id of the tensor-attribute volume / voxel=(alpha, beta, superquadric mode) */
    unsigned int m_superquadricTransformationTBO;   /**< texture buffer object id of the superquadric transformations */

    short m_voxelIndexLoc;  /**< location of visible voxel index variable on the GPU */
    short position_loc;     /**< location of position variable on the GPU */

private:
    typedef struct tensor_attr_
    {
        int voxel_index; /**< voxel index */
        int voxel_count; /**< the number of pixels the voxed covered */
    }TensorAttr;

    /**
     * @brief Set parameters of the Blinn-Phong reflection model.
     *
     */
    void setLighting();

    /**
     * @brief Set into the shader program the smallest dimension of the voxel.
     *
     */
    void setVoxelFactor();

    bool m_initialized; /**< initializes (1) or not (0) */

    float m_voxelDim[3];    /**< size of the voxel in x-, y- and z-directions in the texture space */
    float m_voxelFactor;    /**< the smallest dimension of the voxel */

    unsigned int m_visibleVoxelDataBufferID; /**< buffer object containing the visible voxels data (voxel_index, voxel_count) */
};

#endif // BASE_SUPERQUADRIC_RENDERER_PROGRAM_SHADER_H
