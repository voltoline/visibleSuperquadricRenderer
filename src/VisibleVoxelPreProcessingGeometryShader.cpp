#include "VisibleVoxelPreProcessingGeometryShader.h"

#include "QManagementResources.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

VisibleVoxelPreProcessingGeometryShader::VisibleVoxelPreProcessingGeometryShader(unsigned short volume_dim[3], float voxel_dim[3], const float *scale_factors)
{
    m_volumeDim[0] = volume_dim[0];
    m_volumeDim[1] = volume_dim[1];
    m_volumeDim[2] = volume_dim[2];

    m_voxelDim[0] = voxel_dim[0];
    m_voxelDim[1] = voxel_dim[1];
    m_voxelDim[2] = voxel_dim[2];

    m_scaleFactors[0] = scale_factors[0];
    m_scaleFactors[1] = scale_factors[1];
    m_scaleFactors[2] = scale_factors[2];

    m_visibleVoxelFBO = nullptr;
    m_processingBlockCoordBuffer = nullptr;

    m_processingBlockRangeWidth = 1;
    m_processingBlockRangeHeight = 8;

    m_numProcessingBlock = 0;
}

void VisibleVoxelPreProcessingGeometryShader::initialize()
{
    createCentroidReorientationMatrix();

    QManagementResource *vs = new QManagementResource;
    QManagementResource *gs = new QManagementResource;
    QManagementResource *fs = new QManagementResource;

    GLuint vertex = vs->compileResourceFileShader(QManagementResource::vertex, "./shaders/visible_voxel_preprocessing.vert");
    GLuint geometry = gs->compileResourceFileShader(QManagementResource::geometry, "./shaders/visible_voxel_preprocessing.geom.glsl");
    GLuint fragment = fs->compileResourceFileShader(QManagementResource::fragment, "./shaders/visible_voxel_preprocessing.frag");

    m_visibleVoxelPreProcessingShaderProgram = new vmtkManagementShader;
    m_visibleVoxelPreProcessingShaderProgram->setShader(vertex);
    m_visibleVoxelPreProcessingShaderProgram->setShader(geometry);
    m_visibleVoxelPreProcessingShaderProgram->setShader(fragment);
    m_visibleVoxelPreProcessingShaderProgram->linking();

    delete vs;
    delete gs;
    delete fs;

    glGenBuffers(1, &m_processingBlockCoordVBO);

    m_visibleVoxelPreProcessingShaderProgram->binding();
    {
        GLint volume_dim_unif_loc = glGetUniformLocation(m_visibleVoxelPreProcessingShaderProgram->getProgram(), "volume_dim");
        glUniform3iv(volume_dim_unif_loc, 1, m_volumeDim);

        GLint processing_block_range[2] = {m_processingBlockRangeWidth, m_processingBlockRangeHeight};
        GLint processing_block_range_unif_loc = glGetUniformLocation(m_visibleVoxelPreProcessingShaderProgram->getProgram(), "processing_block_range");
        glUniform2iv(processing_block_range_unif_loc, 1, processing_block_range);

        GLint voxel_dim_unif_loc = glGetUniformLocation(m_visibleVoxelPreProcessingShaderProgram->getProgram(), "voxel_dim");
        glUniform3fv(voxel_dim_unif_loc, 1, m_voxelDim);

        GLint reorient_matrix_loc = glGetUniformLocation(m_visibleVoxelPreProcessingShaderProgram->getProgram(), "reorient_matrix");
        glUniformMatrix4fv(reorient_matrix_loc, 1, GL_FALSE, glm::value_ptr(m_centroidReorientationMatrix));
    }
    m_visibleVoxelPreProcessingShaderProgram->releasing();
}

void VisibleVoxelPreProcessingGeometryShader::process(GLuint visible_voxel_2d_tex_id)
{
    m_visibleVoxelFBO->binding();

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ONE);

    m_visibleVoxelPreProcessingShaderProgram->binding();
    {
        glUniform1i(glGetUniformLocation(m_visibleVoxelPreProcessingShaderProgram->getProgram(), "visible_voxel_tex"), 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, visible_voxel_2d_tex_id);

        GLuint processing_start_coord_loc;
        glBindBuffer(GL_ARRAY_BUFFER, m_processingBlockCoordVBO);
        {
            processing_start_coord_loc = glGetAttribLocation(m_visibleVoxelPreProcessingShaderProgram->getProgram(), "processing_start_coord");
            glVertexAttribIPointer(processing_start_coord_loc, 2, GL_INT, 0, (const void *)0);
            glEnableVertexAttribArray(processing_start_coord_loc);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_POINTS, 0, m_numProcessingBlock);

        glDisableVertexAttribArray(processing_start_coord_loc);
    }
    m_visibleVoxelPreProcessingShaderProgram->releasing();

    glDisable(GL_BLEND);

    m_visibleVoxelFBO->releasing();
}

void VisibleVoxelPreProcessingGeometryShader::resize(int w, int h)
{
    m_width = w;
    m_height = h;

    if(m_visibleVoxelFBO != nullptr)
        delete m_visibleVoxelFBO;

    m_visibleVoxelFBO = new vmtkFrameBufferObject(w, h);

    recomputeProcessingBlockCoordinate();
}

void VisibleVoxelPreProcessingGeometryShader::setProjectionMatrix(glm::mat4 projection)
{
    m_visibleVoxelPreProcessingShaderProgram->binding();
    GLshort projectionMatrixLoc = glGetUniformLocation(m_visibleVoxelPreProcessingShaderProgram->getProgram(), "projection_matrix");
    glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projection));
    m_visibleVoxelPreProcessingShaderProgram->releasing();
}

void VisibleVoxelPreProcessingGeometryShader::setModelViewMatrix(glm::mat4 modelView)
{
    m_visibleVoxelPreProcessingShaderProgram->binding();
    GLshort mvMatrixLoc = glGetUniformLocation(m_visibleVoxelPreProcessingShaderProgram->getProgram(), "model_view_matrix");
    glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelView));
    m_visibleVoxelPreProcessingShaderProgram->releasing();
}

void VisibleVoxelPreProcessingGeometryShader::recomputeProcessingBlockCoordinate()
{
    unsigned short num_block_horizontal = std::ceil(m_width / ((float) m_processingBlockRangeWidth));
    unsigned short num_block_vertical = std::ceil(m_height / ((float) m_processingBlockRangeHeight));

    m_numProcessingBlock = num_block_horizontal * num_block_vertical;

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

    glBindBuffer(GL_ARRAY_BUFFER, m_processingBlockCoordVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Coord) * m_numProcessingBlock, m_processingBlockCoordBuffer, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VisibleVoxelPreProcessingGeometryShader::createCentroidReorientationMatrix()
{
    m_centroidReorientationMatrix = glm::scale(glm::mat4(1.f), glm::vec3(2.f * m_scaleFactors[0] , 2.f * m_scaleFactors[1], 2.f * m_scaleFactors[2]));
    m_centroidReorientationMatrix = glm::translate(m_centroidReorientationMatrix, glm::vec3(-0.5f, -0.5f, -0.5f));
}

vmtkFrameBufferObject *VisibleVoxelPreProcessingGeometryShader::getVisibleVoxelFBO() const
{
    return m_visibleVoxelFBO;
}
