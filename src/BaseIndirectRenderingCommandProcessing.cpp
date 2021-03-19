#include "BaseIndirectRenderingCommandProcessing.h"

#include "QManagementResources.h"
#include "vmtkManagementShader.h"

BaseIndirectRenderingCommandProcessing::BaseIndirectRenderingCommandProcessing()
{
    m_scanProcessingGeometryShaderPath = "";
}

void BaseIndirectRenderingCommandProcessing::initialize()
{
    initializeReduceProcessing();
    initializeScanProcessing();
}

void BaseIndirectRenderingCommandProcessing::initializeReduceProcessing()
{
    QManagementResource *vs = new QManagementResource;
    QManagementResource *gs = new QManagementResource;

    GLuint vertex = vs->compileResourceFileShader(QManagementResource::vertex, "./shaders/rendering_data_pre_processing.vert");
    GLuint geometry = gs->compileResourceFileShader(QManagementResource::geometry, "./shaders/rendering_data_pre_processing.geom.glsl");

    m_indirectRenderingCommandReduceProcessingShaderProgram = new vmtkManagementShader;
    m_indirectRenderingCommandReduceProcessingShaderProgram->setShader(vertex);
    m_indirectRenderingCommandReduceProcessingShaderProgram->setShader(geometry);
    m_indirectRenderingCommandReduceProcessingShaderProgram->linking();

    const char *varying_names_rendering_data_pre_processing[] = {"num_visible_voxels", "max_voxel_pixel"};
    glTransformFeedbackVaryings(m_indirectRenderingCommandReduceProcessingShaderProgram->getProgram(), 2, varying_names_rendering_data_pre_processing, GL_INTERLEAVED_ATTRIBS);

    glLinkProgram(m_indirectRenderingCommandReduceProcessingShaderProgram->getProgram());

    delete vs;
    delete gs;

    glGenTransformFeedbacks(1, &m_renderingDataReduceProcessingTransformFeedbackID);
    glGenBuffers(1, &m_renderingDataReduceProcessingTransformFeedbackBufferID);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_renderingDataReduceProcessingTransformFeedbackID);
    glBindBuffer(GL_ARRAY_BUFFER, m_renderingDataReduceProcessingTransformFeedbackBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * 2 * 10, nullptr, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_renderingDataReduceProcessingTransformFeedbackBufferID);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BaseIndirectRenderingCommandProcessing::initializeScanProcessing()
{
    QManagementResource *vs = new QManagementResource;
    QManagementResource *gs = new QManagementResource;

    GLuint vertex = vs->compileResourceFileShader(QManagementResource::vertex, "./shaders/rendering_data_final_processing.vert");
    GLuint geometry = gs->compileResourceFileShader(QManagementResource::geometry, m_scanProcessingGeometryShaderPath);

    m_indirectRenderingCommandScanProcessingShaderProgram = new vmtkManagementShader;
    m_indirectRenderingCommandScanProcessingShaderProgram->setShader(vertex);
    m_indirectRenderingCommandScanProcessingShaderProgram->setShader(geometry);
    m_indirectRenderingCommandScanProcessingShaderProgram->linking();

    const char *varying_names_rendering_data[] = {"rendering_data"};
    glTransformFeedbackVaryings(m_indirectRenderingCommandScanProcessingShaderProgram->getProgram(), 1, varying_names_rendering_data, GL_INTERLEAVED_ATTRIBS);

    glLinkProgram(m_indirectRenderingCommandScanProcessingShaderProgram->getProgram());

    delete vs;
    delete gs;

    glGenTransformFeedbacks(1, &m_renderingDataScanProcessingTransformFeedbackID);
    glGenBuffers(1, &m_renderingDataScanProcessingTransformFeedbackBufferID);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_renderingDataScanProcessingTransformFeedbackID);
    glBindBuffer(GL_ARRAY_BUFFER, m_renderingDataScanProcessingTransformFeedbackBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * 5, nullptr, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_renderingDataScanProcessingTransformFeedbackBufferID);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenTextures(1, &m_renderingDataReduceProcessingTextureBufferID);
    glBindTexture(GL_TEXTURE_BUFFER, m_renderingDataReduceProcessingTextureBufferID);
    glBindBuffer(GL_TEXTURE_BUFFER, m_renderingDataReduceProcessingTransformFeedbackBufferID);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RG32I, m_renderingDataReduceProcessingTransformFeedbackBufferID);

    glBindTexture(GL_TEXTURE_BUFFER, 0);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

void BaseIndirectRenderingCommandProcessing::process(unsigned int processed_visible_voxel_texture_id)
{
    doReduceStepProcessing(processed_visible_voxel_texture_id);
    doScanStepProcessing();
}

void BaseIndirectRenderingCommandProcessing::doReduceStepProcessing(unsigned int processed_visible_voxel_texture_id)
{
    m_indirectRenderingCommandReduceProcessingShaderProgram->binding();
    {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_BUFFER, processed_visible_voxel_texture_id);
        glUniform1i(glGetUniformLocation(m_indirectRenderingCommandReduceProcessingShaderProgram->getProgram(), "visible_voxel_buffer"), 4);

        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_renderingDataReduceProcessingTransformFeedbackID);
        {
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_renderingDataReduceProcessingTransformFeedbackBufferID);
            {
                glEnable(GL_RASTERIZER_DISCARD);
                {
                    glBeginTransformFeedback(GL_POINTS);

                    glDrawArrays(GL_POINTS, 0, 10);

                    glEndTransformFeedback();
                }
                glDisable(GL_RASTERIZER_DISCARD);
            }
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
        }
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

    }
    m_indirectRenderingCommandReduceProcessingShaderProgram->releasing();
}

void BaseIndirectRenderingCommandProcessing::doScanStepProcessing()
{
    m_indirectRenderingCommandScanProcessingShaderProgram->binding();
    {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_BUFFER, m_renderingDataReduceProcessingTextureBufferID);
        glUniform1i(glGetUniformLocation(m_indirectRenderingCommandScanProcessingShaderProgram->getProgram(), "rendering_data_buffer"), 4);

        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_renderingDataScanProcessingTransformFeedbackID);
        {
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_renderingDataScanProcessingTransformFeedbackBufferID);
            {
                glEnable(GL_RASTERIZER_DISCARD);
                {
                    glBeginTransformFeedback(GL_POINTS);

                    glDrawArrays(GL_POINTS, 0, 1);

                    glEndTransformFeedback();
                }
                glDisable(GL_RASTERIZER_DISCARD);
            }
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
        }
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

    }
    m_indirectRenderingCommandScanProcessingShaderProgram->releasing();
}

vmtkManagementShader *BaseIndirectRenderingCommandProcessing::getScanProcessingShaderProgram() const
{
    return m_indirectRenderingCommandScanProcessingShaderProgram;
}

GLuint BaseIndirectRenderingCommandProcessing::getRenderingCommandBuffer() const
{
    return m_renderingDataScanProcessingTransformFeedbackBufferID;
}

void BaseIndirectRenderingCommandProcessing::setScanProcessingGeometryShaderPath(const std::string &scanProcessingGeometryShaderPath)
{
    m_scanProcessingGeometryShaderPath = scanProcessingGeometryShaderPath;
}
