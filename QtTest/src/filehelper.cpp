#include "filehelper.h"
#include <iostream>
#include <vector>
#include <QTextStream>
#include <QFile>
#include <la.h>
//based on tiny_obj_loader
void readFile(const char* filename, std::vector<glm::vec3>* vs,std::vector<std::vector<int>>* fs){
    //read position and ids of positions in faces
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        std::cout<<"Cannot open file"<<std::endl;
        return;
    }
    QTextStream ifs(&file);
    QString buf;
    while(!ifs.atEnd()){
        buf = ifs.readLine();
        std::string lineBuf(buf.toStdString());
        if(lineBuf.size()>0){
            if(lineBuf[lineBuf.size()-1]=='\n')lineBuf.erase(lineBuf.size()-1);
        }
        if(lineBuf.size()>0){
            if(lineBuf[lineBuf.size()-1]=='\r')lineBuf.erase(lineBuf.size()-1);
        }
        if(lineBuf.empty()){
            continue;
        }
        const char *s = lineBuf.c_str();
        //remove space
        s += strspn(s," \t");
        //make sure s is valid
        assert(s);
        //ignore empty/comment
        if(s[0]=='\0')continue;
        if(s[0]=='#')continue;
        //vertex pos
        if(s[0]=='v'&&(s[1]==' '||s[1]=='\t')){
            s+=2;
            glm::vec3 v(0,0,0);
            std::vector<float> res;
            readV(s,&res);
            for(int i=0;i<3;i++){
                v[i] = res[i];
            }
            vs->push_back(v);
            continue;
        }
        //face
        if(s[0]=='f'&&(s[1]==' '||s[1]=='\t')){
            s+=2;
            s += strspn(s," \t");
            std::vector<int> f;
            readF(s,&f);
            fs->push_back(f);
            continue;
        }
    }
};
void readV(const char* s,std::vector<float> *res){
    //go to next item
    s += strspn(s," \t");
    bool endLine = (s[0]=='\n'||s[0]=='\0'||s[0]=='\r');
    while(!endLine){
        const char* end = s + strcspn(s," \t\r");
        std::string str;
        str.assign(s,end);
        res->push_back(std::stof(str));
        s = end;
        //go to next item
        s += strspn(s," \t");
        endLine = s[0]=='\n'||s[0]=='\0'||s[0]=='\r';
    }
};
void readF(const char* s,std::vector<int> *res){
    //go to next item
    s += strspn(s," \t");
    bool endLine = (s[0]=='\n'||s[0]=='\0'||s[0]=='\r');
    while(!endLine){
        //only consider v/vt/vn
        const char* end = s + strcspn(s,"/");
        std::string str;
        str.assign(s,end);
        res->push_back(std::stoi(str)-1);//to match the v

        s = end + 1;
        end = s + strcspn(s,"/");
        s = end + 1;
        s += strcspn(s," \t\r");
        s += strspn(s," \t");
        endLine = (s[0]=='\n'||s[0]=='\0'||s[0]=='\r');
    }
};
