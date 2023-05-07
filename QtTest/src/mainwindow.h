#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "glm/detail/type_vec.hpp"
#include "qlistwidget.h"
#include <QMainWindow>
#include "la.h"
#include "qtreewidget.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_actionCamera_Controls_triggered();

    void slot_addVertToList(QListWidgetItem*);

    void slot_addFaceToList(QListWidgetItem*);

    void slot_addEdgeToList(QListWidgetItem*);

    void slot_changeRotDisplay(QString str);
signals:
    void sig_mousePress(QMouseEvent*);
    void sig_moveCamera(glm::vec2);
private:
    Ui::MainWindow *ui;
    int timerId;
    bool holdRight;
    glm::vec2 mousePos;
protected:
    void mousePressEvent (QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent *);
    void timerEvent(QTimerEvent *event);
};


#endif // MAINWINDOW_H
