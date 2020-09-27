#ifndef RENDERVIEW_H
#define RENDERVIEW_H

#include <QWindow>
#include <boiler.h>

class RenderView : public QWindow
{
    Boiler::Engine &engine;
    bool running;

    void resizeEvent(QResizeEvent *event) override
    {
        QWindow::resizeEvent(event);
    }

    void exposeEvent(QExposeEvent *event) override
    {
    }

    bool event(QEvent *event) override
    {
        QWindow::event(event);

        if (running)
        {
            engine.step();
            requestUpdate();
        }
        return true;
    }

public:
    RenderView(Boiler::Engine &engine) : engine(engine)
    {
        running = false;
        setSurfaceType(QSurface::VulkanSurface);
    }

	~RenderView()
	{
	}

    void setRunning(bool running) { this->running = running; }
};


#endif // RENDERVIEW_H
