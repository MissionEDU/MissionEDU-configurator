#include "configurator.h"
#include <QApplication>
using namespace std;
using namespace nlohmann;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Configurator w;
    w.show();
    return a.exec();
}
