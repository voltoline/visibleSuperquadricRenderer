/**
  * @file QManagementShader.h
  * @brief Manage GL frame buffer object.
  * @author José Angel Ivan Rubianes Silva
  * @date February 2015
  * @note This code is created by @author.
  */
#ifndef VMTKMANAGEMENTSHADER_H
#define VMTKMANAGEMENTSHADER_H

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
#include <sstream>
#include <stdio.h>

/**
 * @brief The ShaderProgram class
 * @details constructs a program from the compiled shaders
 */
class vmtkManagementShader
{
public:
    /**
     * @brief constructs/create a shader program.
     */
    vmtkManagementShader();

    /**
     * @brief deconstructs/delete a shader program.
     */
    ~vmtkManagementShader();

    /**
     * @brief add a shader to be linked to the program.
     * @param[in] value shader name
     */
    void setShader(GLuint value);
    /**
     * @brief link the shaders included in <shaders>
     * @return  true (successful) or false (failed)
     */
    bool linking();

    /**
     * @brief installs <program> object as part of current rendering state.
     * @return true (successful) or false (failed)
     */
    bool binding();

    /**
     * @brief releases the current program object from the rendering pipeline.
     */
    void releasing();

    /**
     * @brief get the program
     * @return program object name
     */
    GLuint getProgram() const;

    /**
    * @brief get information log for the program object.
    * @param[out] IdInf  information log for the program
    * @param[in] program program name
    */
    void shaderInformation(const char *IdInf, GLuint m_program);

private:
    GLuint m_program;               /**< program object */
    GLint  m_linked;                /**< link status */
};


#endif // VMTKMANAGEMENTSHADER_H
