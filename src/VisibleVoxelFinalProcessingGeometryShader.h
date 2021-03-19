/**
  * @file VisibleVoxelFinalProcessingGeometryShader.h
  * @author Raphael Voltoline
  * @author Wu Shin-Ting
  * @date 2019
  */
#ifndef VISIBLE_VOXEL_FINAL_PROCESSING_GEOMETRY_SHADER_H
#define VISIBLE_VOXEL_FINAL_PROCESSING_GEOMETRY_SHADER_H

#include "vmtkManagementShader.h"

/**
 * @ingroup DWI_Volume_Processing
 *
 * @brief Perform the final processing of the visible voxels and transfer their data from a Framebuffer object to a Transform Feedback one.
 *
 * During this processing, those indices of voxels with FA below a user-defined threshold are removed.
 *
 * Shaders that meke up the m_visibleVoxelFinalProcessingShaderProgram
 * Vertex shader: visible_voxel_final_processing.vert;
 * Geometry shader: visible_voxel_final_processing.geom.
 *
 */
class VisibleVoxelFinalProcessingGeometryShader
{
public:
    /**
     * @brief Constructor of an object of the class VisibleVoxelFinalProcessingGeometryShader.
     *
     * @param volume_dim[] dimensions of the scanned volume in voxels.
     */
    VisibleVoxelFinalProcessingGeometryShader(unsigned short volume_dim[3]);

    /**
     * @brief Initialize the OpenGL rendering parameters.
     *
     */
    void initialize();

    /**
     * @brief Start the transfering the data from the FBO to the TFO.
     *
     * @param pre_processed_visible_voxel_2d_tex_id identifier of the FBO texture.
     */
    void process(GLuint pre_processed_visible_voxel_2d_tex_id);

    /**
     * @brief Set the size of the rendering rectangle.
     *
     * @param w width of the rectangle.
     * @param h height of the rectangle.
     */
    void resize(int w, int h);

    /**
     * @brief Set the threshold of fractional anisotropy below which the voxels are discarded.
     *
     * @param fa_threshold threshold.
     */
    void setFaThreshold(float fa_threshold);

    /**
     * @brief Set the texture buffer object id of the color-coded FA data
     *
     * @param color_coded_fa_tbo texture buffer object.
     */
    void setColorCodedFaTBO(const GLuint &color_coded_fa_tbo);

    /**
     * @brief get the data of the final visible voxels as a texture object
     *
     */
    GLuint getFinalVisibleVoxelAsTexture() const;

    /**
     * @brief get the data of the final visible voxels as a buffer object
     *
     */
    GLuint getFinalVisibleVoxelAsBuffer() const;

private:
    /**
     * @brief Recompute the number of "pixels" in each block to be processed in parallel with other blocks.
     *
     */
    void recomputeProcessingBlockCoordinate();

    /**
     * @brief Resize the Transform Feedback buffer.
     *
     */
    void resizeTransformFeedbackBuffer();

    /**
     * @brief Pixel coordinates
     *
     */
    typedef struct _coord
    {
        int x; /**< x-coordinate */
        int y; /**< y-coordinate */
    } Coord;

    unsigned short m_width;     /**< width of the rendering rectangle */
    unsigned short m_height;    /**< height of the rendering rectangle */

    int m_volumeDim[3]; /**< dimensions in voxels of the scanned volume */

    unsigned int m_numProcessingBlock;      /**< number of processing blocks */
    Coord *m_processingBlockCoordBuffer;    /**< buffer containing the blocks coordinates to be processed */
    int m_processingBlockRangeWidth;        /**< number of pixels per block in x-direction */
    int m_processingBlockRangeHeight;       /**< number of pixels per block in y-direction */

    GLuint m_processingBlockCoordVBO; /**< vertex buffer object containing the coordinates of blocks */

    bool m_isFirstFrame; /**< is it the first frame (1) or not (0)? */

    GLuint m_colorCodedFaTBO; /**< texture buffer object id of the color-coded FA volume / voxel=(R, G, B, Fa) */

    vmtkManagementShader* m_visibleVoxelFinalProcessingShaderProgram; /**< GPU program */

    GLuint m_finalProcessingTBO;                        /**< texture object containing the final visible voxels */
    GLuint m_finalProcessingTransformFeedbackID;        /**< identifier of transform feedback object */
    GLuint m_finalProcessingTransformFeedbackBufferID;  /**< buffer object containing the final visible voxels */
};

#endif // VISIBLE_VOXEL_FINAL_PROCESSING_GEOMETRY_SHADER_H
