#ifndef RENDERVIEW_H
#define RENDERVIEW_H

#include <QWindow>
#include <QKeyEvent>

#include <boiler.h>
#include "samplepart.h"

class RenderView : public QWindow
{
    Boiler::Engine &engine;
    bool running;

    void resizeEvent(QResizeEvent *event) override
    {
		std::cout << "Resize" << std::endl;
        QWindow::resizeEvent(event);
		engine.getRenderer().resize(Boiler::Size(event->size().width(), event->size().height()));
    }

    void exposeEvent(QExposeEvent *event) override
    {
    }

	void setKey(int key, bool state)
	{
		auto part = static_cast<SamplePart *>(engine.getPart().get());
		switch (key)
		{
			case Qt::Key::Key_A:
			{
				part->moveLeft = state;
				break;
			}
			case Qt::Key::Key_D:
			{
				part->moveRight = state;
				break;
			}
			case Qt::Key::Key_W:
			{
				part->moveFurther = state;
				break;
			}
			case Qt::Key::Key_S:
			{
				part->moveCloser = state;
				break;
			}
			case Qt::Key::Key_PageUp:
			{
				part->moveUp = state;
				break;
			}
			case Qt::Key::Key_PageDown:
			{
				part->moveDown = state;
				break;
			}
			case Qt::Key::Key_Left:
			{
				part->turnLeft = state;
				break;
			}
			case Qt::Key::Key_Right:
			{
				part->turnRight = state;
				break;
			}
		}
	}

	void keyPressEvent(QKeyEvent *event) override
	{
		setKey(event->key(), true);
	}

	void keyReleaseEvent(QKeyEvent *event) override
	{
		setKey(event->key(), false);
	}

    bool event(QEvent *event) override
    {
        QWindow::event(event);
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
