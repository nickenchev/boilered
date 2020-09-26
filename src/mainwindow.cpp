#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "entitylistmodel.h"

MainWindow::MainWindow(Boiler::Engine &engine, QWidget *parent)
    : QMainWindow(parent), engine(engine), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    renderView = new RenderView(engine);
    container = QWidget::createWindowContainer(renderView, ui->renderParent);

    const int width = ui->renderParent->geometry().width();
    const int height = ui->renderParent->geometry().height();
    container->setGeometry(0, 0, width, height);

    entityListModel = new EntityListModel(engine.getEcs());
}

MainWindow::~MainWindow()
{
    delete entityListModel;
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

void MainWindow::start()
{
    // start of part, load content
    engine.getPart()->onStart();

    // setup entities list
    ui->entitiesListView->setModel(entityListModel);

    // request updates and start running engine
    getRenderView()->requestUpdate();
    getRenderView()->setRunning(true);
}

void MainWindow::on_pushButton_clicked()
{
    engine.getEcs().newEntity();
}
