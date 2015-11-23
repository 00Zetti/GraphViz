#ifndef RENDERER_H
#define RENDERER_H

#include "../include/compound.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>

class Renderer
{
//datastructures for renderer
private:

    //store camera data
    struct Camera
    {
        Camera() : height(1.0f)
        {}
        float height;
    };

	//store buffer data
    template<typename T>
    struct Buffer
    {
        int id;
        std::vector<T> data;
    };

	//mouseState
    typedef enum
    {
        IDLE = 0,
        ZOOM = 1
    }MOUSESTATE;

//variables
private:

	//screen dimensions
    static int mWidth;
    static int mHeight;
    static Camera   mCamera;

	//store FPS
    static float    mDeltaTime;
    static MOUSESTATE mState;

	//Buffer for visualising tree itself
    static Buffer<float> mTreeNodeBuffer;

	//Buffer for visualising pathes
    static Buffer<float> mPathBuffer;

//functions
public:

	//init GLUT, GLEW, OpenGL
    static bool     initGLUT(int &argc,char **argv,unsigned int width, unsigned int height);

	//init Buffers
    static bool     initBuffers();

	//init Shaders and Program
    static bool     initProgram();

	//parse Tree into Buffer
    static bool     parseData(Compound *c);

	//run Window
    static void     run();

private:

	//GLUT Callbacks
    static void     display();
    static void     keyboard(unsigned char key, int x, int y);
    static void     mouseButton(int button, int state, int x, int y);
    static void     mouseMotion(int x, int y);
    static void     resize(int width,int height);
    static void     idle();

	//read Shader into String
    static std::string readFile(const std::string &source);

	//print Compile Info(Shader)
    static void printShaderInfoLog(GLuint shader);
};


#endif //RENDERER_H
