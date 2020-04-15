#ifndef GLESIMAGEPROCESSING_SHADER_H
#define GLESIMAGEPROCESSING_SHADER_H

#include <GLES3/gl3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Gfx
{

class Shader
{
private:
    GLuint program;
    bool isCorrect;

public:
    Shader() : program(0), isCorrect(false) {}

    ~Shader()
    {
        glDeleteProgram(program);
    }

    // Initialize shader (can't be done in the constructor)
    Shader& init()
    {
        program = glCreateProgram();
        return *this;
    }

    Shader& addStage(GLenum stage, const char* source)
    {
        GLuint stageShader = glCreateShader(stage);
        glShaderSource(stageShader, 1, &source, nullptr);
        glCompileShader(stageShader);
        GLint status;
        glGetShaderiv(stageShader, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
        {
            std::stringstream ss;
            ss << "Could not compile shader. Shader info log: ";
            GLint logLength;
            glGetShaderiv(stageShader, GL_INFO_LOG_LENGTH, &logLength);
            char *log = new char[logLength + 1];
            glGetShaderInfoLog(stageShader, logLength, nullptr, log);
            log[logLength] = '\0'; // just in case
            ss << log;
            delete[] log;
            throw std::runtime_error(ss.str());
        }
        else
        {
            glAttachShader(program, stageShader);
        }
        glDeleteShader(stageShader);
        return *this;
    }

    Shader& addStageFromFile(GLenum stage, const char* file)
    {
        std::ifstream srcStream(file);
        std::ostringstream src;
        src << srcStream.rdbuf();
        srcStream.close();

        return addStage(stage, src.str().c_str());
    }

    Shader& link()
    {
        glLinkProgram(program);
        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status != GL_TRUE)
        {
            std::stringstream ss;
            ss << "Could not link shader. Linkage info log: ";
            GLint logLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
            char *log = new char[logLength + 1];
            glGetProgramInfoLog(program, logLength, nullptr, log);
            log[logLength] = '\0';
            ss << log;
            delete[] log;
            throw std::runtime_error(ss.str());
        }
        else
        {
            isCorrect = true;
        }
        return *this;
    }

    GLint attrLoc(const char* attr) const
    {
        return glGetAttribLocation(program, attr);
    }

    GLint unfLoc(const char* uniform) const
    {
        return glGetUniformLocation(program, uniform);
    }

    bool activate() const
    {
        if (isCorrect)
        {
            glUseProgram(program);
        }
        return isCorrect;
    }

};

}

#endif //GLESIMAGEPROCESSING_SHADER_H
