#ifndef Shader_hpp
#define Shader_hpp

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

namespace gps {

class Shader
{
public:
    GLuint shaderProgram;
    void loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName);
    void loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName, std::string geometryShaderFileName);
    void useShaderProgram();

    // utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;

private:
    std::string readShaderFile(std::string fileName);
    void shaderCompileLog(GLuint shaderId);
    void shaderLinkLog(GLuint shaderProgramId);
};

}

#endif /* Shader_hpp */
