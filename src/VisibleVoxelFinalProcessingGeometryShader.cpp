#include "VisibleVoxelFinalProcessingGeometryShader.h"

#include "QManagementResources.h"
#include <cmath>

VisibleVoxelFinalProcessingGeometryShader::VisibleVoxelFinalProcessingGeometryShader(unsigned short volume_dim[])
{
    m_volumeDim[0] = volume_dim[0];
    m_volumeDim[1] = volume_dim[1];
    m_volumeDim[2] = volume_dim[2];

    m_processingBlockRangeWidth = 1;
    m_processingBlockRangeHeight = 8;

    m_numProcessingBlock = 0;

    m_processingBlockCoordBuffer = nullptr;
    m_isFirstFrame = true;
}

void VisibleVoxelFinalProcessingGeometryShader::initialize()
{
    QManagementResource *vs = new QManagementResource;
    QManagementResource *gs = new QManagementResource;

    GLuint vertex = vs->compileResourceFileShader(QManagementResource::vertex, "./shaders/visible_voxel_final_processing.vert");
    GLuint geometry = gs->compileResourceFileShader(QManagementResource::geometry, "./shaders/visible_voxel_final_processing.geom.glsl");

    m_visibleVoxelFinalProcessingShaderProgram = new vmtkManagementShader;
    m_visibleVoxelFinalProcessingShaderProgram->setShader(vertex);
    m_visibleVoxelFinalProcessingShaderProgram->setShader(geometry);
    m_visibleVoxelFinalProcessingShaderProgram->linking();

    const char *varying_names_filter[] = {"voxel_index", "pixel_count"};
    glTransformFeedbackVaryings(m_visibleVoxelFinalProcessingShaderProgram->getProgram(), 2, varying_names_filter, GL_INTERLEAVED_ATTRIBS);

    glLinkProgram(m_visibleVoxelFinalProcessingShaderProgram->getProgram());

    delete vs;
    delete gs;

    glGenBuffers(1, &m_processingBlockCoordVBO);

    glGenTransformFeedbacks(1, &m_finalProcessingTransformFeedbackID);
    glGenBuffers(1, &m_finalProcessingTransformFeedbackBufferID);

    glGenTextures(1, &m_finalProcessingTBO);
    glBindTexture(GL_TEXTURE_BUFFER, m_finalProcessingTBO);
    glBindBuffer(GL_TEXTURE_BUFFER, m_finalProcessingTransformFeedbackBufferID);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RG32I, m_finalProcessingTransformFeedbackBufferID);

    glBindTexture(GL_TEXTURE_BUFFER, 0);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    m_visibleVoxelFinalProcessingShaderProgram->binding();
    {
        GLint volume_dim_unif_loc = glGetUniformLocation(m_visibleVoxelFinalProcessingShaderProgram->getProgram(), "volume_dim");
        glUniform3iv(volume_dim_unif_loc, 1, m_volumeDim);

        GLfloat fa_threshold = 0;
        GLint fa_threshold_unif_loc = glGetUniformLocation(m_visibleVoxelFinalProcessingShaderProgram->getProgram(), "fa_threshold");
        glUniform1f(fa_threshold_unif_loc, fa_threshold);

        GLint processing_block_range[2] = {m_processingBlockRangeWidth, m_processingBlockRangeHeight};
        GLint processing_block_range_unif_loc = glGetUniformLocation(m_visibleVoxelFinalProcessingShaderProgram->getProgram(), "processing_block_range");
        glUniform2iv(processing_block_range_unif_loc, 1, processing_block_range);

        GLint last_processing_block_coord[2] = {0, 0};
        GLint last_processing_block_coord_unif_loc = glGetUniformLocation(m_visibleVoxelFinalProcessingShaderProgram->getProgram(), "last_processing_block_coord");
        glUniform2iv(last_processing_block_coord_unif_loc, 1, last_processing_block_coord);
    }
    m_visibleVoxelFinalProcessingShaderProgram->releasing();
}

