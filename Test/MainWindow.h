#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

class Test : public QMainWindow
{
	Q_OBJECT

public:
	Test(QWidget *parent = 0);
	~Test();

private:
	Ui::TestClass ui;
};

#endif // MAINWINDOW_H
