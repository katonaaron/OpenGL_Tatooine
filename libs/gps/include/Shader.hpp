#ifndef Shader_hpp
#define Shader_hpp

#include <GL/glew.h>
#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <unordered_map>

namespace gps {

class Shader
{
public:
    GLuint shaderProgram;
    void loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName);
    void loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName, std::string geometryShaderFileName);
    void useShaderProgram();

    // utility uniform functions
    void setUniform(const std::string &name, bool value);
    void setUniform(const std::string &name, int value);
    void setUniform(const std::string &name, float value);
    void setUniform(const std::string &name, const glm::vec3& value);
    void setUniform(const std::string &name, const glm::vec4& value);
    void setUniform(const std::string &name, const glm::mat3& value);
    void setUniform(const std::string &name, const glm::mat4& value);

private:
    std::string readShaderFile(std::string fileName);
    void shaderCompileLog(GLuint shaderId);
    void shaderLinkLog(GLuint shaderProgramId);

    GLuint getUniformLocation(const std::string &name);

    std::unordered_map<std::string, GLuint> uniformLocations;
};

}

#endif /* Shader_hpp */
