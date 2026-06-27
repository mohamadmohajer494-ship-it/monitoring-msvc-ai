#include "glviewport.h"
#include <QDebug>

static ImGuiKey ImGuiKeyFromQt(int qtKey)
{
    switch (qtKey) {
    case Qt::Key_Tab:        return ImGuiKey_Tab;
    case Qt::Key_Left:       return ImGuiKey_LeftArrow;
    case Qt::Key_Right:      return ImGuiKey_RightArrow;
    case Qt::Key_Up:         return ImGuiKey_UpArrow;
    case Qt::Key_Down:       return ImGuiKey_DownArrow;
    case Qt::Key_PageUp:     return ImGuiKey_PageUp;
    case Qt::Key_PageDown:   return ImGuiKey_PageDown;
    case Qt::Key_Home:       return ImGuiKey_Home;
    case Qt::Key_End:        return ImGuiKey_End;
    case Qt::Key_Insert:     return ImGuiKey_Insert;
    case Qt::Key_Delete:     return ImGuiKey_Delete;
    case Qt::Key_Backspace:  return ImGuiKey_Backspace;
    case Qt::Key_Space:      return ImGuiKey_Space;
    case Qt::Key_Return:
    case Qt::Key_Enter:      return ImGuiKey_Enter;
    case Qt::Key_Escape:     return ImGuiKey_Escape;
    case Qt::Key_A:          return ImGuiKey_A;
    case Qt::Key_C:          return ImGuiKey_C;
    case Qt::Key_V:          return ImGuiKey_V;
    case Qt::Key_X:          return ImGuiKey_X;
    case Qt::Key_Y:          return ImGuiKey_Y;
    case Qt::Key_Z:          return ImGuiKey_Z;
    default:
        return ImGuiKey_None;
    }
}

static QList<GLViewport*> g_viewportInstances;

QList<GLViewport*>& GLViewport::instances()
{
    return g_viewportInstances;
}

GLViewport::GLViewport(GlobalVariables *gVar, QWidget *parent)
    : QOpenGLWidget{parent}
    , gVar(gVar)
{
    setAttribute(Qt::WA_TranslucentBackground, false);
    setAutoFillBackground(false);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    m_throttleTimer = new QTimer(this);
    m_throttleTimer->setSingleShot(true);
    connect(m_throttleTimer, &QTimer::timeout, this, &GLViewport::onThrottleTimeout);
    m_clock.start();
    instances().append(this);
}

GLViewport::~GLViewport()
{
    instances().removeAll(this);
    if (m_ctxConn) {
        QObject::disconnect(m_ctxConn);
        m_ctxConn = {};
    }
    m_ctxAlive = false;
    if (m_throttleTimer)
        m_throttleTimer->stop();
    if (this->isValid()) {
        makeCurrent();
        cleanupBackend(true);
        doneCurrent();
    } else {
        cleanupBackend(false);
    }
    m_renderers.clear();
}

void GLViewport::setTargetFps(int fps)
{
    if (fps <= 0) fps = 1;
    m_targetFps = fps;
    m_targetFrameNs = static_cast<qint64>(1000000000LL / fps);
}

void GLViewport::setRefreshPolicy(RefreshPolicy p)
{
    m_policy = p;
    if (m_policy == RefreshPolicy::Auto)
        requestRepaint();
}

void GLViewport::drawGrid()
{
    ImPlot::StyleColorsDark();
    ImPlotStyle &ips = ImPlot::GetStyle();
    ips.MinorAlpha = 0.55f;
    ips.MajorGridSize = ImVec2(2.0f, 2.0f);
    ips.MinorGridSize = ImVec2(1.2f, 1.2f);
    ips.MajorTickLen = ImVec2(7.0f, 7.0f);
    ips.MinorTickLen = ImVec2(3.0f, 3.0f);
    ImVec4 gridColor = ips.Colors[ImPlotCol_AxisGrid];
    gridColor.w = qMax(gridColor.w, 0.45f);
    gridColor.x = qMin(gridColor.x + 0.1f, 0.8f);
    gridColor.y = qMin(gridColor.y + 0.1f, 0.8f);
    gridColor.z = qMin(gridColor.z + 0.1f, 0.8f);
    ips.Colors[ImPlotCol_AxisGrid] = gridColor;
}

void GLViewport::cleanupBackend(bool haveGL)
{
    if (haveGL && m_backendReady) {
        if (m_imgui)
            ImGui::SetCurrentContext(m_imgui);
        ImGui_ImplOpenGL3_Shutdown();
    } else {
        clearImGuiBackendFlagsNoGL();
    }
    m_backendReady = false;
    if (m_implot) {
        ImPlot::DestroyContext(m_implot);
        m_implot = nullptr;
    }
    if (m_imgui) {
        ImGui::DestroyContext(m_imgui);
        m_imgui = nullptr;
    }
    ImGui::SetCurrentContext(nullptr);
    ImPlot::SetCurrentContext(nullptr);
}

