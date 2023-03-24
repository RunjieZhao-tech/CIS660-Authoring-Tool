//Quadrangulation Reducing Points

#include <iostream>
#include <half_edge.h>
#include <glm/glm.hpp>
void reducePoints(int input[],int times);
void Test();

int main()
{
    Test();
}

//Each index in the input represents the number of edges in one side
//Now we suppose it is a triangle which means that there are at most 3 values in the array
//The length of input must be less than or equal to 6
void reducePoints(int input[], int times) {
    for (int i = 0; i < 3; i++) {
        if (input[i] == 1 || input[(i + 2) % 3] == 1) {
            continue;
        }
        int d = std::min(input[i],input[(i + 2) % 3]) - 1;
        input[i] -= d;
        input[(i + 2) % 3] -= d;
        std::cout << "formed quadrangulation: " << d << " " << input[(i+1)%3] << std::endl;
        std::cout << "rest input("<< times <<"): " << input[0] << " " << input[1] << " " << input[2] << std::endl;
    }
    int count = 0;
    for (int i = 0; i < 3; i++) {
        if (input[i] == 1) {
            count++;
        }
    }
    if (count >= 2) {
        return;
    }
    else {
        reducePoints(input,times+1);
    }
    return;
}

//create a triangle and decompose it into several pieces.
void Test(){
    //create three vertices
    Vertex* v1 = new Vertex();
    v1->pos = glm::vec3(0, 0, 0);
    Vertex* v2 = new Vertex();
    v2->pos = glm::vec3(1, 0, 0);
    Vertex* v3 = new Vertex();
    v3->pos = glm::vec3(0, 1, 0);
    std::cout << v1->pos.x << " " << v1->pos.y << " " << v1->pos.z << std::endl;

    std::vector<Vertex*> vertex_arr;
    vertex_arr.push_back(v1);
    vertex_arr.push_back(v2);
    vertex_arr.push_back(v3);

    //create edges
    std::vector<HalfEdge*> edge_arr;
    HalfEdge* he1 = new HalfEdge();
    he1->setVert(v1);
    HalfEdge* he2 = new HalfEdge();
    he2->setVert(v2);
    HalfEdge* he3 = new HalfEdge();
    he3->setVert(v3);

    he1->setNextEdge(he2);
    he2->setNextEdge(he3);
    he3->setNextEdge(he1);

    Face* face = new Face();
    face->setEdge(he1);


}
