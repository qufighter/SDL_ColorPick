#pragma once


// #include <QFlags>
// #include <QImage>
// #include <QObject>
// #include <KGlobalAccel>
// #include <KLocalizedString>
#include <QApplication>
// #include <qnamespace.h>
// #include <QScreen>
// #include <QEventLoopLocker>
// #include <QObject>
// //#include <QQmlEngine>
// //#include <QQuickItem>

// #include <QQuickView>
// #include <QQmlContext>

// #include <QVariantAnimation>
// #include <QCommandLineParser>
// //#include <QDBusConnection>
// #include <QDir>
// #include <QSessionManager>

// //#include <KWindowSystem>

// #include <QScreen>

#include <QQuickView>
#include <QQmlContext>

//#include "../LinuxFileChooser.h" // not really needed(sort of circiular...)

/*
TBD: todo:

clen includeles
fix dpr/scale support??  (see note virtualGeometry ?)
strikethrough profit
yes there is still some sample code pulled out of spectacle for reference

*/


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
    void setShapeAndBegin();
    // using UniquePointer = std::unique_ptr<ColorPickQtWindow, void (*)(ColorPickQtWindow *)>;

    // static UniquePointer makeUnique();

    // static QList<ColorPickQtWindow *> instances();


    //QScreen* screenToFollow() const;

    //Q_SIGNALS:
    //void screenToFollowChanged();

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
    void debugQRect(const char* label, const QRect &rect);

protected:
    ~ColorPickQtWindow();


    // ~ColorPickQtWindow();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    //using QQuickView::setTitle;

    //static QList<ColorPickQtWindow *> s_colorPickQtWindowInstances;

	const std::unique_ptr<QQmlContext> m_context;
    //std::unique_ptr<QQmlComponent> m_component;

    //QPointer<QScreen> m_screenToFollow;

    QRect* computedScreenSize;

    // QKeySequence m_pressedKeys;
};




// the following is essentailly the  coresponding .cpp file

//#include "ColorpickQtWindow.h"

// QList<ColorPickQtWindow *> ColorPickQtWindow::s_cndowInstances = {};

ColorPickQtWindow::ColorPickQtWindow()
    : QQuickView() // I'm an honest guy and I will admint I have no clue what this syntax does and thats because of just how incomprehensible it is...
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

    //setVisibility(QWindow::FullScreen); // < breaks everything!

    setWindowStates(Qt::WindowFullScreen);  // does nothing? we now use showFullScreen *tbd refactor into a helper here*

    //cpqtwin->
    //cpqtwin->setColor(Qt::red);
#ifdef COLORPICK_DEBUG_MODE
    setColor(QColor(255, 0, 0, 10));
    //setColor(Qt::transparent);
#else
    setColor(Qt::transparent);
#endif

    setCursor(Qt::CrossCursor);

    computedScreenSize = new QRect(0,0,0,0);
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

    FREE_FOR_NEW(computedScreenSize); // tbd there are alternatives to free, eg unique_ptr ?
}

// lol though we will probably need these, doesn't it seem like someone fixed the bug in the wrong place? i.e Qt rects not working right for osme reason on wayland?  or maybe a differnet rect taht is unified is needed?  This is a big lol for implmenetation... not criticizing like solving problems ALWAYS BEATS doing it right.... but doesn't this just make problems also??? 
// anyway see SIze and virtual size, befor eimplemetning anythign like this...QRect     virtualGeometry() const  https://doc.qt.io/qt-6/qscreen.html
// QRectF Geometry::mapFromPlatformRect(const QRectF &rect, qreal dpr)
// {
//     if (rect.isEmpty() || KWindowSystem::isPlatformWayland()) {
//         return rect;
//     }

//     // Make the position scaled like Wayland.
//     return {rect.topLeft() / dpr, rect.size()};
// }
// anway TBD we have problems to solve here


void ColorPickQtWindow::setShapeAndBegin(){

    computedScreenSize->setLeft(0);
    computedScreenSize->setRight(0);
    computedScreenSize->setTop(0);
    computedScreenSize->setBottom(0);

    //auto engine = getQmlEngine();
    const auto screens = qApp->screens(); // strangely something about these is NOT RIGHT eg assigned at launch???  change dpr at runtime...
    for (QScreen* screen : screens) { // QPointer<QScreen>
        // const auto screenRect = Geometry::mapFromPlatformRect(screen->geometry(), screen->devicePixelRatio());
        SDL_Log("Screen N %i x %i, dpr:%f %i,%i ", screen->geometry().width(), screen->geometry().height(), screen->devicePixelRatio(), screen->geometry().x(), screen->geometry().y());

        debugQRect("Screen geometry: ", screen->geometry());


        if( screen->geometry().x() < computedScreenSize->x() ){
            computedScreenSize->setLeft(screen->geometry().x());
        }
        if( screen->geometry().y() < computedScreenSize->y() ){
            computedScreenSize->setTop(screen->geometry().y());
        }
        if( screen->geometry().right() > computedScreenSize->right() ){
            computedScreenSize->setRight(screen->geometry().right());
        }
        if( screen->geometry().bottom() > computedScreenSize->bottom() ){
            computedScreenSize->setBottom(screen->geometry().bottom());
        }
    }
    debugQRect("Click shield computed size:", *computedScreenSize);

    //m_screenToFollow = screens[0];
    syncGeometryWithScreen();

    requestActivate();

    show();
    showFullScreen();  // this is  having wierd side effects on my gnome desktop  hiding the launcher and status bar, but USUALLY they are in the screenshot anyway...
    // otherwise seems to do wrong thing with regular show() - just not topmost - maybe another way

}

