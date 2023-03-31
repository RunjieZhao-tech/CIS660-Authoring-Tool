#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include "camera.h"
#include "scene/display.h"
#include "scene/mesh.h"
#include "scene/skeleton.h"
#include <unordered_set>
#include <unordered_map>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include "smartpointerhelp.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QJsonArray>
#include <QWidgetItem>
#include "dualquat.h"
#include "tile_solver.h"
class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    Patch m_patch;

    QListWidgetItem* m_selectedItem;

    VertexDisplay m_vertDisplay;
    FaceDisplay m_faceDisplay;
    HalfEdgeDisplay m_hEdgeDisplay;
    PatchDisplay m_patchDisplay;

    std::vector<Collider*> m_colliders;

    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_glCamera;

    bool hideSkinDisplay;

//    int drawMode;//decide which program to draw mesh
//    enum modes{FLAT,SKELETON,LAMBERT};


    void updateDisplay();

    void findInitialVerts(int jointId,
                          std::vector<Vertex*>* out_verts,
                          std::unordered_map<Vertex*,float>* out_dists);
    void setShaderTransforms();
public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void changePatch();
    void selectedInfo();
    void printMousePos();
    void rotatePolarCamera(glm::vec2 mouseMove);
protected:
    void keyPressEvent(QKeyEvent *e);
signals:
    void sig_sendVert(QListWidgetItem*);
    void sig_sendFace(QListWidgetItem*);
    void sig_sendHalfEdge(QListWidgetItem*);
public slots:
    void slot_setSelected(QListWidgetItem*);
    void slot_mousePress(QMouseEvent * event);
    void slot_subdivision();
    void slot_tile();
};


#endif // MYGL_H
