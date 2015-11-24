#include "../include/compound.h"
#include "../include/renderer.h"

#include <iostream>
#include <GL/freeglut.h>

using namespace std;

int main(int argc, char *argv[])
{
    // create random compound graph
    Compound com = Compound::create_random(2, 6, 2, 10);

    // mutate root node
    NodeId root = com.get_root_id();
    com.get_node(root)->set_label("Root");
    com.get_node(root)->set_position(Point2D(0.2, 1.5));

    // print compound graph
    cout << com.to_string() << endl;

    if(!Renderer::initGLUT(argc,argv,512,512))
    {
        exit(EXIT_FAILURE);
    }

    Renderer::initProgram();
    Renderer::run();
    return 0;
}

