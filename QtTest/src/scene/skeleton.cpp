#include "skeleton.h"
#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QJsonArray>
SkeletonJoint::SkeletonJoint()
    :SkeletonJoint("default",glm::vec3(),glm::quat(),glm::mat4())
{}
SkeletonJoint::SkeletonJoint(std::string name,
                              SkeletonJoint* parent,
                              std::vector<SkeletonJoint*>* children,
                              glm::vec3 pos,
                              glm::quat rot,
                              glm::mat4 bind)
    :parent(parent),prPos(pos),rot(rot),bindMat(bind)
{
    setName(name);
    addChildren(children);
};
SkeletonJoint::SkeletonJoint(std::string name,
                              glm::vec3 pos,
                              glm::quat rot,
                             glm::mat4 bind)
    :SkeletonJoint(name,nullptr,nullptr,pos,rot,bind)
{};
SkeletonJoint::~SkeletonJoint(){
    std::cout << "delete "<< this->getName()<<std::endl;
};

void SkeletonJoint::setName(std::string name){
    setText(0,QString::fromStdString(name));
};
std::string SkeletonJoint::getName()const{
    return QTreeWidgetItem::text(0).toStdString();
};
void SkeletonJoint::addChild(SkeletonJoint* child){
    children.push_back(child);
    QTreeWidgetItem::addChild(child);
    if(this!=child->parent){
        child->parent = this;
    }
};
void SkeletonJoint::addChildren(std::vector<SkeletonJoint*>* children){
    if(children){
        for(SkeletonJoint *child:*children){
            addChild(child);
        }
    }
};
const std::vector<SkeletonJoint*>* SkeletonJoint::getChildren()const{
    return &(this->children);
};

glm::mat4 SkeletonJoint::getLocalTransform()const{
    glm::mat4 res = glm::mat4_cast(rot);
    res[3] = glm::vec4(prPos,1);
    return res;
};
glm::mat4 SkeletonJoint::getOverallTransform()const{
    if(parent){
        return parent->getOverallTransform()*getLocalTransform();
    }else{
        return getLocalTransform();
    }
};

Skeleton::Skeleton(OpenGLContext* mp_context)
    :Drawable(mp_context),m_root(nullptr)
{}

