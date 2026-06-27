#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
    QSurfaceFormat fmt;
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    fmt.setVersion(3, 2);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setSwapInterval(1);
    fmt.setSamples(0);
    QSurfaceFormat::setDefaultFormat(fmt);
    QApplication app(argc, argv);
    QScopedPointer<GlobalVariables> gVar(new GlobalVariables());
    MainWindow window(gVar.data());
    window.setWindowTitle("Monitoring");
    window.setWindowIcon(QIcon(":/Res/Images/Logo.png"));
    window.showMaximized();
    return app.exec();
}
