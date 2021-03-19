#include "BaseSuperquadricTess.h"

#include "QManagementResources.h"

BaseSuperquadricTess::BaseSuperquadricTess()
{

}

void BaseSuperquadricTess::initialize()
{
    QManagementResource *vs = new QManagementResource;
    QManagementResource *tcs = new QManagementResource;
    QManagementResource *tes = new QManagementResource;

    GLuint vertex = vs->compileResourceFileShader(QManagementResource::vertex, "./shaders/base_superquadric_processing.vert");
    GLuint tessellation_control = tcs->compileResourceFileShader(QManagementResource::tessControl, "./shaders/base_superquadric_processing.tcs.glsl");
    GLuint tessellation_evaluation = tes->compileResourceFileShader(QManagementResource::tessEvaluation, "./shaders/base_superquadric_processing.tes.glsl");

    m_baseSuperquadricProcessingShaderProgram = new vmtkManagementShader;
    m_baseSuperquadricProcessingShaderProgram->setShader(vertex);
    m_baseSuperquadricProcessingShaderProgram->setShader(tessellation_control);
    m_baseSuperquadricProcessingShaderProgram->setShader(tessellation_evaluation);
    m_baseSuperquadricProcessingShaderProgram->linking();

    const char *varying_names_base_superquadric[] = {"superquadric_coord"};
    glTransformFeedbackVaryings(m_baseSuperquadricProcessingShaderProgram->getProgram(), 1, varying_names_base_superquadric, GL_INTERLEAVED_ATTRIBS);

    glLinkProgram(m_baseSuperquadricProcessingShaderProgram->getProgram());

    delete vs;
    delete tcs;
    delete tes;

    glGenTransformFeedbacks(1, &m_baseSuperquadricProcessingTransformFeedbackID);
    glGenBuffers(1, &m_baseSuperquadricProcessingTransformFeedbackBufferID);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_baseSuperquadricProcessingTransformFeedbackID);
    glBindBuffer(GL_ARRAY_BUFFER, m_baseSuperquadricProcessingTransformFeedbackBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 2 * 3 * 30 * 30, nullptr, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_baseSuperquadricProcessingTransformFeedbackBufferID);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BaseSuperquadricTess::compute()
{
    m_baseSuperquadricProcessingShaderProgram->binding();
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_baseSuperquadricInfoUniformBlockBufferID);
        {
            GLuint uniform_block_index = glGetUniformBlockIndex(m_baseSuperquadricProcessingShaderProgram->getProgram(), "base_superquadric_grid");
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_baseSuperquadricInfoUniformBlockBufferID);
            glUniformBlockBinding(m_baseSuperquadricProcessingShaderProgram->getProgram(), uniform_block_index, 0);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_baseSuperquadricProcessingTransformFeedbackID);
        {
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_baseSuperquadricProcessingTransformFeedbackBufferID);
            {
                glEnable(GL_RASTERIZER_DISCARD);
                {
                    glBeginTransformFeedback(GL_TRIANGLES);

                    glPatchParameteri(GL_PATCH_VERTICES, 1);

                    glDrawArrays(GL_PATCHES, 0, 1);

                    glEndTransformFeedback();
                }
                glDisable(GL_RASTERIZER_DISCARD);
            }
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
        }
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    }
    m_baseSuperquadricProcessingShaderProgram->releasing();
}

void BaseSuperquadricTess::setBaseSuperquadricInfoUniformBlockBufferId(GLuint buffer_id)
{
    m_baseSuperquadricInfoUniformBlockBufferID = buffer_id;
}

GLuint BaseSuperquadricTess::getBaseSuperquadricBufferId()
{
    return m_baseSuperquadricProcessingTransformFeedbackBufferID;
}
