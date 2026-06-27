#include "floatwindow.h"
#include <QCloseEvent>

FloatWindow::FloatWindow(GlobalVariables *gVar,
                         QWidget *child,
                         const QString &name,
                         QWidget *parent)
    : QMainWindow{parent}
    , gVar(gVar)
    , childWidget(child)
    , name(name)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setStyleSheet("background : " + gVar->getClrBackground() + " ; ");
    setCentralWidget(childWidget);
    childWidget->show();
    setWindowTitle(name);
    resize(800, 600);
}

void FloatWindow::closeEvent(QCloseEvent *event) {
    QWidget* child = this->takeCentralWidget();
    // setCentralWidget(nullptr);
    // if (child) {
    //     setCentralWidget(nullptr);
    //     child->setParent(nullptr);
    // }
    emit closed(child);
    QMainWindow::closeEvent(event);
}
