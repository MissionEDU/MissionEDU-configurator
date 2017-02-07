#include "configurator.h"
#include <QApplication>
using namespace std;
using namespace nlohmann;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Configurator w;
    w.show();/*

    ConfiguratorHandler handle{};
    Swockets swocket{SwocketMode::ISCLIENT, &handle, "127.0.0.1"};
    cout << "swocket created" << endl;*/
    return a.exec();
}
