#include "../include/compound.h"
#include "../include/renderer.h"

#include <iostream>
#include <GL/freeglut.h>

using namespace std;

int main(int argc, char *argv[])
{
    // create random compound graph
    Compound com = Compound::create_random(7, 80, 3, 10);

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
    std::cout << "graphviz line 29" << std::endl;
    //create buffers
    Renderer::initBuffers();
    std::cout << "graphviz line 32" << std::endl;
    //calculate node positions, pass pathes to buffers
    Renderer::parseData(&com);
    std::cout << "graphviz line 35" << std::endl;
    Renderer::run();
    return 0;
}

