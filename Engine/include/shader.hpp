#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

class Shader
{
public:
    unsigned int ID;
    Shader(){}

    ~Shader(){
        glDeleteProgram(ID);
    }
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();		
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();			
            // if geometry shader path is present, also load a geometry shader
            if(geometryPath != nullptr)
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        unsigned int geometry;
        if(geometryPath != nullptr)
        {
            const char * gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }

        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if(geometryPath != nullptr)
            glAttachShader(ID, geometry);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        unsigned int uniformBlockIndex = glGetUniformBlockIndex(ID, "CameraMatrices");
        if (uniformBlockIndex != GL_INVALID_INDEX)
        {
            glUniformBlockBinding(ID, uniformBlockIndex, 0);
        }
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteShader(geometry);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() const
    { 
        glUseProgram(ID); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {   
        use();
        int flag = glGetUniformLocation(ID, name.c_str());
        if (flag != -1)
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
        }
        else
        {
            checkUniformErrors(name, flag);
        }
        
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        use();
        int flag = glGetUniformLocation(ID, name.c_str());
        if (flag != -1)
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
        }
        else
        {
            checkUniformErrors(name, flag);
        }
        
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    {   
        use();
        int flag = glGetUniformLocation(ID, name.c_str());
        if (flag != -1)
        {
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
        }
        else
        {
            checkUniformErrors(name, flag);
        }
    }
    void setMat4(const std::string &name, const glm::mat4& value) const
    { 
        use();
        int flag = glGetUniformLocation(ID, name.c_str());
        if (flag != -1)
        {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); 
        }
        else
        {
            checkUniformErrors(name, flag);
        }
        
    }
    void setVec3(const std::string &name, const glm::vec3& value) const
    { 
        use();
        int flag = glGetUniformLocation(ID, name.c_str());
        if (flag != -1)
        {
            glUniform3f(glGetUniformLocation(ID, name.c_str()),value.x, value.y, value.z); 
        }
        else
        {
            checkUniformErrors(name, flag);
        }
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        use();
        int flag = glGetUniformLocation(ID, name.c_str());
        if (flag != -1)
        {
            glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
        }
        else
        {
            checkUniformErrors(name, flag);
        }
        
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    void checkUniformErrors(const std::string &name, int flag) const
    {
        if (flag == -1)
        {
            std::cout << "FROMENT ERROR: CANT FIND UNIFORM VARIABLE " << name << std::endl;
        }
        
    }
};
#endif