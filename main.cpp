#include "controller.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    app::Controller controller;
    return a.exec();
}
