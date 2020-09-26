#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(Boiler::Engine &engine, QWidget *parent)
    : QMainWindow(parent), engine(engine), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    renderView = new RenderView(engine);
    container = QWidget::createWindowContainer(renderView, ui->renderParent);
    container->setGeometry(0, 0, 640, 480);
}

MainWindow::~MainWindow()
{
    delete renderView;
    delete container;
    delete ui;
}

QWidget *MainWindow::getRenderContainer()
{
    return container;
}

RenderView *MainWindow::getRenderView()
{
    return renderView;
}
