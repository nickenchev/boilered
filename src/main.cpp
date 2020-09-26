#include "mainwindow.h"
#include "samplepart.h"

#include <QApplication>
#include <QVulkanInstance>
#include <boiler.h>
#include <video/vulkanrenderer.h>

class EditorPart : public Boiler::Part
{
public:
    EditorPart(Boiler::Engine &engine) : Part("Editor", engine)
    {
        engine.getRenderer().setClearColor(glm::vec3(1, 0, 0));
    }

    void onStart() override
    {
    }

    void update(double) override
    {
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::vector<const char *> extensions;
    extensions.push_back("VK_KHR_surface");
    extensions.push_back("VK_KHR_xlib_surface");
    extensions.push_back("VK_KHR_xcb_surface");

    auto renderer = std::make_unique<Boiler::Vulkan::VulkanRenderer>(extensions);
    Boiler::Engine engine(renderer.get());
    //auto part = std::make_shared<EditorPart>(engine);
    auto part = std::make_shared<SamplePart>(engine);
    engine.setPart(part);

    QVulkanInstance instance;
    instance.setVkInstance(renderer->getVulkanInstance());
    if (!instance.create())
    {
        throw std::runtime_error("Error creating QVulkanInstance");
    }

    MainWindow window(engine);
    window.getRenderView()->setGeometry(0, 0, 640, 480);
    window.getRenderView()->setVulkanInstance(&instance);
    window.show();

    // create the surface for the render view window
    VkSurfaceKHR surface = instance.surfaceForWindow(window.getRenderView());
    renderer->setSurface(surface);

    const QRect initialSize = window.getRenderContainer()->geometry();
    const Boiler::Size engSize(initialSize.width(), initialSize.height());
    engine.initialize(engSize);

    window.start();

    return app.exec();
}
