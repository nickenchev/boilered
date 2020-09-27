#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVulkanInstance>

#include "core/engine.h"

class EntityListModel;
class RenderView;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace Boiler
{
	class Renderer;
	class Part;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
	// Qt members
    QVulkanInstance instance;
    Ui::MainWindow *ui;
    QWidget *container;
    RenderView *renderView;
    EntityListModel *entityListModel;

	// Boiler related
	std::unique_ptr<Boiler::Renderer> renderer;
	std::shared_ptr<Boiler::Part> part;
    Boiler::Engine engine;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

	// overrides
	void closeEvent(QCloseEvent *event) override;
	void showEvent(QShowEvent *event) override;

    QWidget *getRenderContainer();
    RenderView *getRenderView();

    void start();

private slots:
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_H