void VisibleVoxelFinalProcessingGeometryShader::process(GLuint pre_processed_visible_voxel_2d_tex_id)
{
    m_visibleVoxelFinalProcessingShaderProgram->binding();
    {
        glUniform1i(glGetUniformLocation(m_visibleVoxelFinalProcessingShaderProgram->getProgram(), "visible_voxel_tex"), 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, pre_processed_visible_voxel_2d_tex_id);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_BUFFER, m_colorCodedFaTBO);
        glUniform1i(glGetUniformLocation(m_visibleVoxelFinalProcessingShaderProgram->getProgram(), "fa_buffer"), 5);


        GLuint processing_start_coord_loc;

        glBindBuffer(GL_ARRAY_BUFFER, m_processingBlockCoordVBO);
        {
            processing_start_coord_loc = glGetAttribLocation(m_visibleVoxelFinalProcessingShaderProgram->getProgram(), "processing_start_coord");
            glVertexAttribIPointer(processing_start_coord_loc, 2, GL_INT, 0, (const void *)0);
            glEnableVertexAttribArray(processing_start_coord_loc);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if(m_isFirstFrame)
        {
            m_isFirstFrame = false;

            glEnable(GL_RASTERIZER_DISCARD);

            glDrawArrays(GL_POINTS, 0, m_numProcessingBlock);

            glDisable(GL_RASTERIZER_DISCARD);

        }

        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_finalProcessingTransformFeedbackID);
        {
            glEnable(GL_RASTERIZER_DISCARD);
            {
                glBeginTransformFeedback(GL_POINTS);

                glDrawArrays(GL_POINTS, 0, m_numProcessingBlock);

                glEndTransformFeedback();
            }
            glDisable(GL_RASTERIZER_DISCARD);

        }
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        glDisableVertexAttribArray(processing_start_coord_loc);
    }
    m_visibleVoxelFinalProcessingShaderProgram->releasing();
}

void VisibleVoxelFinalProcessingGeometryShader::resize(int w, int h)
{
    m_width = w;
    m_height = h;

    recomputeProcessingBlockCoordinate();
    resizeTransformFeedbackBuffer();
}

void VisibleVoxelFinalProcessingGeometryShader::setFaThreshold(float fa_threshold)
{
    m_visibleVoxelFinalProcessingShaderProgram->binding();
    {
        GLint fa_threshold_unif_loc = glGetUniformLocation(m_visibleVoxelFinalProcessingShaderProgram->getProgram(), "fa_threshold");
        glUniform1f(fa_threshold_unif_loc, fa_threshold);
    }
    m_visibleVoxelFinalProcessingShaderProgram->releasing();
}

void VisibleVoxelFinalProcessingGeometryShader::recomputeProcessingBlockCoordinate()
{
    unsigned short num_block_horizontal = std::ceil(m_width / ((float) m_processingBlockRangeWidth));
    unsigned short num_block_vertical = std::ceil(m_height / ((float) m_processingBlockRangeHeight));

    m_numProcessingBlock = num_block_horizontal * num_block_vertical + 1;

    if(m_processingBlockCoordBuffer != nullptr)
        delete[] m_processingBlockCoordBuffer;

    m_processingBlockCoordBuffer = new Coord[m_numProcessingBlock];
    unsigned int block_count = 0;

    for(unsigned short j = 0; j < num_block_vertical; j++)
    {
        for(unsigned short i = 0; i < num_block_horizontal; i++)
        {
            m_processingBlockCoordBuffer[block_count].x = i * m_processingBlockRangeWidth;
            m_processingBlockCoordBuffer[block_count].y = j * m_processingBlockRangeHeight;
            block_count++;
        }
    }

    m_processingBlockCoordBuffer[block_count].x = m_width;
    m_processingBlockCoordBuffer[block_count].y = m_height;

    m_visibleVoxelFinalProcessingShaderProgram->binding();
    {
        GLint last_processing_block_coord[2] = {m_processingBlockCoordBuffer[block_count].x, m_processingBlockCoordBuffer[block_count].y};
        GLint last_processing_block_coord_unif_loc = glGetUniformLocation(m_visibleVoxelFinalProcessingShaderProgram->getProgram(), "last_processing_block_coord");
        glUniform2iv(last_processing_block_coord_unif_loc, 1, last_processing_block_coord);
    }
    m_visibleVoxelFinalProcessingShaderProgram->releasing();

    glBindBuffer(GL_ARRAY_BUFFER, m_processingBlockCoordVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Coord) * m_numProcessingBlock, m_processingBlockCoordBuffer, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VisibleVoxelFinalProcessingGeometryShader::resizeTransformFeedbackBuffer()
{
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_finalProcessingTransformFeedbackID);
    glBindBuffer(GL_ARRAY_BUFFER, m_finalProcessingTransformFeedbackBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * 2 * m_height * m_width, nullptr, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_finalProcessingTransformFeedbackBufferID);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint VisibleVoxelFinalProcessingGeometryShader::getFinalVisibleVoxelAsBuffer() const
{
    return m_finalProcessingTransformFeedbackBufferID;
}

GLuint VisibleVoxelFinalProcessingGeometryShader::getFinalVisibleVoxelAsTexture() const
{
    return m_finalProcessingTBO;
}

void VisibleVoxelFinalProcessingGeometryShader::setColorCodedFaTBO(const GLuint &color_coded_fa_tbo)
{
    m_colorCodedFaTBO = color_coded_fa_tbo;
}
