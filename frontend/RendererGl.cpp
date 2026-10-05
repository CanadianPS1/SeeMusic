#include "RendererGl.hpp"
#include <iostream>
#include <vector>
namespace renderer{
    const char* RendererGL::vertexShaderSource = 
        "#version 330 core\n"
        "layout (location = 0) in vec2 posIn;\n"
        "uniform vec2 posUL;\n"
        "uniform vec2 scaleWH;\n"
        "out vec2 posFrag;\n"
        "void main(){\n"
        "   gl_Position = vec4(posIn.x * scaleWH.x + posUL.x, posIn.y * scaleWH.y - posUL.y, 0.0, 1.0);"
        "   posFrag = posIn + vec2(-1.0f, 1.0f);\n"
        "}\0";
    const char* RendererGL::fragmentShaderSource = 
        "#version 330 core\n"
        "in vec2 posFrag;\n"
        "uniform float sizePixel;\n"
        "uniform vec4 drawColor;\n"
        "uniform int shapeType;\n"
        "out vec4 FragColor;\n"
        "void fillOval(){\n"
        "   float distance = 1.0 - sqrt(posFrag.x * posFrag.x + posFrag.y * posFrag.y) + sizePixel / 2.0;\n"
        "   float alpha = clamp(distance / sizePixel, 0.0, 1.0);\n"
        "   if(alpha < 0.01f) discard;\n"
        "   FragColor = vec4(drawColor.xyz, drawColor.w * alpha);\n"
        "}\n"
        "void fillRectangle(){\n"
        "   FragColor = drawColor;\n"
        "}\n"
        "void main(){\n"
        "   switch(shapeType){\n"
        "       case 1: fillOval(); break;\n"
        "       default: fillRectangle(); break;\n"
        "   }\n"
        "}\0";
    RendererGL::RendererGL(SDL_Window* setWindow) : window(setWindow){
        loadShader();
        loadBuffers();
        setDrawColor(255,255,255,255);
    }
    RendererGL::~RendererGL(){
        if(shaderProgramID > 0) glDeleteProgram(shaderProgramID);
        if(VAO > 0) glDeleteVertexArrays(1, &VAO);
        if(VBO > 0) glDeleteBuffers(1, &VBO);
        if(EBO > 0) glDeleteBuffers(1, &EBO);
    }
    void RendererGL::loadShader(){
        const GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShaderID);
        if(!checkShaderForCompileErrors(vertexShaderID, "vertex shader")){
            glDeleteShader(vertexShaderID);
            glDeleteShader(vertexShaderID);
            return;
        }
        const GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShaderID);
        if(!checkShaderForCompileErrors(fragmentShaderID, "fragment shader")){
            glDeleteShader(vertexShaderID);
            glDeleteShader(fragmentShaderID);
            return;
        }
        shaderProgramID = glCreateProgram();
        glAttachShader(shaderProgramID, vertexShaderID);
        glAttachShader(shaderProgramID, fragmentShaderID);
        glLinkProgram(shaderProgramID);
        if(!checkShaderProgramForCompileErrors(shaderProgramID)){
            glDeleteShader(vertexShaderID);
            glDeleteShader(fragmentShaderID);
            glDeleteProgram(shaderProgramID);
            return;
        }
        std::cout<<"Shader Program Created Correctly"<<std::endl;
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
        glUseProgram(shaderProgramID);
    }
    bool RendererGL::checkShaderForCompileErrors(GLuint shader, std::string shaderType){
        GLint statusShader;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &statusShader);
        if(statusShader == GL_FALSE){
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> errorMessage(length);
            glGetShaderInfoLog(shader, length, &length, &errorMessage[0]);
            std::cout<<"ERROR: problem creating "<<shaderType<<std::endl<<&errorMessage[0]<<std::endl;
            return false;
        }
        return true;
    }
    bool RendererGL::checkShaderProgramForCompileErrors(GLuint shaderProgram){
        GLint statusShaderProgram;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &statusShaderProgram);
        if(statusShaderProgram == GL_FALSE){
            GLint length;
            glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> errorMessage(length);
            glGetProgramInfoLog(shaderProgram, length, &length, &errorMessage[0]);
            std::cout<<"ERROR: problem creating shader program "<<std::endl<<&errorMessage[0]<<std::endl;
            return false;
        }
        return true;
    }
    void RendererGL::loadBuffers(){
        float vertices[] = {
            2.0f, 0.0f,
            2.0f, -2.0f,
            0.0f, -2.0f,
            0.0f, 0.0f
        };
        unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
        };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
    }
    void RendererGL::setDrawColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a){
        glUniform4f(glGetUniformLocation(shaderProgramID, "drawColor"), (float)(r / 255.0f), (float)(g / 255.0f), (float)(b / 255.0f), (float)(a / 255.0f));
    }
    void RendererGL::fillRect(const SDL_Rect* rect){fillShape(rect, 0);}
    void RendererGL::fillOval(const SDL_Rect* rect){fillShape(rect, 1);}
    void RendererGL::fillShape(const SDL_Rect* rect, int shapeType){
        if(rect != nullptr && rect->w > 0 && rect->h > 0 && window != nullptr){
            int windowWidth = 0, windowHeight = 0;
            SDL_GetWindowSize(window, &windowWidth, &windowHeight);
            if(windowWidth > 0 && windowHeight > 0){
                float widthGL = (float)rect->w / windowWidth;
                float heightGL = (float)rect->h / windowHeight;
                float xGL = -1.0f + (float)rect->x / windowWidth * 2.0f;
                float yGL = -1.0f + (float)rect->x / windowHeight * 2.0f;
                glUniform2f(glGetUniformLocation(shaderProgramID, "posUL"), xGL, yGL);
                glUniform2f(glGetUniformLocation(shaderProgramID, "scaleWH"), widthGL, heightGL);
                glUniform1f(glGetUniformLocation(shaderProgramID, "sizePixel"), 1.0f / rect->w + 1.0f / rect->h);
                glUniform1i(glGetUniformLocation(shaderProgramID, "shapeType"), shapeType);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
    }
    void RendererGL::swapWindow(){SDL_GL_SwapWindow(window);}
}
