#pragma once

#include <QQuickView>
#include <QQmlContext>


// TBD, make cpp file and implement the constructor(s)? 
// then we can certainuly...
// CaptureWindow::CaptureWindow(Mode mode, QScreen *screen, QQmlEngine *engine, QWindow *parent)
// SpectacleWindow::SpectacleWindow(QQmlEngine *engine, QWindow *parent)
/*

CaptureWindow::CaptureWindow(Mode mode, QScreen *screen, QQmlEngine *engine, QWindow *parent)
    : SpectacleWindow(engine, parent)
    , m_screenToFollow(screen)
{

	so, is this calling the parent constructor  SpectacleWindow(engine, parent)
	and what is this syntax after that?

*/

class ColorPickQtWindow : public QQuickView
{
    //Q_OBJECT
    // Q_PROPERTY(bool annotating READ isAnnotating WRITE setAnnotating NOTIFY annotatingChanged FINAL)
    // Q_PROPERTY(qreal logicalX READ logicalX NOTIFY logicalXChanged)
    // Q_PROPERTY(qreal logicalY READ logicalY NOTIFY logicalYChanged)

public:

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

protected:
    // explicit ColorPickQtWindow(QQmlEngine *engine = nullptr, QWindow *parent = nullptr);
    // ~ColorPickQtWindow();

    using QQuickView::setTitle;

    //static QList<ColorPickQtWindow *> s_colorPickQtWindowInstances;

	const std::unique_ptr<QQmlContext> m_context;
    //std::unique_ptr<QQmlComponent> m_component;


};
