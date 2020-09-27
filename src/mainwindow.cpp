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

	// initialize thwe playground part
	part = std::make_unique<SamplePart>(engine);
	engine.setPart(part);

    renderView = new RenderView(engine);
    renderView->setVulkanInstance(&instance);

    container = QWidget::createWindowContainer(renderView, ui->renderParent);

	// data models
    entityListModel = new EntityListModel(engine.getEcs());
}

void MainWindow::showEvent(QShowEvent *event)
{
	container->setGeometry(0, 0, ui->renderParent->width(), ui->renderParent->height());

    // create the surface for the render view window
	auto *vkRenderer = static_cast<Boiler::Vulkan::VulkanRenderer *>(renderer.get());
    VkSurfaceKHR surface = instance.surfaceForWindow(renderView);
    vkRenderer->setSurface(surface);

	// initialize the engine for the current render view
    const QRect initialSize = container->geometry();
    const Boiler::Size engSize(initialSize.width(), initialSize.height());
    engine.initialize(engSize);

	start();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
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
