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

GLuint Renderer::progTree = 0;
bool Renderer::initGLUT(int &argc, char **argv, unsigned int width, unsigned int height)
{
    //init glut, context, version
    mWidth = width;
    mHeight = height;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutInitWindowSize(mWidth,mHeight);
    glutInitContextVersion(4,0);
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

    glPointSize(5.0f);
    return true;
}

bool Renderer::initBuffers()
{
    //generate and bind VertexArrayObject
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    //generate and initialize buffer for treenodes
    glGenBuffers(1,&mTreeNodeBuffer.id);
    glBindBuffer(GL_ARRAY_BUFFER,mTreeNodeBuffer.id);
    glBufferData(GL_ARRAY_BUFFER,0,NULL,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    //generate and initialize buffer for connections
    glGenBuffers(1,&mTreeConnectionBuffer.id);
    glBindBuffer(GL_ARRAY_BUFFER,mTreeConnectionBuffer.id);
    glBufferData(GL_ARRAY_BUFFER,0,NULL,GL_STATIC_DRAW);

    glVertexAttribPointer(0,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER,0);
    return true;
}

bool Renderer::initProgram()
{

    GLint success = 0;
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
    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);

    if(success == GL_FALSE)
    {
        std::cout << "compilation of vertex shader failed." << std::endl;
        return false;
    }
    else
    {
        std::cout << "successfully compiled vertex shader." << std::endl;
    }
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
    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);

    if(success == GL_FALSE)
    {
        std::cout << "compilation of vertex shader failed." << std::endl;
        return false;
    }
    else
    {
        std::cout << "successfully compiled fragment shader." << std::endl;
    }

	//define and create program
    progTree = glCreateProgram();

	//attach shader to program
    glAttachShader(progTree,vertexShader);
    glAttachShader(progTree,fragmentShader);

	//link shader together
    glLinkProgram(progTree);

    glDetachShader(progTree,vertexShader);
    glDetachShader(progTree,fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

	//use this program
    glUseProgram(progTree);

    return true;
}

bool Renderer::parseData(Compound *c)
{
    //get root
    TreeNode* t = c->get_node(c->get_root_id());


    //calculate radial positions for treenodes
    setRadialPosition(c,t,0.0f,360.0f,0.1f);

    //write pathes to buffer, first Point2D determines length of Path, e.g.(Point2D(3,3) next three entries are one path
    const std::vector<std::pair<NodeId,NodeId>>* leaves = c->get_connections();
    for(unsigned int i = 0;i <leaves->size();++i)
    {
        //find connection and shortest path, push back into pathBuffer

        //ugly workaround
        std::pair<NodeId,NodeId> con = leaves->at(i);
        Path p = c->get_shortest_path(con.first,con.second);
        std::vector<Point2D> controlpoints;
        for(unsigned int j = 0;j < p.nodes.size();++j)
        {
            controlpoints.push_back(c->get_node(p.nodes.at(j))->get_position());
        }

        std::vector<Point2D> spline = createSplines(controlpoints,50);

    }


   //send node positions to opengl
   glBindBuffer(GL_ARRAY_BUFFER,mTreeNodeBuffer.id);
   glBufferData(GL_ARRAY_BUFFER,mTreeNodeBuffer.data.size() * sizeof(Point2D),mTreeNodeBuffer.data.data(),GL_STATIC_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER,0,mTreeNodeBuffer.data.size()* sizeof(Point2D),mTreeNodeBuffer.data.data());
   glBindBuffer(GL_ARRAY_BUFFER,0);

   //send nodeConnections to opengl
   glBindBuffer(GL_ARRAY_BUFFER,mTreeConnectionBuffer.id);
   glBufferData(GL_ARRAY_BUFFER,mTreeConnectionBuffer.data.size() * sizeof(Point2D),mTreeConnectionBuffer.data.data(),GL_STATIC_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER,0,mTreeConnectionBuffer.data.size() * sizeof(Point2D),mTreeConnectionBuffer.data.data());
   glBindBuffer(GL_ARRAY_BUFFER,0);

    return true;
}

void Renderer::run()
{
	//dummy MVP
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,1,0.1,100);
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

    renderTree();
    //display rendered image
    glutSwapBuffers();


	//time measurement
    int end = glutGet(GLUT_ELAPSED_TIME);
    mDeltaTime = float(end-start)/1000.0f;
}

void Renderer::renderTree()
{
    glUseProgram(progTree);
    glDrawArrays(GL_LINES,0,mTreeConnectionBuffer.data.size());
    glDrawArrays(GL_POINTS,0,mTreeConnectionBuffer.data.size());
    //ugly fix for blue center
    glDrawArrays(GL_POINTS,0,1);
    glUseProgram(0);
}

void Renderer::renderSplines()
{
    //glUseProgram(progSplines);
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

        mTreeNodeBuffer.data.push_back(t->get_position());
    }
    else
    {

        //calculate position from angles
        float angle = (angleMin+angleMax)/2;
        angle *= PI/180;
        float x = radius*(sin(angle)) * t->get_level();
        float y = radius*(cos(angle)) * t->get_level();
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

            setRadialPosition(c,child,tempMin,tempMax,radius);
        }
        else
        {
            TreeNode* child = c->get_node(t->get_child_ids().at(i));
            float tempMin = tempMax;
            tempMax = tempMin + (angleMax-angleMin) * (child->get_num_children()+1)/t->get_num_children();
            setRadialPosition(c,child,tempMin,tempMax,radius);
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

//controlPoints: points of control polygon
//steps : steps of t
std::vector<Point2D> Renderer::createSplines(const std::vector<Point2D> &controlPoints,float steps)
{
    std::vector<Point2D> result;
    //TODO: create cubic B-Spline with uniform parameterization
    //for uniform parameterization, first control points interpolates t0 up to t3 with 0,
    //each t for each Point is the number of the point
    //e.g. t4 for controlPoint.at(1) is 1, t5 for controlPoint.at(2) is 2
    //steps determines the number of returning spline points, e.g. steps = 50, return 50 interpolated points on the spline
    //Have fun

    for(unsigned int i = 0;i < steps;++i)
    {
        //parameter to determine point on curve
        float t = i * (controlPoints.size()-1)/(steps-1);

    }

    return result;
}

