/**
  * @file QManagementShader.h
  * @brief Manage GL frame buffer object.
  * @author José Angel Ivan Rubianes Silva
  * @date February 2015
  * @note This code is created by @author.
  */
#ifndef QMANAGEMENTRESOURCES_H
#define QMANAGEMENTRESOURCES_H

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

#include <stdio.h>

/**
 * @brief The ManagementShader class
 * @details compiles a shader program
 */
class QManagementResource
{
public:
    /**
     * @brief constructs a manager for a program to be runnable on GPU.
     */
    QManagementResource();
    ~QManagementResource();

    /**
     * @enum TypeOfShader
     * @brief types of shaders to be linked to a program runnable on GPU
     */
    enum TypeOfShader
    {
         vertex          = GL_VERTEX_SHADER,            /**< vertex shader */
         fragment        = GL_FRAGMENT_SHADER,          /**< fragment shader */
         geometry        = GL_GEOMETRY_SHADER,          /**< geometry shader */
         tessControl     = GL_TESS_CONTROL_SHADER,      /**< tesselation control shader */
         tessEvaluation  = GL_TESS_EVALUATION_SHADER,   /**< tesselation evaluation shader */
         compute         = GL_COMPUTE_SHADER            /**< compute shader */
    };

    /**
     * @brief compile a shader in a file.
     * @param[in] tos type of shader
     * @param[in] file name of file that contains the source code
     * @return shader object name
     */
    GLuint compileFileShader(TypeOfShader tos, std::string file);

    /**
     * @brief compile a shader in a resource file.
     * @param[in] tos type of shader
     * @param[in] file name of file that contains the source code
     * @return shader object name
     */
    GLuint compileResourceFileShader(TypeOfShader tos, std::string file);

    /**
     * @brief compile a shader from the input string.
     * @param[in] tos type of shader
     * @param[in] sourceCode source code string
     * @return shader object name
     */
    GLuint compileStringShader(TypeOfShader tos, std::string sourceCode);

    /**
     * @brief get the last compiled source code string.
     * @return code string
     */
    char *getSourceCode() const;

    /**
     * @brief print the last compiled source code string.
     */
    void printSourceCode();

    /**
     * @brief get the last compiled shader type.
     * @return type of shader
     */
    const char *getNameTypeShader();

    /**
    * @brief get information log for the program object.
    * @param[out] IdInf  information log for the program
    * @param[in] program program name
    */
    void shaderInformation(const char *IdInf, GLuint m_shader);

    /**
     * @brief Get enable status for showing the information of compilation.
     * @return enable state
     */
    bool getEnableInfoCompile() const;

    /**
     * @brief Set enable status for showing the infomation of compilation.
     * @param enableInfoCompile: state bool value for enabling
     */
    void setEnableInfoCompile(bool enableInfoCompile);

private:
    /**
     * @brief load a shader file
     * @param[in] file name of file
     * @return shader source code string
     */
    char *loadFileShader(char* file);

    /**
     * @brief load a resource shader file
     * @param[in] file name of file in format resource
     * @return shader source code string
     */
    std::string loadResourceFileShader(char* file);

    /**
     * @brief set the type of shader
     * @param[in] tos type of shader
     */
    void setNameTypeShader(TypeOfShader tos);

    char *m_sourceCode;                     /**< the last compiled source code */
    const char *m_nameTypeShader;           /**< type of the last compiled shader code */

    GLuint m_shader;            /**< the last compiled shader */
    bool m_isfile;              /**< is source a file? (true or false) */
    std::string m_filename;     /**< name file */
    bool m_enableInfoCompile;
};


#endif // QMANAGEMENTSHADER_H
