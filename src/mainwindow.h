#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVulkanInstance>

class EntityListModel;
class RenderView;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace Boiler
{
	class Engine;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
	// Qt members
    Ui::MainWindow *ui;
    QWidget *container;
    RenderView *renderView;
    EntityListModel *entityListModel;
	Boiler::Engine &engine;

public:
    MainWindow(Boiler::Engine &engine, QWidget *parent = nullptr);
    ~MainWindow();

	// overrides
	void closeEvent(QCloseEvent *event) override;
	void showEvent(QShowEvent *event) override;

	RenderView *getRenderView() { return renderView; }

private slots:
	void importModel();
};
#endif // MAINWINDOW_H