void GLViewport::connectContextSignals()
{
    if (m_ctxConn) {
        QObject::disconnect(m_ctxConn);
        m_ctxConn = {};
    }
    if (QOpenGLContext* ctx = context()) {
        m_ctxConn = QObject::connect(ctx,
                                     &QOpenGLContext::aboutToBeDestroyed,
                                     this,
                                     &GLViewport::onContextAboutToBeDestroyed,
                                     Qt::DirectConnection);
    }
}

void GLViewport::clearImGuiBackendFlagsNoGL()
{
    if (!m_imgui)
        return;
    ImGui::SetCurrentContext(m_imgui);
    ImGuiIO& io = ImGui::GetIO();
    io.BackendRendererUserData = nullptr;
    io.BackendRendererName     = nullptr;
    io.BackendFlags &= ~ImGuiBackendFlags_RendererHasVtxOffset;
    ImGui::SetCurrentContext(nullptr);
}

bool GLViewport::isRenderable() const
{
    if (!m_backendReady || !m_imgui || !m_implot) return false;
    if (width() <= 0 || height() <= 0) return false;
    if (!isVisible()) return false;
    return true;
}

void GLViewport::initializeGL()
{
    initializeOpenGLFunctions();
    cleanupBackend(false);
    m_imgui  = ImGui::CreateContext();
    m_implot = ImPlot::CreateContext();
    ImGui::SetCurrentContext(m_imgui);
    ImPlot::SetCurrentContext(m_implot);
    ImGui_ImplOpenGL3_Init("#version 150");
    m_backendReady = true;
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ImGui::StyleColorsDark();
    drawGrid();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    m_ctxAlive = true;
    connectContextSignals();
    emit ready();
}

void GLViewport::resizeGL(int width, int height)
{
    const float dpr = devicePixelRatioF();
    glViewport(0, 0, int(width * dpr), int(height * dpr));
}

void GLViewport::paintGL()
{
    if (!isRenderable())
        return;
    ImGui::SetCurrentContext(m_imgui);
    ImPlot::SetCurrentContext(m_implot);
    const float dpr = devicePixelRatioF();
    const int   w   = width();
    const int   h   = height();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(dpr, dpr);
    static qint64 prevNs = 0;
    qint64 nowNs = m_clock.nsecsElapsed();
    if (prevNs == 0)
        prevNs = nowNs;
    float dt = float((nowNs - prevNs) / 1e9);
    if (dt <= 0.f || dt > 0.5f)
        dt = 1.f / float(m_targetFps);
    io.DeltaTime = dt;
    prevNs = nowNs;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    QColor bColor(gVar->getClrBackground());
    glClearColor(bColor.redF(), bColor.greenF(), bColor.blueF(), 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::SetNextWindowSize(ImVec2((float)w, (float)h));
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse  | ImGuiWindowFlags_NoBackground;
    if (ImGui::Begin("GLViewportRoot", nullptr, flags)) {
        for (auto& r : m_renderers)
            r();
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    m_lastPaintNs = m_clock.nsecsElapsed();
    if (m_policy == RefreshPolicy::Auto)
        requestRepaint();
}

void GLViewport::mousePressEvent(QMouseEvent *event)
{
    if (!m_imgui) {
        QOpenGLWidget::mousePressEvent(event);
        return;
    }
    ImGui::SetCurrentContext(m_imgui);
    ImGuiIO& io = ImGui::GetIO();
    if (event->button() == Qt::LeftButton)   io.MouseDown[0] = true;
    if (event->button() == Qt::RightButton)  io.MouseDown[1] = true;
    if (event->button() == Qt::MiddleButton) io.MouseDown[2] = true;
    io.MousePos = ImVec2(event->position().x(), event->position().y());
    requestRepaint();
}

void GLViewport::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_imgui) {
        QOpenGLWidget::mouseReleaseEvent(event);
        return;
    }
    ImGui::SetCurrentContext(m_imgui);
    ImGuiIO& io = ImGui::GetIO();
    if (event->button() == Qt::LeftButton)
        io.MouseDown[0] = false;
    if (event->button() == Qt::RightButton)
        io.MouseDown[1] = false;
    if (event->button() == Qt::MiddleButton)
        io.MouseDown[2] = false;
    io.MousePos = ImVec2(event->position().x(), event->position().y());
    requestRepaint();
}

void GLViewport::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_imgui) {
        QOpenGLWidget::mouseMoveEvent(event);
        return;
    }
    ImGui::SetCurrentContext(m_imgui);
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(event->position().x(), event->position().y());
    requestRepaint();
}

