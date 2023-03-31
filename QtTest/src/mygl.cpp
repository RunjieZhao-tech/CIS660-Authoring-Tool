#include "mygl.h"
#include "scene/mesh.h"
#include <la.h>
#include <bits/stdc++.h>
#include <iostream>
#include <QApplication>
#include <QKeyEvent>

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_patch(),
      m_selectedItem(nullptr),
      m_vertDisplay(this),
      m_faceDisplay(this),
      m_hEdgeDisplay(this),
      m_patchDisplay(this),
      m_progLambert(this), m_progFlat(this),
      m_glCamera(),hideSkinDisplay(true)
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_vertDisplay.destroy();
    m_faceDisplay.destroy();
    m_hEdgeDisplay.destroy();
    m_patchDisplay.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    m_vertDisplay.create();
    m_faceDisplay.create();
    m_hEdgeDisplay.create();
    m_patchDisplay.create();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    //const char* json = "E:/GitStorage/CIS4600/homework-05-06-07-micro-maya-mesh-editor-LichengCAO/jsons/cow_skeleton.json";
    //const char* obj = "E:/GitStorage/CIS4600/homework-05-06-07-micro-maya-mesh-editor-LichengCAO/obj_files/cow.obj";
    //changeOBJ(obj);
    //changeSkeleton(json);
    m_patch.quadrangulate();
    changePatch();
    m_patchDisplay.updatePatch(&m_patch);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Create a model matrix. This one rotates the square by PI/4 radians then translates it by <-2,0,0>.
    //Note that we have to transpose the model matrix before passing it to the shader
    //This is because OpenGL expects column-major matrices, but you've
    //implemented row-major matrices.
    m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
    m_progFlat.setModelMatrix(glm::mat4(1.f));

    m_progLambert.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setModelMatrix(glm::mat4(1.f));
    m_progLambert.setCamPos(m_glCamera.eye);

    //draw displays
    glDisable(GL_DEPTH_TEST);

    m_progFlat.draw(m_patchDisplay);
    m_progFlat.draw(m_vertDisplay);
    m_progFlat.draw(m_faceDisplay);
    m_progFlat.draw(m_hEdgeDisplay);
    glEnable(GL_DEPTH_TEST);
}
void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    HalfEdge* eptr= dynamic_cast<HalfEdge*>(m_selectedItem);
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used
    switch(e->key()){
    case Qt::Key_Escape:
        QApplication::quit();
        break;
    case Qt::Key_Right:
        m_glCamera.RotateAboutUp(-amount);
        break;
    case Qt::Key_Left:
        m_glCamera.RotateAboutUp(amount);
        break;
    case Qt::Key_Up:
        m_glCamera.RotateAboutRight(-amount);
        break;
    case Qt::Key_Down:
        m_glCamera.RotateAboutRight(amount);
        break;
    case Qt::Key_1:
        m_glCamera.fovy += amount;
        break;
    case Qt::Key_2:
        m_glCamera.fovy -= amount;
        break;
//    case Qt::Key_W:
//        m_glCamera.TranslateAlongLook(amount);
//        break;
//    case Qt::Key_S:
//        m_glCamera.TranslateAlongLook(-amount);
//        break;
    case Qt::Key_W:
        m_glCamera.PolarZoom(amount);
        break;
    case Qt::Key_S:
        m_glCamera.PolarZoom(-amount);
        break;
    case Qt::Key_D:
        m_glCamera.TranslateAlongRight(amount);
        break;
    case Qt::Key_A:
        m_glCamera.TranslateAlongRight(-amount);
        break;
    case Qt::Key_Q:
        m_glCamera.TranslateAlongUp(-amount);
        break;
    case Qt::Key_E:
        m_glCamera.TranslateAlongUp(amount);
        break;
    case Qt::Key_R:
        m_glCamera = Camera(this->width(), this->height());
        break;
    case Qt::Key_N:
       //N: NEXT half-edge of the currently selected half-edge
       if(eptr!=nullptr){
          slot_setSelected(eptr->nHEdge);
       }
       break;
    case Qt::Key_M:
        //M: SYM half-edge of the currently selected half-edge
        if(eptr!=nullptr){
           slot_setSelected(eptr->sHEdge);
        }
        break;
    case Qt::Key_F:
        //F: FACE of the currently selected half-edge
        if(eptr!=nullptr){
           slot_setSelected(eptr->getFace());
        }
        break;
    case Qt::Key_V:
        //V: VERTEX of the currently selected half-edge
        if(eptr!=nullptr){
           slot_setSelected(eptr->vert);
        }
        break;
    case Qt::Key_H:
        if(e->modifiers() & Qt::ShiftModifier){
            //Shift + H: HALF-EDGE of the currently selected face
            Face* fptr = dynamic_cast<Face*>(m_selectedItem);
            if(fptr!=nullptr){
                slot_setSelected(fptr->hEdge);
            }
        } else {
            //H: HALF-EDGE of the currently selected vertex
            Vertex* vptr = dynamic_cast<Vertex*>(m_selectedItem);
            if(vptr!=nullptr){
                slot_setSelected(vptr->hEdge);
            }
        }
        break;
     case Qt::Key_I:
        selectedInfo();
        break;
    }
    m_glCamera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::changePatch(){
    m_colliders.clear();
    slot_setSelected(nullptr);
    for(unsigned long i=0;i<m_patch.verts.size();i++){
        m_colliders.push_back(m_patch.verts[i].get());
    }
    for(unsigned long i=0;i<m_patch.quads.size();i++){
        m_colliders.push_back(m_patch.quads[i].get());
    }
    for(unsigned long i=0;i<m_patch.hEdges.size();i++){
        m_colliders.push_back(m_patch.hEdges[i].get());
    }
    for(unsigned long i=0;i<m_patch.quads.size();i++){
        sig_sendFace(m_patch.quads[i].get());
    }
    for(unsigned long i=0;i<m_patch.verts.size();i++){
        sig_sendVert(m_patch.verts[i].get());
    }
    for(unsigned long i=0;i<m_patch.hEdges.size();i++){
        sig_sendHalfEdge(m_patch.hEdges[i].get());
    }
    update();
}

