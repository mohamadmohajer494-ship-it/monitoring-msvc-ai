#include "mainlayout.h"

MainLayout::MainLayout(GlobalVariables *gVar, QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
{
    vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    stack = new QStackedWidget(this);
    vLayout->addWidget(stack);
    createWindows();
    for (int var = 0; var < directionPoolSize; ++var) {
        directionPages[var]->getGauge()->setHeading(var * 40);
    }
    int stationID1 = position->addDfStation(35.6892, 51.3890, "DfStation", 45.0);

    slotThemeChange();
    connect(gVar, &GlobalVariables::ThemeChange,
            this, &MainLayout::slotThemeChange);
}

MainLayout::~MainLayout()
{
    for (auto fw : floatWins) {
        disconnect(fw, nullptr, this, nullptr);
        if (QWidget *child = fw->centralWidget()) {
            fw->setCentralWidget(nullptr);
            child->setParent(this);
        }
        fw->deleteLater();
    }
    floatWins.clear();
}

Direction *MainLayout::getDirectionPage(int dfId) const
{
    if (dfId < 0 || dfId >= directionPoolSize)
        return nullptr;
    return reinterpret_cast<Direction*>(directionPages[dfId]);
}

void MainLayout::addPage(const QString &name, QWidget *widget)
{
    if (!widget) {
        qDebug() << "Warning: Cannot add null widget with name:" << name;
        return;
    }
    if (nameToIndex.contains(name)) {
        qDebug() << "Warning: Widget name already exists:" << name;
        return;
    }
    int index = widgetsList.size();
    widgetsList << widget;
    stack->addWidget(widget);
    nameToIndex[name] = index;
    indexToName[index] = name;
}

void MainLayout::showWidget(QString name)
{
    if (!nameToIndex.contains(name)) {
        qDebug() << "Warning: Widget" << name << "not found.";
        return;
    }
    int index = nameToIndex[name];
    if (floatWins.contains(index)) {
        qDebug() << "Warning: Widget" << name << "is detached. Close its window first.";
        return;
    }
    if (index < 0 || index >= widgetsList.size()) {
        qDebug() << "Warning: Invalid widget index for" << name;
        return;
    }
    QWidget *page = widgetsList[index];
    if (!page) {
        qDebug() << "Warning: Widget page is null for" << name;
        return;
    }
    stack->setCurrentWidget(page);
    updatePagesRefreshRate();
    emit setActiveView(name);
}

void MainLayout::detachWidget(QString name)
{
    if (!nameToIndex.contains(name)) {
        qDebug() << "Widget not found:" << name;
        return;
    }
    int index = nameToIndex[name];
    if (floatWins.contains(index)) {
        qDebug() << "Widget already detached:" << name;
        return;
    }
    if (floatWins.size() >= maxDetached) {
        qDebug() << "Limit reached";
        return;
    }
    if (index < 0 || index >= widgetsList.size()) {
        qDebug() << "Invalid widget index:" << index;
        return;
    }
    QWidget *page = widgetsList[index];
    if (!page) {
        qDebug() << "Widget page is null:" << name;
        return;
    }
    if (stack->currentWidget() == page) {
        for (int i = 0; i < widgetsList.size(); ++i) {
            if (i != index && !floatWins.contains(i)) {
                stack->setCurrentWidget(widgetsList[i]);
                break;
            }
        }
    }
    stack->removeWidget(page);
    page->setParent(nullptr);
    auto *fw = new FloatWindow(gVar, page, indexToName[index], this);
    emit setOutView(name);
    floatWins[index] = fw;
    connect(fw, &FloatWindow::closed,
            this,
            [this, index, name](QWidget *child) {
                if (!child) {
                    return;
                }
                if (child->parentWidget()) {
                    child->setParent(nullptr);
                }
                if (stack->indexOf(child) == -1) {
                    stack->addWidget(child);
                }
                child->update();
                floatWins.remove(index);
                updatePagesRefreshRate();
                emit setInView(name);
            },
            Qt::QueuedConnection);
    fw->show();
    updatePagesRefreshRate();
    QMetaObject::invokeMethod(page, "update", Qt::QueuedConnection);
}

void MainLayout::slotThemeChange()
{

}

void MainLayout::createWindows()
{
    widgetsList.clear();
    nameToIndex.clear();
    indexToName.clear();
    overview = new Overview(gVar);
    addPage("Overview", overview);
    panorama = new Panorama(gVar);
    addPage("Panorama", panorama);
    analysis = new Analysis(gVar);
    addPage("Analysis", analysis);
    position = new Position(gVar);
    addPage("Position", position);
    handoff = new Handoff(gVar);
    addPage("Handoff", handoff);
    for (int var = 0; var < directionPoolSize; ++var) {
        directionPages[var] = new Direction(gVar);
        QString name = QString("Direction%1").arg(var + 1, 2, 10, QChar('0'));
        addPage(name, directionPages[var]);
    }
    stack->setCurrentWidget(overview);
}

void MainLayout::updatePagesRefreshRate()
{
    for (int i = 0; i < widgetsList.size(); ++i) {
        QWidget *page = widgetsList[i];
        if (!page)
            continue;
        BaseWindows *bw = qobject_cast<BaseWindows*>(page);
        if (!bw)
            continue;
        const bool visibleToUser = isPageVisibleToUser(i);
        bw->setViewportFps(visibleToUser ? 40 : 1);
    }
}

bool MainLayout::isPageVisibleToUser(int index) const
{
    if (index < 0 || index >= widgetsList.size())
        return false;
    QWidget *page = widgetsList[index];
    if (!page)
        return false;
    if (floatWins.contains(index)) {
        FloatWindow *fw = floatWins.value(index);
        if (!fw)
            return false;

        return fw->isVisible()
               && !fw->isMinimized()
               && page->isVisible();
    }
    return stack
           && stack->currentWidget() == page
           && page->isVisible()
           && this->isVisible();
}
