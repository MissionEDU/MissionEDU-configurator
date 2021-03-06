#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QMainWindow>
#include <QPushButton>
#include "json.hpp"
#include "swockets.hpp"
#include <QListWidget>
#include <vector>
#include <QCheckBox>
class ConfiguratorHandler : public SwocketHandler
{
public:
    ConfiguratorHandler(){}
    ~ConfiguratorHandler(){}
    nlohmann::json cml;
    nlohmann::json tsl;

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
        cml = swocket->receive();
        cml = cml["Payload"];
        tsl = swocket->receive();
        tsl = tsl["Payload"];
        std::cout << "connected" << std::endl;
    }

    virtual void handshake_unsuccessful() {
        std::cout << "handshake unsuccessful" << std::endl;
        throw("Handshake unsuccessful");
    }

    nlohmann::json receive() {
        return swocket->receive();
    }
};

class QPushButtonIndexed : public QPushButton
{
public:
    int index;
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
    std::vector<Swockets*> swockets{};
    std::vector<ConfiguratorHandler*> handles{};
    void resetButtons();
    int selectedController{-1};
    void fillData();
    int currentInd = -1;
    Swockets* currentSwocket;
    ConfiguratorHandler* currentHandler;
    void resetMinMax();
    std::vector<QCheckBox*> add_check_boxes{};
    std::vector<QCheckBox*> edit_check_boxes{};
private slots:
    void on_addClientButton_clicked();
    void on_tasksButton_clicked();
    void on_comandsButton_clicked();
    void on_updateButton_clicked();
    void on_disconnectButton_clicked();
    void edit_this();
    void commandSelectionChanged(const QString&);
    void taskSelectionChanged(const QString&);
    void addParamChanged(const QString&);
    void editParamChanged(const QString&);
    void add_commands_to_list(ConfiguratorHandler* handle, QListWidget* list, std::vector<QCheckBox*>* checkboxes);
    void on_editCommandButton_clicked();
    void on_addCommandButton_clicked();
    void on_editTaskButton_clicked();
    void on_deleteTaskButton_clicked();
    void on_deleteCommandButton_clicked();
    void on_addTaskButton_clicked();
};

#endif // CONFIGURATOR_H