void MyGL::updateDisplay(){
    Face* fPtr = dynamic_cast<Face*>(m_selectedItem);
    Vertex* vPtr = dynamic_cast<Vertex*>(m_selectedItem);
    HalfEdge* ePtr = dynamic_cast<HalfEdge*>(m_selectedItem);
    m_vertDisplay.updateVertex(vPtr);
    m_faceDisplay.updateFace(fPtr);
    m_hEdgeDisplay.updateHalfEdge(ePtr);

//    SkeletonJoint* skPtr = dynamic_cast<SkeletonJoint*>(m_selectedJoint);
//    m_jointDisplay.updateJoint(skPtr);
//    if(!hideSkinDisplay){
//        if(skPtr==nullptr){
//           m_skinDisplay.updateMeshJoint(&m_mesh,-1);
//        }else{
//           for(unsigned int i=0;i<m_skeleton.joints.size();i++){
//               if(skPtr == m_skeleton.joints[i].get()){
//                  m_skinDisplay.updateMeshJoint(&m_mesh,i);
//               }
//           }
//        }
//    }
}
void MyGL::selectedInfo(){
    Face* fPtr = dynamic_cast<Face*>(m_selectedItem);
    HalfEdge* hPtr = dynamic_cast<HalfEdge*>(m_selectedItem);
    Vertex* vPtr = dynamic_cast<Vertex*>(m_selectedItem);
    if(fPtr!=nullptr){
        std::cout<<"current face: " << fPtr->id<<std::endl;
        std::cout<<"its vertices: ";
        std::vector<Vertex*> verts = fPtr->getVertices();
        std::vector<HalfEdge*> edges = fPtr->getHalfEdges();
        for(Vertex* vert:verts){
            std::cout<<vert->id<<"/ ";
        }
        std::cout<<std::endl;
        std::cout<<"its halfedges: ";
        for(HalfEdge* edge:edges){
            std::cout<<edge->id<<"/ ";
        }
        std::cout<<std::endl;
        std::cout<<std::endl;
    }else if(hPtr!=nullptr){
        std::cout<<"current halfedge: "<<hPtr->id<<std::endl;
        std::cout <<"its SYM: "<<hPtr->sHEdge->id<<std::endl;
        std::cout <<"its face: "<<hPtr->face->id<<std::endl;
        std::cout <<"its vertex: "<<hPtr->vert->id<<std::endl;
    }else if(vPtr!=nullptr){
        std::cout<<"current vertex: "<<vPtr->id<<std::endl;
        std::cout.precision(3);
        std::cout <<"its position: "
                  <<vPtr->pos.x
                  <<", "<< vPtr->pos.y
                  <<", "<< vPtr->pos.z
                  <<std::endl;
        for(int i =0;i<vPtr->jointIds.size();i++){
            if(vPtr->jointIds[i]==-1)continue;
            //std::cout<<"joint: "<<m_skeleton.joints[vPtr->jointIds[i]].get()->getName()<<" weight: "<<vPtr->jointWeights[i]<<std::endl;
        }
    }
}
void MyGL::printMousePos(){
    QPoint pt = mapFromGlobal(QCursor::pos());
    qDebug()<< pt.x()<<", "<<pt.y();
}
void MyGL::rotatePolarCamera(glm::vec2 mouseMove){
    mouseMove /= 5.f;
    m_glCamera.PolarMoveHorizontal(mouseMove.x);
    m_glCamera.PolarMoveVerticle(mouseMove.y);
    update();
};