void GLViewport::wheelEvent(QWheelEvent *event)
{
    if (!m_imgui) {
        QOpenGLWidget::wheelEvent(event);
        return;
    }
    ImGui::SetCurrentContext(m_imgui);
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel  += event->angleDelta().y() / 120.0f;
    io.MouseWheelH += event->angleDelta().x() / 120.0f;
    event->accept();
    requestRepaint();
}

void GLViewport::keyPressEvent(QKeyEvent *event)
{
    if (!m_imgui) {
        QOpenGLWidget::keyPressEvent(event);
        return;
    }
    ImGui::SetCurrentContext(m_imgui);
    ImGuiIO& io = ImGui::GetIO();
    Qt::KeyboardModifiers mods = event->modifiers();
    io.AddKeyEvent(ImGuiKey_ModCtrl,  (mods & Qt::ControlModifier) != 0);
    io.AddKeyEvent(ImGuiKey_ModShift, (mods & Qt::ShiftModifier)   != 0);
    io.AddKeyEvent(ImGuiKey_ModAlt,   (mods & Qt::AltModifier)     != 0);
#ifdef Qt::MetaModifier
    io.AddKeyEvent(ImGuiKey_ModSuper, (mods & Qt::MetaModifier)    != 0);
#endif
    ImGuiKey key = ImGuiKeyFromQt(event->key());
    if (key != ImGuiKey_None)
        io.AddKeyEvent(key, true);
    QString txt = event->text();
    if (!txt.isEmpty()) {
        QByteArray utf8 = txt.toUtf8();
        io.AddInputCharactersUTF8(utf8.constData());
    }
    if (io.WantCaptureKeyboard)
        event->accept();
    else
        QOpenGLWidget::keyPressEvent(event);
    requestRepaint();
}

void GLViewport::keyReleaseEvent(QKeyEvent *event)
{
    if (!m_imgui) {
        QOpenGLWidget::keyReleaseEvent(event);
        return;
    }
    ImGui::SetCurrentContext(m_imgui);
    ImGuiIO& io = ImGui::GetIO();
    Qt::KeyboardModifiers mods = event->modifiers();
    io.AddKeyEvent(ImGuiKey_ModCtrl,  (mods & Qt::ControlModifier) != 0);
    io.AddKeyEvent(ImGuiKey_ModShift, (mods & Qt::ShiftModifier)   != 0);
    io.AddKeyEvent(ImGuiKey_ModAlt,   (mods & Qt::AltModifier)     != 0);
#ifdef Qt::MetaModifier
    io.AddKeyEvent(ImGuiKey_ModSuper, (mods & Qt::MetaModifier)    != 0);
#endif
    ImGuiKey key = ImGuiKeyFromQt(event->key());
    if (key != ImGuiKey_None)
        io.AddKeyEvent(key, false);
    if (io.WantCaptureKeyboard)
        event->accept();
    else
        QOpenGLWidget::keyReleaseEvent(event);
    requestRepaint();
}

void GLViewport::showEvent(QShowEvent *event)
{
    QOpenGLWidget::showEvent(event);
    requestRepaint();
}

void GLViewport::hideEvent(QHideEvent *event)
{
    QOpenGLWidget::hideEvent(event);
}

void GLViewport::onContextAboutToBeDestroyed()
{
    if (!m_ctxAlive)
        return;
    m_ctxAlive = false;
    makeCurrent();
    cleanupBackend(true);
    doneCurrent();
    if (m_ctxConn) {
        QObject::disconnect(m_ctxConn);
        m_ctxConn = {};
    }
}

void GLViewport::onThrottleTimeout()
{
    requestRepaint();
}

void GLViewport::requestRepaint()
{
    if (!m_backendReady || !isVisible())
        return;
    const qint64 nowNs = m_clock.nsecsElapsed();
    const qint64 delta = nowNs - m_lastPaintNs;
    if (delta >= m_targetFrameNs) {
        if (m_repaintPending)
            return;
        m_repaintPending = true;
        QMetaObject::invokeMethod(this, [this](){
            m_repaintPending = false;
            QOpenGLWidget::update();
        }, Qt::QueuedConnection);
    } else {
        const qint64 remainNs = m_targetFrameNs - delta;
        int ms = int((remainNs + 999999) / 1000000);
        if (ms < 1) ms = 1;
        if (!m_throttleTimer->isActive())
            m_throttleTimer->start(ms);
    }
}

void GLViewport::dumpAllFps()
{
    // qDebug() << "==== GLViewport FPS Dump ====";
    // const auto list = instances();
    // for (GLViewport* vp : list) {
    //     if (!vp)
    //         continue;
    //     qDebug() << "  name =" << vp->debugName()
    //              << "visible =" << vp->isVisible()
    //              << "fps =" << vp->targetFps();
    // }
    // qDebug() << "=============================";
}
