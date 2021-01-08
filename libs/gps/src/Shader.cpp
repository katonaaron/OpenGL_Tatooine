#include "Shader.hpp"

namespace gps {
    std::string Shader::readShaderFile(std::string fileName)
    {
        std::ifstream shaderFile;
        std::string shaderString;

        //open shader file
        shaderFile.open(fileName.c_str());

        std::stringstream shaderStringStream;

        //read shader content into stream
        shaderStringStream << shaderFile.rdbuf();

        //close shader file
        shaderFile.close();

        //convert stream into GLchar array
        shaderString = shaderStringStream.str();
        return shaderString;
    }

    void Shader::shaderCompileLog(GLuint shaderId)
    {
        GLint success;
        GLchar infoLog[512];

        //check compilation info
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
            std::cout << "Shader compilation error\n" << infoLog << std::endl;
        }
    }

    void Shader::shaderLinkLog(GLuint shaderProgramId)
    {
        GLint success;
        GLchar infoLog[512];

        //check linking info
        glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "Shader linking error\n" << infoLog << std::endl;
        }
    }

    void Shader::loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName)
    {
        //read, parse and compile the vertex shader
        std::string v = readShaderFile(vertexShaderFileName);
        const GLchar* vertexShaderString = v.c_str();
        GLuint vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderString, NULL);
        glCompileShader(vertexShader);
        //check compilation status
        shaderCompileLog(vertexShader);

        //read, parse and compile the vertex shader
        std::string f = readShaderFile(fragmentShaderFileName);
        const GLchar* fragmentShaderString = f.c_str();
        GLuint fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderString, NULL);
        glCompileShader(fragmentShader);
        //check compilation status
        shaderCompileLog(fragmentShader);

        //attach and link the shader programs
        this->shaderProgram = glCreateProgram();
        glAttachShader(this->shaderProgram, vertexShader);
        glAttachShader(this->shaderProgram, fragmentShader);
        glLinkProgram(this->shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        //check linking info
        shaderLinkLog(this->shaderProgram);
    }

    void Shader::useShaderProgram()
    {
        glUseProgram(this->shaderProgram);
    }

    void Shader::loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName,
                            std::string geometryShaderFileName) {

        //read, parse and compile the vertex shader
        std::string v = readShaderFile(vertexShaderFileName);
        const GLchar* vertexShaderString = v.c_str();
        GLuint vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderString, NULL);
        glCompileShader(vertexShader);
        //check compilation status
        shaderCompileLog(vertexShader);

        //read, parse and compile the vertex shader
        std::string f = readShaderFile(fragmentShaderFileName);
        const GLchar* fragmentShaderString = f.c_str();
        GLuint fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderString, NULL);
        glCompileShader(fragmentShader);
        //check compilation status
        shaderCompileLog(fragmentShader);

        //read, parse and compile the geometry shader
        std::string g = readShaderFile(geometryShaderFileName);
        const GLchar* geometryShaderString = g.c_str();
        GLuint geometryShader;
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geometryShaderString, NULL);
        glCompileShader(geometryShader);
        //check compilation status
        shaderCompileLog(geometryShader);

        //attach and link the shader programs
        this->shaderProgram = glCreateProgram();
        glAttachShader(this->shaderProgram, vertexShader);
        glAttachShader(this->shaderProgram, fragmentShader);
        glAttachShader(this->shaderProgram, geometryShader);
        glLinkProgram(this->shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteShader(geometryShader);
        //check linking info
        shaderLinkLog(this->shaderProgram);
    }

    void Shader::setUniform(const std::string &name, bool value) {
        glUniform1i(getUniformLocation(name), (int)value);
    }

    void Shader::setUniform(const std::string &name, int value) {
        glUniform1i(getUniformLocation(name), value);
    }

    void Shader::setUniform(const std::string &name, float value) {
        glUniform1f(getUniformLocation(name), value);
    }

    void Shader::setUniform(const std::string &name, const glm::vec3 &value) {
        glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
    }

    void Shader::setUniform(const std::string &name, const glm::vec4 &value) {
        glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
    }

    void Shader::setUniform(const std::string &name, const glm::mat3 &value) {
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::setUniform(const std::string &name, const glm::mat4 &value) {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    GLuint Shader::getUniformLocation(const std::string &name) {
        std::unordered_map<std::string, GLuint>::const_iterator uniform = uniformLocations.find(name);

        if(uniform == uniformLocations.end()) {
            const GLuint uniformLoc = glGetUniformLocation(shaderProgram, name.c_str());
            uniformLocations[name] = uniformLoc;
            return uniformLoc;
        } else {
            return uniform->second;
        }
    }

}
