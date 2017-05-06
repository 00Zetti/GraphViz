#ifndef RENDERER_H
#define RENDERER_H

#include "compound.h"

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
        Buffer<T>() : id(-1)
        {}
        GLuint id;
        std::vector<T> data;
    };

    template<typename T>
    struct uVar
    {
        uVar<T>() : id(-1)
        {}
        GLuint id;
        T value;
    };

	//mouseState
    typedef enum
    {
        IDLE = 0,
        SET = 1
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
    static Buffer<Point2D>  mTreeConnectionBuffer;  //display connections between nodes
    static Buffer<Point2D>  mTreeNodeBuffer;        //display nodes
    static Buffer<Point2D>  mSplineControlBuffer;
    static Buffer<float>    mSplineTBuffer;
    static Buffer<float>    mSplineOpacityBuffer;
	//Buffer for visualising pathes
    static Buffer<Point2D> mSplineBuffer;             //display pathes between nodes

    static GLuint vaoTreeConn;
    static GLuint vaoSpline;
    static GLuint vaoTreeNode;
    static GLuint progTreeConn;                         //display Tree
    static GLuint progSpline;                      //display Splines
    static GLuint progTreeNode;
    static GLuint progSelectSpline;
    static int    stepsize;

    static uVar<float> minAlpha;
    static uVar<float> maxAlpha;
    static uVar<float> beta;

    static bool showTree;
    static bool showLeaves;
    static bool showSplines;

    static Point2D mMousePosition;
    static GLuint mMousePositionID;

    static Point2D mMouseOldPosition;

    static Point2D mTempMouse;
    static Point2D mTempMouseOld;

    //texture shit
    static GLuint frameBuffer;
    static GLuint colorBuffer;

    static uVar<float> splineIndex;
    static bool selected;

//functions
public:

	//init GLUT, GLEW, OpenGL
    static bool     initGLUT(int &argc,char **argv,unsigned int width, unsigned int height);

	//init Buffers
    static bool     initBuffers();

	//init Shaders and Program
    static bool     initProgram();

	//parse Tree into Buffer
    static bool     parseData(Compound* c);

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


    //calculation
    static void setRadialPosition(Compound *c, TreeNode* t, float angleMin, float angleMax, float radius);
    static void createSplines(std::vector<Point2D> &spline,const std::vector<Point2D> &controlPoints);


    static void renderTree();
    static void renderSplines();
    static void renderNodes();

    //helper functions
    static float alpha(float t, float ti, float tikj);
    static void validRange(float &p,float min, float max);
    static void fillBuffer(TreeNode* parent, TreeNode* node);
    static std::string readFile(const std::string &source);

    template <typename T>
    static void copyBufferDataToGL(GLenum bufferType, Buffer<T> &buffer,GLenum draw);
    static void checkShader(GLuint shader,const std::string &name);
    static GLuint createShader(const std::string &source,GLenum shaderType);
    static GLuint createProgram(GLuint vertexShader, GLuint fragmentShader);

};


#endif //RENDERER_H
