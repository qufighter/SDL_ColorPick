#pragma once

#include <QQuickView>
#include <QQmlContext>

//#include "../LinuxFileChooser.h" // not really needed(sort of circiular...)


class ColorPickQtWindow : public QQuickView
{
    //Q_OBJECT
    // Q_PROPERTY(bool annotating READ isAnnotating WRITE setAnnotating NOTIFY annotatingChanged FINAL)
    // Q_PROPERTY(qreal logicalX READ logicalX NOTIFY logicalXChanged)
    // Q_PROPERTY(qreal logicalY READ logicalY NOTIFY logicalYChanged)

    // Q_OBJECT
    // Q_PROPERTY(QScreen *screenToFollow READ screenToFollow NOTIFY screenToFollowChanged FINAL)

public:
    // explicit ColorPickQtWindow();
    // explicit ColorPickQtWindow(QQmlEngine *engine = nullptr, QWindow *parent = nullptr);
    explicit ColorPickQtWindow();

    // using UniquePointer = std::unique_ptr<ColorPickQtWindow, void (*)(ColorPickQtWindow *)>;

    // static UniquePointer makeUnique();

    // static QList<ColorPickQtWindow *> instances();


    QScreen* screenToFollow() const;

    //Q_SIGNALS:
    void screenToFollowChanged();

    //static QList<ColorPickQtWindow *> instances();
    /**
     * Round value to be physically pixel perfect, based on the device pixel ratio.
     * Meant to be used with coordinates, line widths and shape sizes.
     * This is meant to be used in QML.
     */
    // Q_INVOKABLE qreal dprRound(qreal value) const;
    // Q_INVOKABLE qreal dprCeil(qreal value) const;
    // Q_INVOKABLE qreal dprFloor(qreal value) const;

    // /**
    //  * Get the basename for a file URL.
    //  * This is meant to be used in QML.
    //  */
    // Q_INVOKABLE QString baseFileName(const QUrl &url) const;


//public Q_SLOTS:
    // virtual void save();
    // virtual void saveAs();
    // virtual void copyImage();
    // virtual void copyLocation();
    // virtual void copyToClipboard(const QVariant &content);
    // void showPrintDialog();
    // void showPreferencesDialog();
    // void showFontDialog();
    // void showColorDialog(int option);
    // void openContainingFolder(const QUrl &url);

//Q_SIGNALS:
    // void annotatingChanged();
    // void logicalXChanged();
    // void logicalYChanged();

    void syncGeometryWithScreen();

protected:
    ~ColorPickQtWindow();


    // ~ColorPickQtWindow();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    using QQuickView::setTitle;

    //static QList<ColorPickQtWindow *> s_colorPickQtWindowInstances;

	const std::unique_ptr<QQmlContext> m_context;
    //std::unique_ptr<QQmlComponent> m_component;

    QPointer<QScreen> m_screenToFollow;


};




// the following is essentailly the  coresponding .cpp file

//#include "ColorpickQtWindow.h"

// QList<ColorPickQtWindow *> ColorPickQtWindow::s_cndowInstances = {};

ColorPickQtWindow::ColorPickQtWindow()
    : QQuickView()
    //, m_screenToFollow(screen)
{
    SDL_Log("ColorPickQtWindow constructor called");


    // tbd we really want to build a new rect probably and span screens with it??? still 
    // we cannot seem to get reall fullscreen BUT granted we are not on a kde desktop... ??

    // tbd much more things belong in this constructor and not the caller...
    setFlags({
        Qt::Window, // the default window flag
        Qt::FramelessWindowHint,
        Qt::NoDropShadowWindowHint,
        Qt::MaximizeUsingFullscreenGeometryHint // also use the areas where system UIs are
    });

    //cpqtwin->setVisibility(QWindow::FullScreen); // < breaks everything!

    setWindowStates(Qt::WindowFullScreen);

    //cpqtwin->setColor(Qt::transparent);
    //cpqtwin->setColor(Qt::red);
    setColor(QColor(255, 0, 0, 128));





    //auto engine = getQmlEngine();
    const auto screens = qApp->screens();
    for (auto *screen : screens) {
        // const auto screenRect = Geometry::mapFromPlatformRect(screen->geometry(), screen->devicePixelRatio());
        SDL_Log("Screen N %i x %i, dpr:%f %i,%i ", screen->geometry().width(), screen->geometry().height(), screen->devicePixelRatio(), screen->geometry().x(), screen->geometry().y());
        // // Don't show windows for screens that don't have an image.
        // if (!m_videoMode
        //     && !m_annotationDocument->baseImage().isNull()
        //     && !screenRect.intersects(m_annotationDocument->canvasRect())) {
        //     continue;
        // }
        //m_captureWindows.emplace_back(CaptureWindow::makeUnique(mode, screen, engine));
    }

    m_screenToFollow = screens[0];
    syncGeometryWithScreen();

}