void MyGL::slot_setSelected(QListWidgetItem* i){
    //update list widget
    if(m_selectedItem!=nullptr){
        m_selectedItem->setSelected(false);
    }
    m_selectedItem = i;
    if(m_selectedItem!=nullptr){
       m_selectedItem->setSelected(true);
    }
    //update display
    updateDisplay();

    update();
}
void MyGL::slot_mousePress(QMouseEvent* event){
    HitInfo info;
    //since mygl is not at top left
    QPoint pt = mapFromGlobal(QCursor::pos());
    m_glCamera.RayCast(glm::vec2(pt.x(),pt.y()),&info,&m_colliders);
    QListWidgetItem* item = dynamic_cast<QListWidgetItem*>(info.collider);
    slot_setSelected(item);
    //printMousePos();
};

void MyGL::slot_subdivision(){
    HalfEdge* ePtr = dynamic_cast<HalfEdge*>(m_selectedItem);
    if(ePtr!=nullptr){
        m_patch.subDivide(ePtr,3);
        m_patchDisplay.updatePatch(&m_patch);
        changePatch();
    }
}

void MyGL::slot_tile(){
    Patch* p = &m_patch;
    if(p==nullptr)return;
    L3Tile l = L3Tile();
    L2Tile l1 = L2Tile();
    SquareTile l2 = SquareTile();
    LTile l3 = LTile();
    std::vector<Tile*> tiles;
    tiles.push_back(&l);
    tiles.push_back(&l1);
    tiles.push_back(&l2);
    tiles.push_back(&l3);
    std::vector<Face*> quad;
    for(auto&& q:p->quads){
        quad.push_back(q.get());
    }

    TileSolver ts = TileSolver();
    std::vector<std::vector<Face*>> tiling;
    tiling = ts.solveTiling(quad,tiles);
    for(int i=0;i<tiling.size();i++){
        glm::vec3 color = glm::vec3(rand()%100/100.f,rand()%100/100.f,rand()%100/100.f);
        for(Face* q:tiling[i]){
            q->color = color;
        }
    }
    m_patchDisplay.updatePatch(&m_patch);
    update();
}
