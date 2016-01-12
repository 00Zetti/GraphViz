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
Renderer::Buffer<float>   Renderer::mSplineOpacityBuffer;
GLuint Renderer::vaoTreeConn = -1;
GLuint Renderer::vaoSpline = -1;
GLuint Renderer::vaoTreeNode = -1;
GLuint Renderer::progTreeConn = 0;
GLuint Renderer::progSpline = 0;
GLuint Renderer::progTreeNode = 0;

int Renderer::stepsize = 50;
Renderer::uVar<float> Renderer::beta;
Renderer::uVar<float> Renderer::minAlpha;
Renderer::uVar<float> Renderer::maxAlpha;
Renderer::uVar<float> Renderer::splineIndex;


bool Renderer::showLeaves = true;
bool Renderer::showTree = true;
bool Renderer::showSplines = true;

Point2D Renderer::mMousePosition = Point2D(-1.0f,-1.0f);
Point2D Renderer::mMouseOldPosition = Point2D(1.0f,1.0f);
Point2D Renderer::mTempMouse = Point2D(-1.0f,-1.0f);
Point2D Renderer::mTempMouseOld = Point2D(1.0f,1.0f);
GLuint Renderer::mMousePositionID = -1;

GLuint Renderer::frameBuffer = -1;
GLuint Renderer::colorBuffer = -1;
bool Renderer::selected = false;

