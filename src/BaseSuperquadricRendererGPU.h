/**
  * @file Superquadric3DRenderGeometryShader.h
  * @author Raphael Voltoline
  * @author Wu Shin-Ting
  * @date 2019
  */
#ifndef BASE_SUPERQUADRIC_RENDERER_GPU_H
#define BASE_SUPERQUADRIC_RENDERER_GPU_H

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#ifdef __WIN32__
#include <GL/glew.h>
#elif __linux__
#include <GL/glew.h>
#elif __APPLE__
#include <glew.h>
#endif

#include <iostream>
#include <glm/glm.hpp>

class VisibleVoxelPreProcessingGeometryShader;
class VisibleVoxelFinalProcessingGeometryShader;
class BaseSuperquadricRendererProgramShader;
class BaseIndirectRenderingCommandProcessing;

/**
 * @ingroup DWI_Volume_Processing
 *
 * @brief  Base classe to render the visible tensor glyphs using only GPU.
 *
 * The procedure is carried out in four steps:
 * VisibleVoxelPreProcessingGeometryShader,
 * VisibleVoxelFinalProcessingGeometryShader,
 * IndirectRenderingCommandProcessing,
 * SuperquadricRendererProgramShader
 *
 * The last two steps are customizable to perform different approaches.
 *
*/
class BaseSuperquadricRendererGPU
{

public:
    /**
     * @brief Constructor of an object of the class BaseSuperquadricRendererGPU.
     *
     * @param volume_dim dimensions of the volume, in voxels.
     * @param scaleFactors scale factors for x-, y- and z-axis.
     */
    BaseSuperquadricRendererGPU(const unsigned short *volume_dim, const float *scaleFactors);

    /**
     * @brief Setup rendering parameters to a Superquadric3DRenderGeometryShader object.
     */
    virtual void initialize();

    /**
     * @brief Render superquadrics on the GPU.
     *
     */
    void render();

    /**
     * @brief update the dimensions of rendering rectangle.
     *
     * @param w width.
     * @param h height.
     */
    virtual void resize(int w, int h);

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

    /**
     * @brief Set the threshold of fractional anisotropy below which the glyphs are not visible.
     *
     * @param fa_threshold threshold.
     */
    void setFaThreshold(float fa_threshold);

    /**
     * @brief Set the 2D texture object id of the rendered visible voxels.
     *
     * @param visible_voxel_texture_id texture object.
     */
    void setVisibleVoxelTextureID(const GLuint &visible_voxel_texture_id);

    /**
     * @brief Set the texture buffer object id of the color-coded FA data
     *
     * @param color_coded_fa_tbo texture buffer object.
     */
    void setColorCodedFaTBO(const GLuint &color_coded_fa_tbo);

    /**
     * @brief Set the texture buffer object id of the tensor-attribute volume (alpha, beta, superquadric mode)
     *
     * @param tensor_attributes_tbo texture buffer object.
     */
    void setTensorAttributesTBO(const GLuint &tensor_attributes_tbo);

    /**
     * @brief Set the texture buffer object id of the transformation volume
     *
     * @param tensor_transformation_tbo texture buffer object.
     */
    void setSuperquadricTransformationTBO(const GLuint &superquadric_transformation_tbo);

protected:

    /**
     * @brief get the texture buffer object id of the color-coded FA volume
     */
    GLuint getColorCodedFaTBO() const;

    /**
     * @brief get the texture buffer object id of the tensor-attribute volume (alpha, beta, superquadric mode)
     */
    GLuint getTensorAttributesTBO() const;

    /**
     * @brief get the texture buffer object id of the transformation volume
     */
    GLuint getSuperquadricTransformationTBO() const;

    /**
     * @brief get the pointer to the vector containing the number of voxels in x-, y- and z-direction
     */
    const unsigned short* getVolumeDim() const;

private:

    /**
     * @brief virtual method to create a custom superquadric renderer
     */
    virtual BaseSuperquadricRendererProgramShader* createSuperquadricRenderer() = 0;

    /**
     * @brief virtual method to create a custom indirect rendering command processor
     */
    virtual BaseIndirectRenderingCommandProcessing* createIndirectRenderingCommandProcessor() = 0;

    GLuint m_vao; /**< vertex array object */

    unsigned short m_volumeDim[3];  /**< number of voxels in x-, y- and z-direction */
    float m_scaleFactors[3];        /**< scale factors in x-, y- and z-direction */
    float m_voxelDim[3];            /**< size of the voxel in x-, y- and z-directions in the texture space */
    bool m_initialized;             /**< initialized (1) or not (0) */

    GLuint m_colorCodedFaTBO;               /**< texture buffer object id of the color-coded FA volume / voxel=(R, G, B, Fa) */
    GLuint m_tensorAttributesTBO;           /**< texture buffer object id of the tensor-attribute volume / voxel=(alpha, beta, superquadric mode) */
    GLuint m_superquadricTransformationTBO; /**< texture buffer object id of the superquadric transformations */
    GLuint m_visibleVoxelTextureID;         /**< 2D texture object id of the rendered visible voxels. */

    VisibleVoxelPreProcessingGeometryShader *m_visibleVoxelPreProcessingGeometryShader;     /**< GPU progam to preprocessing the visible voxel texture */
    VisibleVoxelFinalProcessingGeometryShader *m_visibleVoxelFinalProcessingGeometryShader; /**< GPU program to perform the final processing of the visible voxels indices */

    BaseIndirectRenderingCommandProcessing *m_indirectRenderingCommandProcessing;   /**< Base GPU program to compute the number of visible voxels and the maximum coverage in pixels  */
    BaseSuperquadricRendererProgramShader *m_superquadricRenderer;                  /**< Base GPU program to render the visible superquadrics */
};

#endif // BASE_SUPERQUADRIC_RENDERER_GPU_H
