#include "configurator.h"
#include <QApplication>
using namespace std;
using namespace nlohmann;

static json tsl;
static json cml;

class ConfiguratorHandler : public SwocketHandler
{
public:
    ConfiguratorHandler(){}
    ~ConfiguratorHandler(){}

    static json pack_message(string m_type){
        json message;
        message["MessageType"] = m_type;
        message["Payload"] = json({});
        return message;
    }

    virtual bool handshake(int sock) {
        json req = pack_message("REQ");
        req["Payload"]["ClientType"] = "config";
        json ack = pack_message("ACK");
        json arf = pack_message("ARF");

        swocket->send(req);
        json resp = swocket->receive();
        string resp_type = resp["MessageType"];
        if(resp_type == "ACK") {
            swocket->send(ack);
            return true;
        } else {
            swocket->send(arf);
            return false;
        }
        return false;
    }

    virtual void recv(nlohmann::json recvObj) {
        cout << endl << endl << "RECEIVED:" << endl;
        std::cout << recvObj << std::endl;
    }

    virtual void disconnect() {
        std::cout << "disconnected" << std::endl;
    }

    virtual void connect(int sock) {
        cml = swocket->receive();
        tsl = swocket->receive();
    }

    virtual void handshake_unsuccessful() {
        std::cout << "handshake unsuccessful" << std::endl;
    }
};


void Configurator::on_addClientButton_clicked() {
    cout << "clicked" << endl;

    //ConfiguratorHandler handle{};
    //Swockets swocket{SwocketMode::ISCLIENT, &handle, "127.0.0.1"};
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Configurator w;
    w.show();

    return a.exec();
}
