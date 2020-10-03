#include <QApplication>
#include <QVulkanInstance>
#include "mainwindow.h"
#include "renderview.h"

#include "core/engine.h"
#include "video/vulkanrenderer.h"

std::vector<const char *> extensions =
{
	"VK_KHR_surface",
	"VK_KHR_xlib_surface",
	"VK_KHR_xcb_surface"
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	auto renderer = std::make_unique<Boiler::Vulkan::VulkanRenderer>(extensions);
	Boiler::Engine engine(renderer.get());

    MainWindow window(engine);
    window.show();

	// setup the vulkan instance for Qt
	QVulkanInstance instance;
    instance.setVkInstance(renderer->getVulkanInstance());
    if (!instance.create())
    {
        throw std::runtime_error("Error creating QVulkanInstance");
    }
	window.getRenderView()->setVulkanInstance(&instance);

    VkSurfaceKHR surface = instance.surfaceForWindow(window.getRenderView());
    renderer->setSurface(surface);

	engine.initialize(Boiler::Size(100, 100));

    // start part, load content
	engine.setPart(std::make_shared<SamplePart>(engine));
    engine.getPart()->onStart();

	while (1)
	{
		engine.step();
		app.processEvents();
	}

	app.exit();
	return 0;
}
