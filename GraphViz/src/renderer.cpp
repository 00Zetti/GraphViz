#include "../include/renderer.h"
#include "../include/config.h"

#include <iostream>
#include <fstream>
#include <math.h>

#define PI 3.141592653589793238462

int Renderer::mWidth = 0;
int Renderer::mHeight = 0;
float   Renderer::mDeltaTime = 0.0f;
Renderer::Camera  Renderer::mCamera;
Renderer::MOUSESTATE Renderer::mState = IDLE;
Renderer::Buffer<Point2D> Renderer::mTreeConnectionBuffer;
Renderer::Buffer<Point2D> Renderer::mTreeNodeBuffer;
Renderer::Buffer<Point2D> Renderer::mPathBuffer;
GLuint Renderer::vao = -1;

bool Renderer::initGLUT(int &argc, char **argv, unsigned int width, unsigned int height)
{
    //init glut, context, version
    mWidth = width;
    mHeight = height;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutInitWindowSize(mWidth,mHeight);
    glutInitContextVersion(4,3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("GraphViz");

    glewExperimental = GL_TRUE;
    //init glew
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
 /*
    glGenVertexArrays(1,&vao);

   glBindVertexArray(vao);


    glGenBuffers(1,&mTreeNodeBuffer.id);
    glBindBuffer(GL_ARRAY_BUFFER,mTreeNodeBuffer.id);
    glBufferData(GL_ARRAY_BUFFER,mTreeNodeBuffer.data.size() * sizeof(Point2D),NULL,GL_STATIC_DRAW);


    glVertexAttribPointer(0,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glEnableVertexAttribArray(0);

    */
    return true;
}

bool Renderer::initProgram()
{
    //Create Shaders

    //vertex shader

	//create
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//find source
    std::string shaderDir(VIZ_DIR);

    std::string vertexSource = readFile(shaderDir+"/vertexShader.vert");
    const char* vertexSourceC = vertexSource.c_str();

	//attach source
    glShaderSource(vertexShader,1,&vertexSourceC,NULL);

	//compile
    glCompileShader(vertexShader);

	//print compile progression
    printShaderInfoLog(vertexShader);

    //fragmentShader

	//create
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//find source
    std::string fragmentSource = readFile(shaderDir+"/fragmentShader.frag");
    const char* fragmentSourceC = fragmentSource.c_str();

	//attach source
    glShaderSource(fragmentShader,1,&fragmentSourceC,NULL);

	//compile
    glCompileShader(fragmentShader);

	//print compile progression
    printShaderInfoLog(fragmentShader);

	//define and create program
    GLuint prog = glCreateProgram();

	//attach shader to program
    glAttachShader(prog,vertexShader);
    glAttachShader(prog,fragmentShader);

	//link shader together
    glLinkProgram(prog);

	//use this program
    glUseProgram(prog);

    return true;
}

bool Renderer::parseData(Compound *c)
{
    TreeNode* t = c->get_node(c->get_root_id());
    //calculate radial positions for treenodes
    setRadialPosition(c,t,0.0f,360.0f,1);


    //write pathes to buffer, first Point2D determines length of Path, e.g.(Point2D(3,3) next three entries are one path
    std::vector<NodeId> leaves = c->get_leaf_ids();
    for(unsigned int i = 0;i <leaves.size();++i)
    {
        //find connection and shortest path, push back into pathBuffer

    }
 //   glBufferSubData(GL_ARRAY_BUFFER,0,mTreeNodeBuffer.data.size()* sizeof(Point2D),mTreeNodeBuffer.data.data());
    return true;
}

void Renderer::run()
{
	//dummy MVP
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,1,0,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0,0,mWidth,mHeight);

	//run Main Loop
    glutMainLoop();
}

void Renderer::display()
{

	//time measurement
    int start = glutGet(GLUT_ELAPSED_TIME);

	//clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  //  glBindVertexArray(vao);

   // glDrawArrays(GL_LINES,0,GLsizei(mTreeNodeBuffer.data.size()/2));
	//display rendered image
    glutSwapBuffers();
	//time measurement
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
	//clicked mouseButton
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
	//released mouseButton
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


	//zoom in/out, when left mouse button is clicked
	if (mState == ZOOM)
	{
		//TODO: implement zoom
	}
	//last to call, force glut to update display
    glutPostRedisplay();
}

void Renderer::resize(int width,int height)
{
	mWidth = width;
	mHeight = height,
	//pass parameters to glut
    glutReshapeWindow(mWidth,mHeight);
    //last to call, force glut to update display
    glutPostRedisplay();
}

void Renderer::idle()
{
	//last to call, force glut to update display
    glutPostRedisplay();
}

std::string Renderer::readFile(const std::string &source)
{
   std::string content;
   std::string line;

   std::ifstream file;
   file.open(source.c_str(),ios_base::in);
   if(file.is_open())
   {
	   //parse file line by line into one string
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
	//shader == -1 , no Shader attached
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

void Renderer::setRadialPosition(Compound* c,TreeNode* t, float angleMin, float angleMax, float radius)
{

    //TODO: decide wether to use bisector or tangent as limits

    //check if root
    std::cout << "Node:" << t->get_id().to_string() << std::endl;
    if(t->get_level() == 0)
    {
        //set root to center
        t->set_position(Point2D(0.0f,0.0f));
        std::cout << "Position: ["<<t->get_position().x <<"|"<<t->get_position().y << "]"<< std::endl;

    }
    else
    {

        //calculate position from angles
        float angle = (angleMin+angleMax)/2;
        angle *= PI/180;
        float x = radius*(sin(angle));
        float y = radius*(cos(angle));
        t->set_position(Point2D(x,y));
        std::cout << "Position: ["<<t->get_position().x <<"|"<<t->get_position().y << "]" << std::endl;
        fillBuffer(c->get_node(t->get_parent_id()),t);
    }
    float tempMax = 0.0f;
    //iterate over each child
    for(unsigned int i = 0;i < t->get_child_ids().size();++i)
    {
        //calc angleMin/Max
        if(i == 0)
        {
            TreeNode* child = c->get_node(t->get_child_ids().at(i));
            float tempMin = angleMin;
            tempMax = tempMin + (angleMax-angleMin) * (child->get_num_children()+1)/t->get_num_children();

            setRadialPosition(c,child,tempMin,tempMax,radius*child->get_level());
        }
        else
        {
            TreeNode* child = c->get_node(t->get_child_ids().at(i));
            float tempMin = tempMax;
            tempMax = tempMin + (angleMax-angleMin) * (child->get_num_children()+1)/t->get_num_children();
            setRadialPosition(c,child,tempMin,tempMax,radius*child->get_level());
        }
    }
}

void Renderer::fillBuffer(TreeNode *parent, TreeNode *node)
{
    if(parent != NULL)
    mTreeConnectionBuffer.data.push_back(parent->get_position());
    if(node != NULL)
    {
        mTreeConnectionBuffer.data.push_back(node->get_position());
        mTreeNodeBuffer.data.push_back(node->get_position());
    }
}
