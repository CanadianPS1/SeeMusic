#pragma once
#include <string>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
namespace renderer{
    class RendererGL{
        public:
            RendererGL(SDL_Window* setWindow);
            ~RendererGL();
            void setDrawColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
            void fillRect(const SDL_Rect* rect);
            void fillOval(const SDL_Rect* rect);
            void swapWindow();
        private:
            void loadOpenGLFunctions();
            void loadShader();
            bool checkShaderForCompileErrors(GLuint shader, std::string shaderType);
            bool checkShaderProgramForCompileErrors(GLuint shaderProgram);
            void loadBuffers();
            void fillShape(const SDL_Rect* rect, int shapeType);
            // Entry points belong to the current SDL OpenGL context.
            PFNGLATTACHSHADERPROC glAttachShader = nullptr;
            PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
            PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
            PFNGLBUFFERDATAPROC glBufferData = nullptr;
            PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
            PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
            PFNGLCREATESHADERPROC glCreateShader = nullptr;
            PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
            PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
            PFNGLDELETESHADERPROC glDeleteShader = nullptr;
            PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;
            PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
            PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
            PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
            PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
            PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
            PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
            PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
            PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
            PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
            PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
            PFNGLUNIFORM1FPROC glUniform1f = nullptr;
            PFNGLUNIFORM1IPROC glUniform1i = nullptr;
            PFNGLUNIFORM2FPROC glUniform2f = nullptr;
            PFNGLUNIFORM4FPROC glUniform4f = nullptr;
            PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
            PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
            SDL_Window* window = nullptr;
            GLuint shaderProgramID = 0 , VAO = 0, VBO = 0, EBO = 0;
            static const char* vertexShaderSource;
            static const char* fragmentShaderSource;
    };
}