bool Renderer::initGLUT(int &argc, char **argv, unsigned int width, unsigned int height)
{
    //init glut, context, version
    mWidth = width;
    mHeight = height;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(mWidth,mHeight);
    glutInitContextVersion(3, 1);
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
    glutPassiveMotionFunc(Renderer::mouseMotion);
    glutIdleFunc(Renderer::idle);
    glutReshapeFunc(Renderer::resize);

    //white as clear color
    glClearColor(1.0f,1.0f,1.0f,1.0f);

    glPointSize(5.0f);
    glLineWidth(2.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


   // glutSetCursor(GLUT_CURSOR_NONE);

    beta.value = 0.0f;
    minAlpha.value = 0.2f;
    maxAlpha.value = 0.7f;

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


    //generate and initialize buffers for splines

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

    glGenBuffers(1,&mSplineOpacityBuffer.id);
    glBindBuffer(GL_ARRAY_BUFFER,mSplineOpacityBuffer.id);
    glBufferData(GL_ARRAY_BUFFER,0,NULL,GL_STATIC_DRAW);

    glVertexAttribPointer(3,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);


    glGenTextures(1,&colorBuffer);
    glBindTexture(GL_TEXTURE_2D,colorBuffer);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1024,1024,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    glBindTexture(GL_TEXTURE_2D,0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,colorBuffer);

    glGenFramebuffers(1,&frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,colorBuffer,0);

    glBindFramebuffer(GL_FRAMEBUFFER,0);

    GLint p_location = glGetUniformLocation(progSpline,"texture");
    glUniform1i(p_location,0);

    //get uniform indices
    beta.id = glGetUniformLocation(progSpline,"beta");
    minAlpha.id = glGetUniformLocation(progSpline,"minAlpha");
    maxAlpha.id = glGetUniformLocation(progSpline,"maxAlpha");
    mMousePositionID = glGetUniformLocation(progSpline,"mousePosition");
    splineIndex.id = glGetUniformLocation(progSpline,"splineIndex");

    return true;
}

bool Renderer::initProgram()
{

    GLuint vertexShader = createShader("/connVertexShader.vert",GL_VERTEX_SHADER);
    GLuint fragmentShader = createShader("/connFragmentShader.frag",GL_FRAGMENT_SHADER);

    progTreeConn = createProgram(vertexShader,fragmentShader);

    vertexShader = createShader("/splineVertexShader.vert",GL_VERTEX_SHADER);
    fragmentShader = createShader("/splineFragmentShader.frag",GL_FRAGMENT_SHADER);

    progSpline = createProgram(vertexShader,fragmentShader);

    vertexShader =  createShader("/nodeVertexShader.vert",GL_VERTEX_SHADER);
    fragmentShader = createShader("/nodeFragmentShader.frag",GL_FRAGMENT_SHADER);

    progTreeNode = createProgram(vertexShader,fragmentShader);

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

    copyBufferDataToGL(GL_ARRAY_BUFFER,mTreeNodeBuffer,GL_STATIC_DRAW);

    copyBufferDataToGL(GL_ARRAY_BUFFER,mTreeConnectionBuffer,GL_STATIC_DRAW);

    copyBufferDataToGL(GL_ARRAY_BUFFER,mSplineBuffer,GL_STATIC_DRAW);

    copyBufferDataToGL(GL_ARRAY_BUFFER,mSplineControlBuffer,GL_STATIC_DRAW);

    copyBufferDataToGL(GL_ARRAY_BUFFER,mSplineOpacityBuffer,GL_STATIC_DRAW);

    copyBufferDataToGL(GL_ARRAY_BUFFER,mSplineTBuffer,GL_STATIC_DRAW);

    return true;
}

void Renderer::run()
{
	//dummy MVP
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1,1,-1,1,0.1,100);
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

    if(showTree)
    {
        renderTree();
    }
    if(showSplines)
    {
        renderSplines();
    }
    if(showLeaves)
    {
        renderNodes();
    }

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
    glBindVertexArray(vaoSpline);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    //get selected splines
  /*  if(selected)
    {
        glBindFramebuffer(GL_FRAMEBUFFER,frameBuffer);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(progSelectSpline);
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }*/

    glUseProgram(progSpline);

    glUniform1f(beta.id,beta.value);
    glUniform1f(minAlpha.id,minAlpha.value);
    glUniform1f(maxAlpha.id,maxAlpha.value);
    glUniform4f(mMousePositionID,mMousePosition.x,mMouseOldPosition.x,mMousePosition.y,mMouseOldPosition.y);


    //draw Splines
    for(unsigned int i = 0;i < mSplineBuffer.data.size();i+=stepsize)
    {
        splineIndex.value = i/stepsize;
        glUniform1f(splineIndex.id,splineIndex.value);
        glDrawArraysInstanced(GL_LINE_STRIP,i,stepsize,1);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
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

    switch(key)
    {
    case '1' : beta.value-= 0.01f;break;
    case '2' : beta.value+= 0.01f;break;
    case '3' : minAlpha.value-= 0.01f;break;
    case '4' : minAlpha.value+= 0.01f;break;
    case '5' : maxAlpha.value-= 0.01f;break;
    case '6' : maxAlpha.value+= 0.01f;break;
    case 'b' : showLeaves = !showLeaves;break;
    case 'n' : showTree = !showTree;break;
    case 'm' : showSplines = !showSplines;break;
    default  : break;
    }

    //check if parameter is still in range
    validRange(beta.value,0.0f,1.0f);
    validRange(minAlpha.value,0.0f,1.0f);
    validRange(maxAlpha.value,0.0f,1.0f);
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

            if(mState == IDLE)
            {
                mTempMouse = Point2D(-(((800.0f-float(x))/800.0f)-0.5f)*2.0f,(((800.0f-float(y))/800.0f)-0.5f)*2.0f);
                mTempMouseOld = Point2D(-(((800.0f-float(x))/800.0f)-0.5f)*2.0f,(((800.0f-float(y))/800.0f)-0.5f)*2.0f);
            }
             mState = SET;
        }
        if(button == GLUT_RIGHT_BUTTON)
        {
            mMousePosition = Point2D(-1.0f,-1.0f);
            mMouseOldPosition = Point2D(1.0f,1.0f);
            selected = false;
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
            selected = true;
            mState = IDLE;
            mMousePosition = mTempMouse;
            mMouseOldPosition = Point2D(-(((800.0f-float(x))/800.0f)-0.5f)*2.0f,(((800.0f-float(y))/800.0f)-0.5f)*2.0f);
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
    if (mState == SET)
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

    //check if root
    if(t->get_level() == 0)
    {
        //set root to center
        t->set_position(Point2D(0.0f,0.0f));

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
        if(node->get_num_children() == 0)
        {
            mTreeNodeBuffer.data.push_back(node->get_position());
        }
    }
}


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

    for( unsigned int i = 0;i < stepsize;++i)
    {
            mSplineOpacityBuffer.data.push_back(float(controlPoints.size()));
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
        if(t == knotsequence.at(0))
        {
            mSplineControlBuffer.data.push_back(first);
            mSplineControlBuffer.data.push_back(last);
            mSplineTBuffer.data.push_back(0.0f);
            spline.push_back(controlPoints.at(0));
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

        Point2D old = iterationPoints.at(iterationPoints.size()-1);
        mSplineControlBuffer.data.push_back(first);
        mSplineControlBuffer.data.push_back(last);
        mSplineTBuffer.data.push_back(float(l)/float(stepsize));

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

void Renderer::validRange(float &p,float min, float max)
{
    p = p < min ? min : p;
    p = p > max ? max : p;
}
template <typename T>
void Renderer::copyBufferDataToGL(GLenum bufferType,Buffer<T> &buffer,GLenum draw)
{
    glBindBuffer(bufferType,buffer.id);
    glBufferData(bufferType,buffer.data.size() * sizeof(T),NULL,draw);
    glBufferSubData(bufferType,0,buffer.data.size() * sizeof(T),buffer.data.data());
    glBindBuffer(bufferType,0);
}

void Renderer::checkShader(GLuint shader,const std::string &name)
{
    GLint success = 0;

    glGetShaderiv(shader,GL_COMPILE_STATUS,&success);

    if(success == GL_FALSE)
    {
        std::cout << "compilation of vertex shader failed." << name << std::endl;
    }
    else
    {
        std::cout << "successfully compiled vertex shader." << name <<  std::endl;
    }

    int log_len = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);

    if (log_len > 1)
    {
        char *log = (char*)calloc(log_len, 1);
        int chars_written = 0;

        glGetShaderInfoLog(shader, log_len, &chars_written, log);
        std::cout << log << endl;
        free(log);
    }
}

GLuint Renderer::createShader(const std::string &source,GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);

    std::string shaderDir(VIZ_DIR);

    string shaderSource = readFile(shaderDir+source);
    const char* shaderSourceCStr = shaderSource.c_str();

    glShaderSource(shader,1,&shaderSourceCStr,NULL);
    glCompileShader(shader);
    checkShader(shader,source);

    return shader;
}

GLuint Renderer::createProgram(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint program = glCreateProgram();

    glAttachShader(program,vertexShader);
    glAttachShader(program,fragmentShader);

    glLinkProgram(program);

    {
        int log_len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);

        if (log_len > 1)
        {
            char *log = (char*)calloc(log_len, 1);
            int chars_written = 0;

            glGetProgramInfoLog(program, log_len, &chars_written, log);
            printf("%s\n", log);
            free(log);
        }
    }

    glDetachShader(program,vertexShader);
    glDetachShader(program,fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}
