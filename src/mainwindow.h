#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <boiler.h>
#include "renderview.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Boiler::Engine &engine;
    Ui::MainWindow *ui;
    QWidget *container;
    RenderView *renderView;

public:
    MainWindow(Boiler::Engine &engine, QWidget *parent = nullptr);
    ~MainWindow();

    QWidget *getRenderContainer();
    RenderView *getRenderView();

    void start()
    {
        engine.getPart()->onStart();
        getRenderView()->requestUpdate();
        getRenderView()->setRunning(true);
    }
};
#endif // MAINWINDOW_H