// ColorPickQtWindow::UniquePointer ColorPickQtWindow::makeUnique(Mode mode, QScreen *screen, QQmlEngine *engine, QWindow *parent)
// {
//     return UniquePointer(new CaptureWindow(mode, screen, engine, parent), [](CaptureWindow *window){
//         s_cndowInstances.removeOne(window);
//         deleter(window);
//     });
// }

// QList<ColorPickQtWindow *> ColorPickQtWindow::instances()
// {
//     return s_cndowInstances;
// }


ColorPickQtWindow::~ColorPickQtWindow()
{
    SDL_Log("ColorPickQtWindow destructor called, tbd actually cleanup...");
    //s_captureWindowInstances.removeOne(this);
    // if (auto rootItem = rootObject()) {
    //     rootItem->removeEventFilter(SelectionEditor::instance());
    // }
}

void ColorPickQtWindow::mousePressEvent(QMouseEvent *event)
{
    //requestActivate();
    //SpectacleWindow::mousePressEvent(event);
    SDL_Log("mouse press detected!");
    event->accept();
}


void ColorPickQtWindow::mouseMoveEvent(QMouseEvent *event)
{
    // if (shouldIgnoreInput() || event->buttons() & ~acceptedMouseButtons() || event->buttons() == Qt::NoButton) {
    //     QQuickItem::mouseMoveEvent(event);
    //     return;
    // }

    // auto tool = m_document->tool();
    // auto mousePos = event->position();
    // auto wrapper = m_document->selectedItemWrapper();
    // if (tool->type() == AnnotationTool::SelectTool && wrapper->hasSelection() && m_allowDraggingSelection) {
    //     auto documentMousePos = m_localToDocument.map(event->position());
    //     auto dx = documentMousePos.x() - m_lastDocumentPressPos.x();
    //     auto dy = documentMousePos.y() - m_lastDocumentPressPos.y();
    //     wrapper->transform(dx, dy);
    // } else if (tool->isCreationTool()) {
    //     using ContinueOptions = AnnotationDocument::ContinueOptions;
    //     using ContinueOption = AnnotationDocument::ContinueOption;
    //     ContinueOptions options;
    //     if (event->modifiers() & Qt::ShiftModifier) {
    //         options |= ContinueOption::SnapAngle;
    //     }
    //     if (event->modifiers() & Qt::ControlModifier) {
    //         options |= ContinueOption::CenterResize;
    //     }
    //     m_document->continueItem(m_localToDocument.map(mousePos), options);
    // }

    // setHoveredMousePath({});
    SDL_Log("mouse move! %i %i", 1, 1);

    event->accept();
}

void ColorPickQtWindow::mouseReleaseEvent(QMouseEvent *event)
{
    // if (shouldIgnoreInput() || event->button() & ~acceptedMouseButtons()) {
    //     QQuickItem::mouseReleaseEvent(event);
    //     return;
    // }

    // m_document->finishItem();

    // auto toolType = m_document->tool()->type();
    // auto wrapper = m_document->selectedItemWrapper();
    // auto selectedOptions = wrapper->options();
    // if (!selectedOptions.testFlag(AnnotationTool::TextOption) //
    //     && !m_document->isCurrentItemValid()) {
    //     m_document->popCurrentItem();
    // } else if (toolType == AnnotationTool::SelectTool && wrapper->hasSelection()) {
    //     wrapper->commitChanges();
    // } else if (!selectedOptions.testFlag(AnnotationTool::TextOption)) {
    //     m_document->deselectItem();
    // }

    // setPressed(false);
    event->accept();
    SDL_Log("mouse release detected!");

    //qApp->quit();
    //free(this);
    stop_picking_mode();

    hide(); // opposite of show(), we should really clean up better...
    //~ColorPickQtWindow();
}


QScreen* ColorPickQtWindow::screenToFollow() const
{
    return m_screenToFollow;
}

void ColorPickQtWindow::syncGeometryWithScreen()
{
    if (!m_screenToFollow) {
        return;
    }

    QRect screenRect = m_screenToFollow->geometry();

    // Set minimum size to ensure the window always covers the area it is meant to.
    setMinimumSize(screenRect.size());
    setMaximumSize(screenRect.size());
    setGeometry(screenRect);
    setScreen(m_screenToFollow);
}