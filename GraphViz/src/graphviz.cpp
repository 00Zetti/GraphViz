#include "../include/compound.h"
#include "../include/renderer.h"

#include <iostream>
#include <GL/freeglut.h>

using namespace std;

int main(int argc, char *argv[])
{
    // create random compound graph
    Compound com = Compound::create_random(7, 160, 2, 10);

    // mutate root node
    NodeId root = com.get_root_id();
    com.get_node(root)->set_label("Root");
    com.get_node(root)->set_position(Point2D(0.0, 0.0));

    // print compound graph
    cout << com.to_string() << endl;

    if(!Renderer::initGLUT(argc,argv,512,512))
    {
        exit(EXIT_FAILURE);
    }

    //create, link shaders
    Renderer::initProgram();

    //create buffers
    Renderer::initBuffers();

    //calculate node positions, pass pathes to buffers
    Renderer::parseData(&com);

    Renderer::run();
    return 0;
}

