#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <boiler.h>
#include "renderview.h"

class EntityListModel;

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

    EntityListModel *entityListModel;

public:
    MainWindow(Boiler::Engine &engine, QWidget *parent = nullptr);
    ~MainWindow();

	// overrides
	void closeEvent(QCloseEvent *event) override;

    QWidget *getRenderContainer();
    RenderView *getRenderView();

    void start();

private slots:
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_H
