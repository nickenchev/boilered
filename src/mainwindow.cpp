#include <array>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "renderview.h"
#include "entitylistmodel.h"
#include "video/vulkanrenderer.h"
#include "samplepart.h"

std::vector<const char *> extensions =
{
	"VK_KHR_surface",
	"VK_KHR_xlib_surface",
	"VK_KHR_xcb_surface"
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
	  renderer(std::make_unique<Boiler::Vulkan::VulkanRenderer>(extensions)),
	  engine(renderer.get()),
	  ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	auto *vkRenderer = static_cast<Boiler::Vulkan::VulkanRenderer *>(renderer.get());

	// setup the vulkan instance for Qt
    instance.setVkInstance(vkRenderer->getVulkanInstance());
    if (!instance.create())
    {
        throw std::runtime_error("Error creating QVulkanInstance");
    }

    renderView = new RenderView(engine);
    renderView->setVulkanInstance(&instance);

    container = QWidget::createWindowContainer(renderView, ui->renderParent);

	// data models
    entityListModel = new EntityListModel(engine.getEcs());
}

void MainWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);

	container->setGeometry(2, 2, ui->renderParent->width() - 4, ui->renderParent->height() - 4);

    // create the surface for the render view window
	auto *vkRenderer = static_cast<Boiler::Vulkan::VulkanRenderer *>(renderer.get());
    VkSurfaceKHR surface = instance.surfaceForWindow(renderView);
    vkRenderer->setSurface(surface);

	// initialize the engine for the current render view
    const QRect initialSize = container->geometry();
    const Boiler::Size engSize(initialSize.width(), initialSize.height());
    engine.initialize(engSize);

    // start part, load content
	part = std::make_shared<SamplePart>(engine);
	engine.setPart(part);
    engine.getPart()->onStart();

    // request updates and start running engine
    renderView->requestUpdate();
    renderView->setRunning(true);

    // setup entities list
    ui->entitiesListView->setModel(entityListModel);
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

void MainWindow::on_pushButton_clicked()
{
    engine.getEcs().newEntity();
}
