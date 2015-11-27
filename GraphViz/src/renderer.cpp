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
Renderer::Buffer<Point2D> Renderer::mSplineBuffer;
Renderer::Buffer<Point2D> Renderer::mSplineControlBuffer;
Renderer::Buffer<float>   Renderer::mSplineTBuffer;
GLuint Renderer::vaoTreeConn = -1;
GLuint Renderer::vaoSpline = -1;
GLuint Renderer::vaoTreeNode = -1;
GLuint Renderer::progTreeConn = 0;
GLuint Renderer::progSpline = 0;
GLuint Renderer::progTreeNode = 0;

int Renderer::stepsize = 50;
float Renderer::beta = 0.5f;

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

    glEnable(GL_MULTISAMPLE);
    glPointSize(5.0f);
    glLineWidth(2.0f);

    return true;
}

bool Renderer::initBuffers()
{
    //generate and bind VertexArrayObject
    glGenVertexArrays(1,&vaoTreeConn);
    glBindVertexArray(vaoTreeConn);

    // generate and initialize buffer for connections
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
     glBindVertexArray(0);

    //generate and initialize buffer for treenodes

     glGenVertexArrays(1,&vaoTreeNode);
     glBindVertexArray(vaoTreeNode);

    glGenBuffers(1,&mTreeNodeBuffer.id);
    glBindBuffer(GL_ARRAY_BUFFER,mTreeNodeBuffer.id);
    glBufferData(GL_ARRAY_BUFFER,0,NULL,GL_STATIC_DRAW);

    glVertexAttribPointer(0,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);


    glGenVertexArrays(1,&vaoSpline);
    glBindVertexArray(vaoSpline);

    glGenBuffers(1,&mSplineBuffer.id);
    glBindBuffer(GL_ARRAY_BUFFER,mSplineBuffer.id);
    glBufferData(GL_ARRAY_BUFFER,0,NULL,GL_STATIC_DRAW);

    glVertexAttribPointer(0,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glGenBuffers(1,&mSplineControlBuffer.id);
    glBindBuffer(GL_ARRAY_BUFFER,mSplineControlBuffer.id);
    glBufferData(GL_ARRAY_BUFFER,0,NULL,GL_STATIC_DRAW);

    glVertexAttribPointer(1,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glGenBuffers(1,&mSplineTBuffer.id);
    glBindBuffer(GL_ARRAY_BUFFER,mSplineTBuffer.id);
    glBufferData(GL_ARRAY_BUFFER,0,NULL,GL_STATIC_DRAW);

    glVertexAttribPointer(2,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

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

    std::string vertexSource = readFile(shaderDir+"/connVertexShader.vert");
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
    std::string fragmentSource = readFile(shaderDir+"/connFragmentShader.frag");
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
    progTreeConn = glCreateProgram();

	//attach shader to program
    glAttachShader(progTreeConn,vertexShader);
    glAttachShader(progTreeConn,fragmentShader);

	//link shader together
    glLinkProgram(progTreeConn);

    glDetachShader(progTreeConn,vertexShader);
    glDetachShader(progTreeConn,fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    vertexSource = readFile(shaderDir+"/splineVertexShader.vert");
    vertexSourceC = vertexSource.c_str();

    glShaderSource(vertexShader,1,&vertexSourceC,NULL);

    glCompileShader(vertexShader);

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

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    fragmentSource = readFile(shaderDir+"/splineFragmentShader.frag");
    fragmentSourceC = fragmentSource.c_str();

    glShaderSource(fragmentShader,1,&fragmentSourceC,NULL);

    glCompileShader(fragmentShader);

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

    progSpline = glCreateProgram();

    glAttachShader(progSpline,vertexShader);
    glAttachShader(progSpline,fragmentShader);

    glLinkProgram(progSpline);

    glDetachShader(progSpline,vertexShader);
    glDetachShader(progSpline,fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vertexShader =  glCreateShader(GL_VERTEX_SHADER);
    vertexSource = readFile(shaderDir+"/nodeVertexShader.vert");
    vertexSourceC = vertexSource.c_str();

    glShaderSource(vertexShader,1,&vertexSourceC,NULL);
    glCompileShader(vertexShader);

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

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    fragmentSource = readFile(shaderDir+"/nodeFragmentShader.frag");
    fragmentSourceC = fragmentSource.c_str();

    glShaderSource(fragmentShader,1,&fragmentSourceC,NULL);
    glCompileShader(fragmentShader);

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

    progTreeNode = glCreateProgram();

    glAttachShader(progTreeNode,vertexShader);
    glAttachShader(progTreeNode,fragmentShader);

    glLinkProgram(progTreeNode);

    glDetachShader(progTreeNode,vertexShader);
    glDetachShader(progTreeNode,fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

bool Renderer::parseData(Compound *c)
{
    //get root
    TreeNode* t = c->get_node(c->get_root_id());


    //calculate radial positions for treenodes
    setRadialPosition(c,t,0.0f,360.0f,0.17f);

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


        createSplines(mSplineBuffer.data,controlpoints);
    }
    std::cout << "created Splines" << std::endl;

//    mPathBuffer.data = createSplines(controlPoints,50);

    glBindBuffer(GL_ARRAY_BUFFER,mSplineBuffer.id);
    glBufferData(GL_ARRAY_BUFFER,mSplineBuffer.data.size() * sizeof(Point2D),NULL,GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER,0,mSplineBuffer.data.size() * sizeof(Point2D),mSplineBuffer.data.data());
    glBindBuffer(GL_ARRAY_BUFFER,0);

   //send node positions to opengl
   glBindBuffer(GL_ARRAY_BUFFER,mTreeNodeBuffer.id);
   glBufferData(GL_ARRAY_BUFFER,mTreeNodeBuffer.data.size() * sizeof(Point2D),NULL,GL_STATIC_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER,0,mTreeNodeBuffer.data.size()* sizeof(Point2D),mTreeNodeBuffer.data.data());
   glBindBuffer(GL_ARRAY_BUFFER,0);

   //send nodeConnections to opengl
   glBindBuffer(GL_ARRAY_BUFFER,mTreeConnectionBuffer.id);
   glBufferData(GL_ARRAY_BUFFER,mTreeConnectionBuffer.data.size() * sizeof(Point2D),NULL,GL_STATIC_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER,0,mTreeConnectionBuffer.data.size() * sizeof(Point2D),mTreeConnectionBuffer.data.data());
   glBindBuffer(GL_ARRAY_BUFFER,0);

   glBindBuffer(GL_ARRAY_BUFFER,mSplineControlBuffer.id);
   glBufferData(GL_ARRAY_BUFFER,mSplineControlBuffer.data.size() * sizeof(Point2D),NULL,GL_STATIC_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER,0,mSplineControlBuffer.data.size()* sizeof(Point2D),mSplineControlBuffer.data.data());
   glBindBuffer(GL_ARRAY_BUFFER,0);

   glBindBuffer(GL_ARRAY_BUFFER,mSplineTBuffer.id);
   glBufferData(GL_ARRAY_BUFFER,mSplineTBuffer.data.size() * sizeof(Point2D),NULL,GL_STATIC_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER,0,mSplineTBuffer.data.size() * sizeof(Point2D), mSplineTBuffer.data.data());
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
    renderSplines();
    renderNodes();
    //display rendered image
    glutSwapBuffers();


	//time measurement
    int end = glutGet(GLUT_ELAPSED_TIME);
    mDeltaTime = float(end-start)/1000.0f;
}

void Renderer::renderTree()
{
    glUseProgram(progTreeConn);
    glBindVertexArray(vaoTreeConn);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_LINES,0,mTreeConnectionBuffer.data.size());
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Renderer::renderSplines()
{
    glUseProgram(progSpline);
    glBindVertexArray(vaoSpline);

    GLint betaID = glGetUniformLocation(progSpline,"beta");

    glUniform1f(betaID,beta);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    for(unsigned int i = 0;i < mSplineBuffer.data.size();i+=stepsize)
    {
        glDrawArrays(GL_LINE_STRIP,i,stepsize);
    }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Renderer::renderNodes()
{
    glUseProgram(progTreeNode);

    glBindVertexArray(vaoTreeNode);

    glEnableVertexAttribArray(0);

    glDrawArrays(GL_POINTS,0,mTreeNodeBuffer.data.size());

    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Renderer::keyboard(unsigned char key, int x, int y)
{

    //last to call
    switch(key)
    {
    case 'm' : beta+= 0.01f;break;
    case 'n' : beta-= 0.01f;break;
       default: break;
    }
    beta = beta < 0.0f ? 0.0f : beta;
    beta = beta > 1.0f ? 1.0f : beta;
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
void Renderer::createSplines(std::vector<Point2D> &spline,const std::vector<Point2D> &controlPoints)
{

    std::vector<float> knotsequence;

    //degree, cubic because of paper
    //if the control polygon only consists of 3 points, use quadratic b spline
    int degree = controlPoints.size() > 3 ? 3 : 2;
    int order = degree+1;

    //create knot sequence
    //interpolate first point
    for(unsigned int i = 0;i < order;++i)
    {
        knotsequence.push_back(0);
    }
    float knotindex = 1.0f;
    for(unsigned int i = order;i < (controlPoints.size());++i)
    {
        knotsequence.push_back(knotindex);
        knotindex++;
    }
    //interpolate last point
    for(unsigned int i = 0;i < order;++i)
    {
        knotsequence.push_back(controlPoints.size()-degree);
    }


    //loop over every step, create steps many points on spline
    for(unsigned int l = 0;l < stepsize;++l)
    {
        //for bundling, first and last point of polygon needed
        Point2D first = controlPoints.at(0);
        Point2D last = controlPoints.at(controlPoints.size()-1);
        //parameter to determine point on curve
        float t = (l * knotsequence.at(knotsequence.size()-1)/(stepsize-1));
        //find startIndex
        int r = degree + floor(t);
        if(t == knotsequence.at(knotsequence.size()-1))
        {
            mSplineControlBuffer.data.push_back(first);
            mSplineControlBuffer.data.push_back(last);
            mSplineTBuffer.data.push_back(float(l)/float(stepsize));
            spline.push_back(controlPoints.at(controlPoints.size()-1));
            continue;
        }
        //find points used to calculate point on spline
        std::vector<Point2D> iterationPoints;

        for(unsigned int j = r - order + 1;j <= r;++j)
        {
            iterationPoints.push_back(controlPoints.at(j));
        }

        for(unsigned int j = 1;j <= degree;++j)
        {
            for( int i = r;i >= r - order +1 + j;--i)
            {
                Point2D d1 = iterationPoints.at(i-1-floor(t)) * (1-alpha(t,knotsequence.at(i),knotsequence.at(i+order-j)));

                Point2D d2 =  iterationPoints.at(i-floor(t)) * alpha(t,knotsequence.at(i),knotsequence.at(i+order-j));
                iterationPoints.at(i-(floor(t))) =   d1+d2;
            }
        }


        //TODO beta parameter

        Point2D old = iterationPoints.at(iterationPoints.size()-1);
        mSplineControlBuffer.data.push_back(first);
        mSplineControlBuffer.data.push_back(last);
        std::cout << "L:" << l << " stepsize" << stepsize << "STEPSIZE" << float(l)/float(stepsize) << std::endl;
        mSplineTBuffer.data.push_back(float(l)/float(stepsize));
        //old = old * beta + (first + (last -first) * (t/(controlPoints.size()-degree))) * (1.0f-beta);

        spline.push_back(old);

    }

}

float Renderer::alpha(float t, float ti, float tikj)
{
    float a1 = t-ti;
    float a2 = tikj-ti;

    if(a2 == 0)
    {
        return 0;
    }
    float result = (t-ti)/(tikj-ti);
    return result;
}

