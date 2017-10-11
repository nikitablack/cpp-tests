#include "mainapplication.h"
#include "myobject.h"

MainApplication::MainApplication(int& argc, char** argv) : QApplication(argc, argv)
{
	w.show();
	
	connect(&w,
	        &QQuickWindow::sceneGraphInitialized,
	        this,
	        &MainApplication::onSceneGraphInitialized,
	        Qt::DirectConnection);
	
	timerId = startTimer(5000);
}

void MainApplication::onSceneGraphInitialized()
{
	myObj = new MyObject;
}

MainApplication& MainApplication::get(int& argc, char** argv)
{
	static MainApplication app{argc, argv};
	return app;
}

void MainApplication::timerEvent(QTimerEvent* e)
{
	killTimer(timerId);
	
	emit testSignal();
}