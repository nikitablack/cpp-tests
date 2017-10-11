#include "myobject.h"
#include "mainapplication.h"

MyObject::MyObject(QObject *parent) : QObject(parent)
{
	int i{0};
	MainApplication& app{MainApplication::get(i, nullptr)};
	
	Q_ASSERT(app.thread() != this->thread());
	
	connect(&app, &MainApplication::testSignal, this, &MyObject::testSignalReceiver, Qt::BlockingQueuedConnection);
}

void MyObject::testSignalReceiver()
{
	int a{42};
	(void)a;
}