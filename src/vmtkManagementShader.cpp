#include "vmtkManagementShader.h"

vmtkManagementShader::vmtkManagementShader()
{
    m_program = glCreateProgram();
}

vmtkManagementShader::~vmtkManagementShader()
{
    if(m_program > 0)
        glDeleteProgram(m_program);
}

void vmtkManagementShader::setShader(GLuint value)
{
    glAttachShader(m_program, value);
}

bool vmtkManagementShader::linking()
{
    glLinkProgram(m_program);
    glGetProgramiv(m_program,GL_LINK_STATUS,&m_linked);
    if ( !m_linked )
    {
        shaderInformation("Linking",m_program);
        std::cerr<<"Failed to link shader program."<<std::endl;
        return false;
    }
    else{return true;}
}

bool vmtkManagementShader::binding()
{
    if (!m_program){return false;}
    if (!m_linked){return false;}
    glUseProgram(m_program);
    return true;
}

void vmtkManagementShader::releasing()
{
    glUseProgram(0);
}

GLuint vmtkManagementShader::getProgram() const
{
    return m_program;
}

void vmtkManagementShader::shaderInformation(const char *IdInf, GLuint program)
{
        int count = 0 ;
        glGetProgramiv ( program, GL_INFO_LOG_LENGTH, &count ) ;
        if ( count > 0 )
        {
          int size = 0;
          char *information = new char [ count ] ;
          glGetProgramInfoLog ( program, count, &size, information ) ;
          if ( size > 0 && information [ 0 ] != '\0' ){
             std::cerr<<IdInf<<" Information \n"<<information<<std::endl;
          delete [] information ;}
        }
}
