#include "BaseSuperquadricRendererProgramShader.h"

#include "QManagementResources.h"
#include "vmtkManagementShader.h"
#include <glm/gtc/type_ptr.hpp>

BaseSuperquadricRendererProgramShader::BaseSuperquadricRendererProgramShader(const unsigned short *volume_dim, unsigned int color_coded_fa_tbo, unsigned int tensor_attributes_tbo, unsigned int tensor_transformation_tbo)
{
    m_voxelDim[0] = 1.f / volume_dim[0];
    m_voxelDim[1] = 1.f / volume_dim[1];
    m_voxelDim[2] = 1.f / volume_dim[2];

    m_initialized = false;

    m_colorCodedFaTBO = color_coded_fa_tbo;
    m_tensorAttributesTBO = tensor_attributes_tbo;
    m_superquadricTransformationTBO = tensor_transformation_tbo;

    m_vertexShaderPath = "";
    m_fragmentShaderPath = "";
}

void BaseSuperquadricRendererProgramShader::initialize()
{
    if(!m_initialized)
    {
        m_initialized = true;

        QManagementResource *vs = new QManagementResource;
        QManagementResource *fs = new QManagementResource;

        GLuint vertex = vs->compileResourceFileShader(QManagementResource::vertex, m_vertexShaderPath);
        GLuint fragment = fs->compileResourceFileShader(QManagementResource::fragment, m_fragmentShaderPath);
        m_superquadric_program = new vmtkManagementShader;
        m_superquadric_program->setShader(vertex);
        m_superquadric_program->setShader(fragment);
        m_superquadric_program->linking();

        delete vs;
        delete fs;

        m_superquadric_program->binding();

        position_loc = glGetAttribLocation(m_superquadric_program->getProgram(), "position");
        m_voxelIndexLoc = glGetAttribLocation(m_superquadric_program->getProgram(), "voxel_index");

        setLighting();
        setVoxelFactor();

        m_superquadric_program->releasing();
    }
}

void BaseSuperquadricRendererProgramShader::setInstancedVertexAttribBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_visibleVoxelDataBufferID);
    glVertexAttribIPointer(m_voxelIndexLoc, 1, GL_INT,  sizeof(TensorAttr), (void *) (0));
    glEnableVertexAttribArray(m_voxelIndexLoc);
    glVertexAttribDivisor(m_voxelIndexLoc, 1);
}

void BaseSuperquadricRendererProgramShader::setModelViewMatrix(glm::mat4 modelView)
{
    if(m_initialized)
    {
        m_superquadric_program->binding();
        GLshort mvMatrixLoc = glGetUniformLocation(m_superquadric_program->getProgram(), "modelViewMatrix");
        glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelView));
        m_superquadric_program->releasing();
    }
}

void BaseSuperquadricRendererProgramShader::setProjectionMatrix(glm::mat4 projection)
{
    if(m_initialized)
    {
        m_superquadric_program->binding();
        GLshort projectionMatrixLoc = glGetUniformLocation(m_superquadric_program->getProgram(), "projectionMatrix");
        glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projection));
        m_superquadric_program->releasing();
    }
}

void BaseSuperquadricRendererProgramShader::setVisibleVoxelDataBufferID(unsigned int visibleVoxelDataBufferID)
{
    m_visibleVoxelDataBufferID = visibleVoxelDataBufferID;
}

void BaseSuperquadricRendererProgramShader::setRenderingCommandBufferID(unsigned int renderingDataBuferID)
{
    m_indirectRenderingCommandBufferID = renderingDataBuferID;
}

void BaseSuperquadricRendererProgramShader::setVoxelFactor()
{
    GLshort direction_factor_loc = glGetUniformLocation(m_superquadric_program->getProgram(), "voxel_factor");

    if(m_voxelDim[0] < m_voxelDim[1])
    {
        if(m_voxelDim[0] < m_voxelDim[2])
        {
            glUniform1f(direction_factor_loc, m_voxelDim[0]);
            m_voxelFactor = m_voxelDim[0];

        } else {

            glUniform1f(direction_factor_loc, m_voxelDim[2]);
            m_voxelFactor = m_voxelDim[2];
        }

    } else if(m_voxelDim[1] < m_voxelDim[2]) {

        glUniform1f(direction_factor_loc, m_voxelDim[1]);
        m_voxelFactor = m_voxelDim[1];

    } else {

        glUniform1f(direction_factor_loc, m_voxelDim[2]);
        m_voxelFactor = m_voxelDim[2];
    }
}

void BaseSuperquadricRendererProgramShader::setLighting()
{
    GLuint program = m_superquadric_program->getProgram();

    GLshort ambient_loc         = glGetUniformLocation(program, "Ambient");
    GLshort light_color_loc     = glGetUniformLocation(program, "LightColor");
    GLshort light_direction_loc = glGetUniformLocation(program, "LightDirection");
    GLshort half_vector_loc     = glGetUniformLocation(program, "HalfVector");
    GLshort shininess_loc       = glGetUniformLocation(program, "Shininess");
    GLshort strength_loc        = glGetUniformLocation(program, "Strength");

    glm::vec3 ambient(0.05f, 0.05f, 0.05f);
    glm::vec3 light_color(0.7f, 0.7f, 0.7f);
    glm::vec3 light_direction(0.f, 0.f, 1.f);
    glm::vec3 half_vector = glm::vec3(light_direction + glm::vec3(0.f, 0.f, 1.f));
    half_vector = glm::normalize(half_vector);

    GLfloat shininess = 10.f;
    GLfloat strength = 0.3f;

    glUniform3fv(ambient_loc, 1, glm::value_ptr(ambient));
    glUniform3fv(light_color_loc, 1, glm::value_ptr(light_color));
    glUniform3fv(light_direction_loc, 1, glm::value_ptr(light_direction));
    glUniform3fv(half_vector_loc, 1, glm::value_ptr(half_vector));
    glUniform1f(shininess_loc, shininess);
    glUniform1f(strength_loc, strength);
}
