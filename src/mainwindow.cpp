#include <array>
#include <QFileDialog>
#include <iostream>
#include <iterator>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "renderview.h"
#include "entitylistmodel.h"
#include "samplepart.h"

#include "assets/gltfimporter.h"

MainWindow::MainWindow(Boiler::Engine &engine, QWidget *parent) : QMainWindow(parent),
																  ui(new Ui::MainWindow),
																  engine(engine)
{
    ui->setupUi(this);

	// setup menu actions
	connect(ui->actionImportModel, &QAction::triggered, this, &MainWindow::importModel);

	// setup the vulkan instance for Qt
    renderView = new RenderView(engine);

    container = QWidget::createWindowContainer(renderView, ui->renderParent);

	// data models
    //entityListModel = new EntityListModel(engine.getEcs());
}

void MainWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);

	container->setGeometry(2, 2, ui->renderParent->width() - 4, ui->renderParent->height() - 4);

	// initialize the engine for the current render view
    const QRect initialSize = container->geometry();
    const Boiler::Size engSize(initialSize.width(), initialSize.height());
    //engine.initialize(engSize);

    // setup entities list
    //ui->entitiesListView->setModel(entityListModel);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	QMainWindow::closeEvent(event);

	renderView->setRunning(false);
	engine.getRenderer().prepareShutdown();
	engine.getRenderer().shutdown();
}

MainWindow::~MainWindow()
{
    delete entityListModel;
    delete renderView;
    delete container;
    delete ui;
}

void MainWindow::importModel()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Import GLTF scene"), "file",
													tr("GLTF Scene (*.gltf);;All Files (*)"));

	Boiler::GLTFImporter importer;
	importer.import(engine, fileName.toStdString());
}