void ColorPickQtWindow::syncGeometryWithScreen()
{
    QRect screenRect = *computedScreenSize;
    setMinimumSize(screenRect.size());
    setMaximumSize(screenRect.size());
    setGeometry(screenRect);
    //setScreen(m_screenToFollow); // we covery both screens just fine, so forget about this for now... maybe needs to be the top left screen???  seemingly not important...
}

void ColorPickQtWindow::mousePressEvent(QMouseEvent *event)
{
    //requestActivate();
    //SpectacleWindow::mousePressEvent(event);
    //SDL_Log("Qt mouse press detected!");
    event->accept();
}


void ColorPickQtWindow::mouseMoveEvent(QMouseEvent *event)
{

    //QPointF mouseScreenPos = event->globalPosition();
    // QPointF mouseScreenPos = event->position();
    //https://doc.qt.io/qt-6/qmouseevent.html
    //https://doc.qt.io/qt-6/qsinglepointevent.html#globalPosition
    // QPoint QCursor::pos() // https://doc.qt.io/qt-6/qcursor.html#pos
    QPoint mouseScreenPos = QCursor::pos();
    // many ways, same thing..
    // //  incomprehensible when using fractional scaling (eg, doubled or not, depending on screen)
    // anyway worrth testing device scale on a real kde/Qt desktop, where I suspect we'll get more sensible values somehow...


    SDL_Log("Qt mouse move! %i %i", (int)mouseScreenPos.x(), (int)mouseScreenPos.y());

    SDL_Event uevent;
    SDL_UserEvent userevent;
    SDL_Point* mmevent = new SDL_Point(); // note: we deallocate this on main thread...

    // hmm this translate code is now everywhere...?  well the -1 might be needed everywhere tough to say, but seems needed for this desktop/screen/size/at least
    mmevent->x = (openglContext->fullPickImgSurface->clip_rect.w - (int)mouseScreenPos.x()) - (openglContext->fullPickImgSurface->clip_rect.w / 2) - 1;
    mmevent->y = (openglContext->fullPickImgSurface->clip_rect.h - (int)mouseScreenPos.y()) - (openglContext->fullPickImgSurface->clip_rect.h / 2) - 1;

    userevent.type = SDL_USEREVENT;
    userevent.code = USER_EVENT_ENUM::PICK_AT_POSITION;
    userevent.data1 = mmevent;
    userevent.data2 = NULL;

    uevent.type = SDL_USEREVENT;
    uevent.user = userevent;

    //SDL_Log("mm event pos type %i", USER_EVENT_ENUM::PICK_AT_POSITION);
    SDL_PushEvent(&uevent);

    event->accept();
}

void ColorPickQtWindow::mouseReleaseEvent(QMouseEvent *event)
{
    SDL_Log("Qt mouse release detected!");
    //qApp->quit();
    //free(this);
    stop_picking_mode(); // also hides pick mode
    //OpenGLContext* openglContext = OpenGLContext::Singleton();
    openglContext->generalUx->addCurrentToPickHistory();
    event->accept();
}


void ColorPickQtWindow::keyPressEvent(QKeyEvent *event)
{
    // Events need to be processed normally first for events to reach items
    QQuickView::keyPressEvent(event);
    if (event->isAccepted()) {
        return;
    }
    //m_pressedKeys = event->key() | event->modifiers();
}


// TBD on GTK we do these at press, proably also for other platforms...
// for ESC who cares... but maybe for refresh we SHOULD use press events (but possibly a problem for sequence detect...)
// either way seems unimportant for now, as hover state is not likely going to be impacted differently by keyboard
// and a little lag for these functions doesn't matter
void ColorPickQtWindow::keyReleaseEvent(QKeyEvent *event)
{
    // Events need to be processed normally first for events to reach items
    // QQuickView::keyReleaseEvent(event);
    // if (event->isAccepted()) {
    //     return;
    // }
    // I'm beleivng this, above, though strictly for this project, is probably not needed... but great


    if (event->matches(QKeySequence::Quit)
        || event->matches(QKeySequence::Close)
        || event->matches(QKeySequence::Cancel) // todo 83592, QKeySequence made our Qt version superior to other platforms for now... fixme for other platforms!
        || event->key() == Qt::Key_Escape
    ){
        event->accept();
        stop_picking_mode();
    } else if (event->matches(QKeySequence::New) // todo 83592, QKeySequence made our Qt version superior to other platforms for now... fixme for other platforms!
        || event->key() == Qt::Key_R
        || event->key() == Qt::Key_J
    ){
        event->accept();
        stop_picking_mode();
        SDL_AddTimer(250, pick_again_soon, nullptr);
    }
    //m_pressedKeys = {};
}


// QScreen* ColorPickQtWindow::screenToFollow() const
// {
//     return m_screenToFollow;
// }

void ColorPickQtWindow::debugQRect(const char* label, const QRect &rect){
#ifdef COLORPICK_DEBUG_MODE
    SDL_Log("%s rectangle: %i,%i %ix%i", label, rect.x(), rect.y(), rect.width(), rect.height());
#endif
}
