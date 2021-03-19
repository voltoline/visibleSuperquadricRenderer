#include "QManagementResources.h"

#include <QFile>
#include <QString>

QManagementResource::QManagementResource()
{
    m_isfile=false;
    m_shader = 0;
    m_enableInfoCompile=false;
}

QManagementResource::~QManagementResource()
{
    if(m_shader > 0)
        glDeleteShader(m_shader);
}

GLuint QManagementResource::compileFileShader(TypeOfShader tos, std::string file)
{    
    m_isfile=true; m_filename=file;
    return compileStringShader(tos,loadFileShader((char*)file.c_str()));
}

GLuint QManagementResource::compileResourceFileShader(TypeOfShader tos, std::string file)
{
    m_isfile=true; m_filename=file;
    return compileStringShader(tos,loadResourceFileShader((char*)file.c_str()));
}

GLuint QManagementResource::compileStringShader(TypeOfShader tos, std::string sourceCode)
{
    setNameTypeShader(tos);
    m_sourceCode=(char*)sourceCode.c_str();
    const char* source=(const char*) m_sourceCode;

    m_shader = glCreateShader(tos) ;

    glShaderSource ( m_shader, 1, &source, 0 ) ;

    glCompileShader( m_shader ) ;
    GLint compiled ;
    glGetShaderiv( m_shader, GL_COMPILE_STATUS, &compiled ) ;
    if ( !compiled )
    {
        shaderInformation(m_nameTypeShader,m_shader);
        if(!m_isfile){
        std::cerr << "Failed to compile "<<m_nameTypeShader<<" shader."<<std::endl;
        }
        else{
        std::cerr << "Failed to compile "<<m_nameTypeShader<<" shader - Filename: "<<m_filename<<std::endl;
        }

        GLsizei len;
        glGetShaderiv( m_shader, GL_INFO_LOG_LENGTH, &len );
        GLchar* log = new GLchar[len+1];
        glGetShaderInfoLog( m_shader, len, &len, log );
        std::cerr << "Shader compilation failed: " << log << std::endl;
        delete [] log;

        std::cerr << "Source: "<<std::endl;
        std::cerr << m_sourceCode<<std::endl;
        exit ( 1 ) ;
    }
    if(!m_isfile){
        std::cout<<m_nameTypeShader<<" shader not compiled."<<std::endl;
    }
    else{
        if(m_enableInfoCompile){
        std::cout<<m_nameTypeShader<<" shader successfully compiled.\nFile: "<<m_filename<<std::endl;
        }
    }
    std::cout<<std::endl;
    return m_shader;
}

char *QManagementResource::loadFileShader(char *file)
{
#if __APPLE__
    std::string AddMACPath="../../../";
    file=(char*)AddMACPath.append(file).c_str();
#endif
    FILE *f;
    char *sourceCode = NULL;

    #ifdef _WIN32
        f = fopen( file, "rb" );
    #else
        f = fopen( file, "rb" );
    #endif
    if ( !f ) {
        std::cerr << "Unable to open file: '" << file << "'" << std::endl;
        return NULL;
    }
    int count=0;
      if (f != NULL) {
        fseek(f, 0, SEEK_END);
        count = ftell(f);
        rewind(f);
        if (count > 0) {
      sourceCode = (char *)malloc(sizeof(char) * (count+1));
      count = fread(sourceCode,sizeof(char),count,f);
      sourceCode[count] = '\0';
        }
        fclose(f);
      }

      return sourceCode;
}

std::string QManagementResource::loadResourceFileShader(char *file)
{
    QString sourceCode;
    QFile infile(file);

    if (!infile.open(QIODevice::ReadOnly))
        {
           std::cerr << "Unable to open file: '" << file<< "'" <<std::endl;

        }
    else
        {
            sourceCode = infile.readAll();
            infile.close();
        }


    return sourceCode.toStdString();
}

const char *QManagementResource::getNameTypeShader()
{
    return m_nameTypeShader;
}

void QManagementResource::setNameTypeShader(TypeOfShader tos)
{
    const char* nameShader[5]={(const char*)"vertex",(const char*)"fragment",(const char*)"geometry",(const char*)"tessControl", (const char*)"compute"};
    if(tos==vertex){m_nameTypeShader=nameShader[0];}
    if(tos==fragment){m_nameTypeShader=nameShader[1];}
    if(tos==geometry){m_nameTypeShader=nameShader[2];}
    if(tos==tessControl){m_nameTypeShader=nameShader[3];}
    if(tos==compute){m_nameTypeShader=nameShader[4];}
}

bool QManagementResource::getEnableInfoCompile() const
{
    return m_enableInfoCompile;
}

void QManagementResource::setEnableInfoCompile(bool enableInfoCompile)
{
    m_enableInfoCompile = enableInfoCompile;
}

char *QManagementResource::getSourceCode() const
{
    return m_sourceCode;
}

void QManagementResource::printSourceCode()
{
    std::cout<<std::endl;
    std::cout<<"***Printing source "<<m_nameTypeShader<<" shader***"<<std::endl;
    std::cout<<m_sourceCode<<std::endl;
    std::cout<<std::endl;
}

void QManagementResource::shaderInformation(const char *IdInf, GLuint shader)
{
        int count = 0 ;
        glGetProgramiv ( shader, GL_INFO_LOG_LENGTH, &count ) ;
        if ( count > 0 )
        {
          int size = 0;
          char *information = new char [ count ] ;
          glGetProgramInfoLog ( shader, count, &size, information ) ;
          if ( size > 0 && information [ 0 ] != '\0' ){
             std::cerr<<IdInf<<" Information \n"<<information<<std::endl;
          delete [] information ;}
        }
}
