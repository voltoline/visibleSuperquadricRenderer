#include "InstancedSuperquadricRendererProgramShader.h"

#include "BaseSuperquadricTess.h"

#define BUFFER_OFFSET(x)  ((const void*) (x))

InstancedSuperquadricRendererProgramShader::InstancedSuperquadricRendererProgramShader(const unsigned short *volume_dim, unsigned int color_coded_fa_tbo, unsigned int tensor_attributes_tbo, unsigned int tensor_transformation_tbo):
    BaseSuperquadricRendererProgramShader(volume_dim, color_coded_fa_tbo, tensor_attributes_tbo, tensor_transformation_tbo)
{
    m_vertexShaderPath = "./shaders/superquadric_parallel.vert";
    m_fragmentShaderPath = "./shaders/superquadric_parallel.frag";
}

void InstancedSuperquadricRendererProgramShader::initialize()
{
    BaseSuperquadricRendererProgramShader::initialize();

    m_baseSuperquadricTess = new BaseSuperquadricTess();
    m_baseSuperquadricTess->initialize();

    m_baseSuperquadricBufferID = m_baseSuperquadricTess->getBaseSuperquadricBufferId();
}

void InstancedSuperquadricRendererProgramShader::execute()
{
    m_baseSuperquadricTess->compute();

    m_superquadric_program->binding();

    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    setInstancedVertexAttribBuffer();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, m_colorCodedFaTBO);
    glUniform1i(glGetUniformLocation(m_superquadric_program->getProgram(), "glyph_color_tbo"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, m_tensorAttributesTBO);
    glUniform1i(glGetUniformLocation(m_superquadric_program->getProgram(), "tensor_attr_tbo"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_BUFFER, m_superquadricTransformationTBO);
    glUniform1i(glGetUniformLocation(m_superquadric_program->getProgram(), "tensor_transformation"), 2);

    glBindBuffer(GL_ARRAY_BUFFER, m_baseSuperquadricBufferID);
    {
        glVertexAttribPointer(position_loc, 2, GL_FLOAT, false, 0, BUFFER_OFFSET(0));
        glEnableVertexAttribArray((GLuint)position_loc);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_indirectRenderingCommandBufferID);
    glDrawArraysIndirect(GL_TRIANGLES, BUFFER_OFFSET(sizeof(GLuint)));

    glVertexAttribDivisor((GLuint)m_voxelIndexLoc, 0);

    glDisable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);

    glBindVertexArray(0);
    m_superquadric_program->releasing();
}

void InstancedSuperquadricRendererProgramShader::setRenderingCommandBufferID(unsigned int renderingDataBuferID)
{
    BaseSuperquadricRendererProgramShader::setRenderingCommandBufferID(renderingDataBuferID);

    m_baseSuperquadricTess->setBaseSuperquadricInfoUniformBlockBufferId(renderingDataBuferID);
}