void Skeleton::createCircle(
        std::vector<GLuint>* out_ids,
        std::vector<glm::vec4>* out_col,
        std::vector<glm::vec4>* out_pos){
    int beginId = out_col->size();
    glm::vec4 base(0.5f,0,0,1);
    int edgeNum = 12;
    glm::mat4 rot = glm::rotate(glm::mat4(1.f),glm::radians(360.f/edgeNum),glm::vec3(0,0,1));
    for(int i=0;i<edgeNum;i++){
        out_col->push_back(glm::vec4());
        out_pos->push_back(base);
        out_ids->push_back(beginId+i);
        if((i+1)==edgeNum){
            out_ids->push_back(beginId);
        }else{
            out_ids->push_back(beginId+i+1);
        }
        base = rot*base;
    }
}
void Skeleton::createJointDisplay(
        const SkeletonJoint* currJoint,
        glm::mat4 overallMat,
        std::vector<GLuint>* out_ids,
        std::vector<glm::vec4>* out_col,
        std::vector<glm::vec4>* out_pos){
    //draw self
    glm::mat4 rotRed = overallMat*glm::rotate(glm::mat4(1.f),glm::radians(90.f),glm::vec3(0,1,0));
    glm::mat4 rotGreen = overallMat*glm::rotate(glm::mat4(1.f),glm::radians(90.f),glm::vec3(1,0,0));
    glm::mat4 rotBlue = overallMat;
    int beginId = out_col->size();
    int endId = 0;
    createCircle(out_ids,out_col,out_pos);
    endId = out_col->size();
    for(int i=beginId;i<endId;i++){
        (*out_col)[i] = glm::vec4(0,0,1,1);
        (*out_pos)[i] = rotBlue*(*out_pos)[i];
    }

    beginId = endId;
    createCircle(out_ids,out_col,out_pos);
    endId = out_col->size();
    for(int i=beginId;i<endId;i++){
        (*out_col)[i] = glm::vec4(1,0,0,1);
        (*out_pos)[i] = rotRed*(*out_pos)[i];
    }

    beginId = endId;
    createCircle(out_ids,out_col,out_pos);
    endId = out_col->size();
    for(int i=beginId;i<endId;i++){
        (*out_col)[i] = glm::vec4(0,1,0,1);
        (*out_pos)[i] = rotGreen*(*out_pos)[i];
    }

    //draw children
    const std::vector<SkeletonJoint*>* children = currJoint->getChildren();
    for(SkeletonJoint* child:*children){
        beginId = endId;
        //draw line between parent(this) and child
        out_ids->push_back(beginId);
        out_ids->push_back(beginId+1);
        out_col->push_back(glm::vec4(0,0,0,1));
        out_col->push_back(glm::vec4(1,1,1,1));
        glm::mat4 newOverallMat = overallMat*child->getLocalTransform();
        out_pos->push_back(overallMat[3]);
        out_pos->push_back(newOverallMat[3]);
        createJointDisplay(child,newOverallMat,out_ids,out_col,out_pos);
        endId = out_col->size();
    }
}
void Skeleton::create(){
    std::vector<GLuint> glIndices;
    std::vector<glm::vec4> glPos;
    std::vector<glm::vec4> glCol;
    if(m_root!=nullptr){
        createJointDisplay(m_root,m_root->getOverallTransform(),&glIndices,&glCol,&glPos);
    }

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, glIndices.size() * sizeof(GLuint), glIndices.data(), GL_STATIC_DRAW);
    count = glIndices.size();
    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glPos.size() * sizeof(glm::vec4), glPos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glCol.size() * sizeof(glm::vec4), glCol.data(), GL_STATIC_DRAW);

}
GLenum Skeleton::drawMode() {
    return GL_LINES;
}
void Skeleton::createJoint(const QJsonObject& obj,SkeletonJoint* parent){
    if(!obj.contains("name")){
        qDebug()<<"error in json";
        return;
    }
    std::string name = obj["name"].toString().toStdString();
    std::cout << "create joint "<<name<<std::endl;
    QJsonArray posArray = obj["pos"].toArray();
    QJsonArray rotArray = obj["rot"].toArray();
    QJsonArray childrenArray = obj["children"].toArray();
    glm::vec3 pos(posArray[0].toDouble(),posArray[1].toDouble(),posArray[2].toDouble());
    float angle = rotArray[0].toDouble()/2.f;
    float cos = std::cos(angle);
    float sin = std::sin(angle);
    glm::quat rot(cos,sin*rotArray[1].toDouble(),sin*rotArray[2].toDouble(),sin*rotArray[3].toDouble());
//    SkeletonJoint(std::string name,
//                  glm::vec3 pos,
//                  glm::quat rot,
//                  glm::mat4 bind);
    uPtr<SkeletonJoint> newJoint = mkU<SkeletonJoint>(name,pos,rot,glm::mat4());
    SkeletonJoint* jPtr = newJoint.get();
    if(parent!=nullptr){
        parent->addChild(jPtr);
        jPtr->bindMat = glm::inverse(jPtr->getLocalTransform())* parent->bindMat ;
    }else{
        jPtr->bindMat = glm::inverse(jPtr->getLocalTransform());
    }
    //have to do this, let parent store behind children
    //because the ~TreeWidget() will delete all the children
    //and we the vector is released from begin to end
    //we will get all elements that after the root points to null
    for(int i=0;i<childrenArray.count();i++){
        createJoint(childrenArray[i].toObject(),jPtr);
    }
    joints.push_back(std::move(newJoint));
}
void Skeleton::createFromJSON(const char* filename){
    destroy();
    clearSkeleton();
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open json file.");
        return;
    }
    QByteArray bArray = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(bArray));
    QJsonObject obj = doc.object();
    if(obj.contains("root")){
       createJoint(obj["root"].toObject(),nullptr);
    }else{
        qDebug()<<"error in createSkeletonFromJSON";
    }
    if(!joints.empty()){
        m_root = joints.back().get();
    }else{
        qDebug()<<"empty skeleton";
    }
    create();
}
void Skeleton::clearSkeleton(){
    joints.clear();
}
