#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H

#include <QApplication>
#include <QQuickWindow>

class MyObject;

class MainApplication : public QApplication
{
Q_OBJECT
public:
	static MainApplication& get(int& argc, char** argv);
	
signals:
	void testSignal();

private:
	MainApplication(int& argc, char** argv);
	void onSceneGraphInitialized();
	void timerEvent(QTimerEvent* e) override;
	
private:
	QQuickWindow w;
	int timerId;
	MyObject* myObj{nullptr};
};

#endif // MAINAPPLICATION_H
