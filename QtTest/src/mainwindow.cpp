#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include "qevent.h"
#include <iostream>
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    holdRight(false),
    mousePos(glm::vec2(QCursor::pos().x(),QCursor::pos().y()))
{
    ui->setupUi(this);
    ui->mygl->setFocus();
    timerId = startTimer(20);
    //update the widget and show items
    connect(ui->mygl,
            SIGNAL(sig_sendVert(QListWidgetItem*)),
            this,
            SLOT(slot_addVertToList(QListWidgetItem*)));
    connect(ui->mygl,
            SIGNAL(sig_sendFace(QListWidgetItem*)),
            this,
            SLOT(slot_addFaceToList(QListWidgetItem*)));
    connect(ui->mygl,
            SIGNAL(sig_sendHalfEdge(QListWidgetItem*)),
            this,
            SLOT(slot_addEdgeToList(QListWidgetItem*)));
    //handle widget when click listed objects in widget list
    connect(ui->facesListWidget,
            SIGNAL(itemClicked(QListWidgetItem*)),
            ui->mygl,
            SLOT(slot_setSelected(QListWidgetItem*)));
    connect(ui->vertsListWidget,
            SIGNAL(itemClicked(QListWidgetItem*)),
            ui->mygl,
            SLOT(slot_setSelected(QListWidgetItem*)));
    connect(ui->halfEdgesListWidget,
            SIGNAL(itemClicked(QListWidgetItem*)),
            ui->mygl,
            SLOT(slot_setSelected(QListWidgetItem*)));
    //handle raycast
    connect(this,
            SIGNAL(sig_mousePress(QMouseEvent*)),
            ui->mygl,
            SLOT(slot_mousePress(QMouseEvent*)));
}

MainWindow::~MainWindow()
{
    killTimer(timerId);
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}
void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}
void MainWindow::slot_addVertToList(QListWidgetItem* i){
    ui->vertsListWidget->addItem(i);
};
void MainWindow::slot_addFaceToList(QListWidgetItem* i){
    ui->facesListWidget->addItem(i);
};
void MainWindow::slot_addEdgeToList(QListWidgetItem* i){
    ui->halfEdgesListWidget->addItem(i);
};

void MainWindow::mousePressEvent (QMouseEvent * event ){
    switch(event->button()){
    case Qt::LeftButton:
        emit sig_mousePress(event);
        break;
    case Qt::RightButton:
        holdRight = true;
        break;
    }
};
void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::RightButton){
        holdRight = false;
    }
};
void MainWindow::timerEvent(QTimerEvent *event){
    //ui->mygl->slot_printMousePos();
    glm::vec2 currMouse(QCursor::pos().x(),QCursor::pos().y());
    if(holdRight){
        ui->mygl->rotatePolarCamera(mousePos - currMouse);
    }
    mousePos = currMouse;
}
void MainWindow::slot_changeRotDisplay(QString str){
    ui->quatDisplay->setText(str);
};
