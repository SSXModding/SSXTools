#include "MainWindow.h"
#include <QtWidgets/QApplication>

using namespace ssxtools::ui;

int main(int argc, char** argv) {
	QCoreApplication::addLibraryPath("./");
	QApplication a(argc, argv);
	MainWindow w;

	w.show();
	return a.exec();
}
