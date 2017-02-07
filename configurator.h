#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QMainWindow>
#include "json.hpp"
#include "swockets.hpp"
#include <vector>
class ConfiguratorHandler : public SwocketHandler
{
public:
    ConfiguratorHandler(){}
    ~ConfiguratorHandler(){}

    static nlohmann::json pack_message(std::string m_type){
        nlohmann::json message;
        message["MessageType"] = m_type;
        message["Payload"] = nlohmann::json({});
        return message;
    }

    virtual bool handshake(int sock) {
        std::cout << "handshake initiated" << std::endl;
        nlohmann::json req = pack_message("REQ");
        req["Payload"]["ClientType"] = "config";
        nlohmann::json ack = pack_message("ACK");
        nlohmann::json arf = pack_message("ARF");

        swocket->send(req);
        nlohmann::json resp = swocket->receive();
        std::string resp_type = resp["MessageType"];
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
        std::cout << std::endl << std::endl << "RECEIVED:" << std::endl;
        std::cout << recvObj << std::endl;
    }

    virtual void disconnect() {
        std::cout << "disconnected" << std::endl;
    }

    virtual void connect(int sock) {
        std::cout << "connect initiated" << std::endl;
        nlohmann::json cml = swocket->receive();
        nlohmann::json tsl = swocket->receive();
        std::cout << "connected" << std::endl;
    }

    virtual void handshake_unsuccessful() {
        std::cout << "handshake unsuccessful" << std::endl;
        throw("Handshake unsuccessful");
    }
};
namespace Ui {
class Configurator;
}

class Configurator : public QMainWindow
{
    Q_OBJECT

public:
    explicit Configurator(QWidget *parent = 0);
    ~Configurator();

private:
    Ui::Configurator *ui;
    std::vector<Swockets*> handles{};
    void resetButtons();
private slots:
    void on_addClientButton_clicked();
    void on_tasksButton_clicked();
    void on_comandsButton_clicked();
    void on_updateButton_clicked();
};

#endif // CONFIGURATOR_H
