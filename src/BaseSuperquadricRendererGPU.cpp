#include "BaseSuperquadricRendererGPU.h"

#include <cstdlib>
#include "VisibleVoxelPreProcessingGeometryShader.h"
#include "VisibleVoxelFinalProcessingGeometryShader.h"
#include "BaseIndirectRenderingCommandProcessing.h"
#include "BaseSuperquadricRendererProgramShader.h"

BaseSuperquadricRendererGPU::BaseSuperquadricRendererGPU(const unsigned short *volume_dim, const float *scaleFactors):
    m_vao(0)
{
    m_volumeDim[0] = volume_dim[0];
    m_volumeDim[1] = volume_dim[1];
    m_volumeDim[2] = volume_dim[2];

    m_scaleFactors[0] = scaleFactors[0];
    m_scaleFactors[1] = scaleFactors[1];
    m_scaleFactors[2] = scaleFactors[2];

    m_voxelDim[0] = 1.f / m_volumeDim[0];
    m_voxelDim[1] = 1.f / m_volumeDim[1];
    m_voxelDim[2] = 1.f / m_volumeDim[2];

    m_initialized = false;

    m_superquadricRenderer = nullptr;

    m_indirectRenderingCommandProcessing = nullptr;
}

void BaseSuperquadricRendererGPU::initialize(){

    if(!m_initialized)
    {
        m_initialized = true;

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        m_visibleVoxelPreProcessingGeometryShader = new VisibleVoxelPreProcessingGeometryShader(m_volumeDim, m_voxelDim, m_scaleFactors);
        m_visibleVoxelPreProcessingGeometryShader->initialize();

        m_visibleVoxelFinalProcessingGeometryShader = new VisibleVoxelFinalProcessingGeometryShader(m_volumeDim);
        m_visibleVoxelFinalProcessingGeometryShader->initialize();
        m_visibleVoxelFinalProcessingGeometryShader->setColorCodedFaTBO(m_colorCodedFaTBO);

        m_indirectRenderingCommandProcessing = createIndirectRenderingCommandProcessor();
        m_indirectRenderingCommandProcessing->initialize();

        m_superquadricRenderer = createSuperquadricRenderer();
        m_superquadricRenderer->initialize();

        m_superquadricRenderer->setVisibleVoxelDataBufferID(m_visibleVoxelFinalProcessingGeometryShader->getFinalVisibleVoxelAsBuffer());
        m_superquadricRenderer->setRenderingCommandBufferID(m_indirectRenderingCommandProcessing->getRenderingCommandBuffer());

        glBindVertexArray(0);
    }
}

void BaseSuperquadricRendererGPU::render()
{
    if(!m_initialized)
    {
        std::cerr << "DiffLinesRenderTexCoordFBO::renderTexCoord ->> It was not initialized!" << std::endl;
        initialize();
    }

    glBindVertexArray(m_vao);

    m_visibleVoxelPreProcessingGeometryShader->process(m_visibleVoxelTextureID);

    m_visibleVoxelFinalProcessingGeometryShader->process(m_visibleVoxelPreProcessingGeometryShader->getVisibleVoxelFBO()->getTexture());

    m_indirectRenderingCommandProcessing->process(m_visibleVoxelFinalProcessingGeometryShader->getFinalVisibleVoxelAsTexture());

    m_superquadricRenderer->execute();

    glBindVertexArray(0);
}

void BaseSuperquadricRendererGPU::setFaThreshold(float fa_threshold)
{
    if(m_initialized)
        m_visibleVoxelFinalProcessingGeometryShader->setFaThreshold(fa_threshold);
}

void BaseSuperquadricRendererGPU::resize(int w, int h)
{
    m_visibleVoxelPreProcessingGeometryShader->resize(w, h);
    m_visibleVoxelFinalProcessingGeometryShader->resize(w, h);
}

void BaseSuperquadricRendererGPU::setModelViewMatrix(glm::mat4 modelView)
{
    if(m_initialized)
    {
        m_superquadricRenderer->setModelViewMatrix(modelView);

        m_visibleVoxelPreProcessingGeometryShader->setModelViewMatrix(modelView);
    }
}

void BaseSuperquadricRendererGPU::setProjectionMatrix(glm::mat4 projection)
{
    if(m_initialized)
    {
        m_superquadricRenderer->setProjectionMatrix(projection);

        m_visibleVoxelPreProcessingGeometryShader->setProjectionMatrix(projection);
    }
}

void BaseSuperquadricRendererGPU::setVisibleVoxelTextureID(const GLuint &visible_voxel_texture_id)
{
    m_visibleVoxelTextureID = visible_voxel_texture_id;
}

void BaseSuperquadricRendererGPU::setColorCodedFaTBO(const GLuint &color_coded_fa_tbo)
{
    m_colorCodedFaTBO = color_coded_fa_tbo;
}

void BaseSuperquadricRendererGPU::setTensorAttributesTBO(const GLuint &tensor_attributes_tbo)
{
    m_tensorAttributesTBO = tensor_attributes_tbo;
}

void BaseSuperquadricRendererGPU::setSuperquadricTransformationTBO(const GLuint &superquadric_transformation_tbo)
{
    m_superquadricTransformationTBO = superquadric_transformation_tbo;
}

GLuint BaseSuperquadricRendererGPU::getColorCodedFaTBO() const
{
    return m_colorCodedFaTBO;
}

GLuint BaseSuperquadricRendererGPU::getTensorAttributesTBO() const
{
    return m_tensorAttributesTBO;
}

GLuint BaseSuperquadricRendererGPU::getSuperquadricTransformationTBO() const
{
    return m_superquadricTransformationTBO;
}

const unsigned short *BaseSuperquadricRendererGPU::getVolumeDim() const
{
    return m_volumeDim;
}
