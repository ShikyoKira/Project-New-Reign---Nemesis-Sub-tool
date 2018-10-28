#include "ProjectNewReignQtGUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ProjectNewReignQtGUI w;
	w.show();
	return a.exec();
}
