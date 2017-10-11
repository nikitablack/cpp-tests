#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <QObject>

class MyObject : public QObject
{
Q_OBJECT
public:
    explicit MyObject(QObject *parent = 0);

signals:

public slots:
	void testSignalReceiver();
};

#endif // MYOBJECT_H
