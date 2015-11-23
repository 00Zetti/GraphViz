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

    template<typename T>
    struct Buffer
    {
        int id;
        std::vector<T> data;
    };

    typedef enum
    {
        IDLE = 0,
        ZOOM = 1
    }MOUSESTATE;

//variables
private:

    static int mWidth;
    static int mHeight;
    static Camera   mCamera;
    static float    mDeltaTime;
    static MOUSESTATE mState;
    static Buffer<float> mTreeNodeBuffer;
    static Buffer<float> mSplineBuffer;

//functions
public:
    static bool     initGLUT(int &argc,char **argv,unsigned int width, unsigned int height);
    static bool     initBuffers();
    static bool     initProgram();
    static bool     parseData(Compound *c);
    static void     run();

private:
    static void     display();
    static void     keyboard(unsigned char key, int x, int y);
    static void     mouseButton(int button, int state, int x, int y);
    static void     mouseMotion(int x, int y);
    static void     resize(int width,int height);
    static void     idle();

    static std::string readFile(const std::string &source);
    static void printShaderInfoLog(GLuint shader);
};


#endif //RENDERER_H
