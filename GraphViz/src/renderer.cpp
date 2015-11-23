#include "../include/renderer.h"

#include <iostream>
#include <fstream>

int Renderer::mWidth = 0;
int Renderer::mHeight = 0;
float   Renderer::mDeltaTime = 0.0f;
Renderer::Camera  Renderer::mCamera;
Renderer::MOUSESTATE Renderer::mState = IDLE;

bool Renderer::initGLUT(int &argc, char **argv, unsigned int width, unsigned int height)
{
    //init glut, context, version
    mWidth = width;
    mHeight = height;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(mWidth,mHeight);
    glutInitContextVersion(4,3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("GraphViz");

    if(glewInit())
    {
        std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
        return false;
    }
    //set functions for callbacks
    glutDisplayFunc(Renderer::display);
    glutKeyboardFunc(Renderer::keyboard);
    glutMouseFunc(Renderer::mouseButton);
    glutMotionFunc(Renderer::mouseMotion);
    glutIdleFunc(Renderer::idle);
    glutReshapeFunc(Renderer::resize);

    //white as clear color
    glClearColor(1.0f,1.0f,1.0f,1.0f);


    return true;
}

bool Renderer::initBuffers()
{

    return true;
}

bool Renderer::initProgram()
{
    //Create Shaders

    //vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertexSource = readFile("../shader/vertexShader.glsl");
    const char* vertexSourceC = vertexSource.c_str();
    glShaderSource(vertexShader,1,&vertexSourceC,NULL);

    glCompileShader(vertexShader);

    printShaderInfoLog(vertexShader);

    //fragmentShader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragmentSource = readFile("../shader/fragmentShader.glsl");
    const char* fragmentSourceC = fragmentSource.c_str();
    glShaderSource(fragmentShader,1,&fragmentSourceC,NULL);

    glCompileShader(fragmentShader);

    printShaderInfoLog(fragmentShader);


    GLuint prog = glCreateProgram();

    glAttachShader(prog,vertexShader);
    glAttachShader(prog,fragmentShader);

    glLinkProgram(prog);

    glUseProgram(prog);

    return true;
}

bool Renderer::parseData(Compound *c)
{
    return true;
}

void Renderer::run()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,1,0,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0,0,mWidth,mHeight);
    glutMainLoop();
}

void Renderer::display()
{
    int start = glutGet(GLUT_ELAPSED_TIME);
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
    int end = glutGet(GLUT_ELAPSED_TIME);
    mDeltaTime = float(end-start)/1000.0f;
}

void Renderer::keyboard(unsigned char key, int x, int y)
{

    //last to call
    glutPostRedisplay();
}

void Renderer::mouseButton(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN)
    {
        if(button == GLUT_LEFT_BUTTON)
        {
            mState = ZOOM;
        }
        if(button == GLUT_RIGHT_BUTTON)
        {

        }
        if(button == GLUT_MIDDLE_BUTTON)
        {

        }
    }
    else if(state == GLUT_UP)
    {
        if(button == GLUT_LEFT_BUTTON)
        {
            mState = IDLE;
        }
        if(button == GLUT_RIGHT_BUTTON)
        {
            mState = IDLE;
        }
        if(button == GLUT_MIDDLE_BUTTON)
        {
            mState = IDLE;
        }
    }
}

void Renderer::mouseMotion(int x, int y)
{
    //last to call
    glutPostRedisplay();
}

void Renderer::resize(int width,int height)
{
    glutReshapeWindow(width,height);
    //last to call
    glutPostRedisplay();
}

void Renderer::idle()
{
    glutPostRedisplay();
}

std::string Renderer::readFile(const std::string &source)
{
   std::string content;
   std::string line;

   std::ifstream file(source.c_str());
   if(file.is_open())
   {
       while(!file.eof())
       {
           getline(file,line);
           line+="\n";
           content += line;
       }
       file.close();
   }
   else
   {
       std::cout << "Unable to open file: " << source << std::endl;
   }

   return content;
}

void Renderer::printShaderInfoLog(GLuint shader)
{
    if (shader == -1)
        return;
    GLint infologLength = 0;
    GLsizei charsWritten  = 0;
    char *infoLog;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH,&infologLength);
    infoLog = (char *)malloc(infologLength);
    glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
    free(infoLog);
}
