/**
  * @file VisibleVoxelPreProcessingGeometryShader.h
  * @author Raphael Voltoline
  * @author Wu Shin-Ting
  * @date 2019
  */
#ifndef VISIBLE_VOXEL_PRE_PROCESSING_GEOMETRY_SHADER_H
#define VISIBLE_VOXEL_PRE_PROCESSING_GEOMETRY_SHADER_H

#include "vmtkManagementShader.h"
#include "vmtkFrameBufferObject.h"
#include <glm/glm.hpp>

/**
 * @ingroup DWI_Volume_Processing
 *
 * @brief Preprocessing the visible voxels and compute its coverage on the GPU.
 *
 * The index of each visible voxels and the number of pixels it occupied are stored in a Framebuffer object.
 *
 * Shaders that make up the m_visibleVoxelPreProcessingGeometryShader:
 * Vertex shader: visible_voxel_preprocessing.vert;
 * Geometry shader: visible_voxel_preprocessing.geom;
 * Fragment shader: visible_voxel_preprocessing.frag.
 */
class VisibleVoxelPreProcessingGeometryShader
{
public:
    /**
     * @brief Constructor of an object of the class VisibleVoxelPreProcessingGeometryShader. 
     *
     * @param volume_dim[] dimensions of the volume, in voxels.
     * @param voxel_dim[] size of the voxel in the texture space
     * @param scale_factors scale factors in x-, y- and z-direction
     */
    VisibleVoxelPreProcessingGeometryShader(unsigned short volume_dim[3], float voxel_dim[3], const float *scale_factors);

    /**
     * @brief Initialize the rendering parameters.
     *
     */
    void initialize();

    /**
     * @brief Process the buffer of visible voxels.
     *
     * @param visible_voxel_2d_tex_id
     */
    void process(GLuint visible_voxel_2d_tex_id);

    /**
     * @brief Set the dimensions of the rendering rectangle.
     *
     * @param w width of the rectangle.
     * @param h height of the rectangle.
     */
    void resize(int w, int h);

    /**
     * @brief Set the projection matrix.
     *
     * @param projection projection 4x4 matrix.
     */
    void setProjectionMatrix(glm::mat4 projection);

    /**
     * @brief Set modelview matrix.
     *
     * @param modelView modelview 4x4 matrix.
     */
    void setModelViewMatrix(glm::mat4 modelView);

    /**
     * @brief get the framebuffer object of the preprocessed visible voxels.
     *
     */
    vmtkFrameBufferObject *getVisibleVoxelFBO() const;

private:
    /**
     * @brief Recompute the number of "pixels" in each block to be processed in parallel with other blocks.
     *
     */
    void recomputeProcessingBlockCoordinate();

    /**
     * @brief Scale the base geometry of the a tensor glyph.
     *
     */
    void createCentroidReorientationMatrix();

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

    int m_volumeDim[3];         /**< dimensions in voxels of the scanned volume */
    float m_voxelDim[3];        /**< size of the voxel in x-, y- and z-directions in the texture space */
    float m_scaleFactors[3];    /**< scale factors in x-, y- and z-direction */

    unsigned int m_numProcessingBlock;      /**< number of processing blocks */
    Coord *m_processingBlockCoordBuffer;    /**< buffer containing the blocks coordinates to be processed */
    int m_processingBlockRangeWidth;        /**< number of pixels in x-direction in each block */
    int m_processingBlockRangeHeight;       /**< number of pixels in y-direction in each block */

    GLuint m_processingBlockCoordVBO;     /**< vertex buffer object containing the coordinates of blocks */

    glm::mat4 m_centroidReorientationMatrix;    /**< geometric transformation matrix to compute the positiion of the voxel centroid */

    vmtkManagementShader* m_visibleVoxelPreProcessingShaderProgram; /**< GPU program */
    vmtkFrameBufferObject *m_visibleVoxelFBO;                       /**< framebuffer object containing the index of each visible voxel and the number of pixels it covered */
};

#endif // VISIBLE_VOXEL_PRE_PROCESSING_GEOMETRY_SHADER_H
