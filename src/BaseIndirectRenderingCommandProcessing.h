#ifndef BASE_INDIRECT_RENDERING_COMMAND_PROCESSING_H
#define BASE_INDIRECT_RENDERING_COMMAND_PROCESSING_H

/**
  * @def GLEW_STATIC
  * @brief binds the code to the <a href=¨http://glew.sourceforge.net/¨>GLEW</a>´s static library
  */
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

class vmtkManagementShader;

/**
 * @ingroup DWI_Volume_Processing
 *
 * @brief Base class to count the number of visible voxels and the maximum pixels covered by a visible voxel.
 * This data is used to configure a indirect rendering command.
 *
 * The technique Reduce-and-Scan is applied to make counting per block.
 *
 * Shaders to make up the GPU program m_indirectRenderingCommandPreProcessingShaderProgram:
 * (a) Reduce step
 *      Vertex shader: rendering_data_pre_processing.vert;
 *      Geometry shader: rendering_data_pre_processing.geom.
 * (b) Scan step
 *      Geometry shader: A derived class must provide the file path of this shader
 */
class BaseIndirectRenderingCommandProcessing
{
public:
    BaseIndirectRenderingCommandProcessing();

    /**
     * @brief Initialize the OpenGL rendering parameters.
     *
     */
    virtual void initialize();

    /**
     * @brief Execute the GPU programs.
     *
     * @param processed_visible_voxel_texture_id identifier of the texture containing the visible voxels indices.
     */
    virtual void process(unsigned int processed_visible_voxel_texture_id);

    /**
     * @brief get the buffer object containing the data of the indirect rendering command.
     *
     */
    GLuint getRenderingCommandBuffer() const;

    /**
     * @brief Set the file path of the geometry shader to perform the scan step on the GPU.
     *
     * @param scanProcessingGeometryShaderPath
     */
    void setScanProcessingGeometryShaderPath(const std::string &scanProcessingGeometryShaderPath);

    /**
     * @brief get the shader program objeto of the scan step.
     *
     */
    vmtkManagementShader *getScanProcessingShaderProgram() const;

private:
    /**
     * @brief Initialize the OpenGL rendering parameters for the Reduce step.
     *
     */
    void initializeReduceProcessing();

    /**
     * @brief Initialize the OpenGL rendering parameters for the Scan step.
     *
     */
    void initializeScanProcessing();

    /**
     * @brief perform the reduce processing
     *
     * @param processed_visible_voxel_buffer_id identifier of the buffer of visible voxels.
     */
    void doReduceStepProcessing(unsigned int processed_visible_voxel_texture_id);

    /**
     * @brief perform the scan processing
     *
     */
    void doScanStepProcessing();

    GLuint m_renderingDataReduceProcessingTransformFeedbackID;          /**< identifier of Transform Feedback object of the Reduce stage */
    GLuint m_renderingDataReduceProcessingTransformFeedbackBufferID;    /**< identifier of Transform Feedback buffer object of the Reduce stage */

    vmtkManagementShader* m_indirectRenderingCommandReduceProcessingShaderProgram;  /**< GPU program of the reduce processing */
    vmtkManagementShader* m_indirectRenderingCommandScanProcessingShaderProgram;    /**< GPU program of the scan processing */

    GLuint m_renderingDataScanProcessingTransformFeedbackID;        /**< identifier of Transform Feedback object of the Scan stage */
    GLuint m_renderingDataScanProcessingTransformFeedbackBufferID;  /**< identifier of Transform Feedback buffer object of the Scan stage */

    std::string m_scanProcessingGeometryShaderPath; /**< file path of the geometry shader of the Scan stage */

    GLuint m_renderingDataReduceProcessingTextureBufferID; /**< buffer object to store the result of the reduce step */
};

#endif // BASE_INDIRECT_RENDERING_COMMAND_PROCESSING_H
