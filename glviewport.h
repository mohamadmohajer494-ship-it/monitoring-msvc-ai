#ifndef GLVIEWPORT_H
#define GLVIEWPORT_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QElapsedTimer>
#include <QColor>
#include <QList>
#include <functional>
#include <vector>
#include "Includes/globalvariables.h"
#include "imgui.h"
#include "implot.h"
#include "imgui_impl_opengl3.h"

class GLViewport : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLViewport(GlobalVariables *gVar ,QWidget *parent = nullptr);
    ~GLViewport();

    using RenderFunc = std::function<void()>;

    enum class RefreshPolicy { Auto, OnDemand };

    void setTargetFps(int fps);
    void setRefreshPolicy(RefreshPolicy p);
    RefreshPolicy refreshPolicy() const { return m_policy; }
    void addRenderer(const RenderFunc& fn) { m_renderers.push_back(fn); }
    void clearRenderers() { m_renderers.clear(); }
    int  targetFps() const { return m_targetFps; }
    ImGuiContext*  imguiContext()  const { return m_imgui; }
    ImPlotContext* implotContext() const { return m_implot; }

    void setDebugName(const QString& name) { m_debugName = name; }
    QString debugName() const { return m_debugName; }

    static void dumpAllFps();

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private slots:
    void onContextAboutToBeDestroyed();
    void onThrottleTimeout();

public slots:
    void requestRepaint();

signals:
    void ready();

private:
    GlobalVariables *gVar;
    ImGuiContext*  m_imgui = nullptr;
    ImPlotContext* m_implot = nullptr;
    bool m_backendReady = false;
    std::vector<RenderFunc> m_renderers;
    QMetaObject::Connection m_ctxConn = {};
    bool m_ctxAlive = false;
    QTimer* m_throttleTimer = nullptr;
    QElapsedTimer m_clock;
    qint64 m_lastPaintNs = 0;
    int m_targetFps = 60;
    qint64 m_targetFrameNs = 0;
    bool m_repaintPending = false;
    RefreshPolicy m_policy = RefreshPolicy::Auto;

    QString m_debugName = QStringLiteral("GLViewport");

    static QList<GLViewport*>& instances();

    void drawGrid();
    void cleanupBackend(bool haveGL);
    void connectContextSignals();
    void clearImGuiBackendFlagsNoGL();
    bool isRenderable() const;
};

#endif // GLVIEWPORT_H
