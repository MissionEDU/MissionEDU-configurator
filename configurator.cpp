#include "configurator.h"
#include "ui_configurator.h"
#include <QMessageBox>
using namespace nlohmann;
using namespace std;

Configurator::Configurator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Configurator)
{
    ui->setupUi(this);
}

void Configurator::on_addClientButton_clicked() {
    try{
        ConfiguratorHandler handle{};
        Swockets* swocket = new Swockets(SwocketMode::ISCLIENT, &handle, "127.0.0.1");
        handles.push_back(swocket);

        QListWidgetItem* widgetItem = new QListWidgetItem();
        QWidget* widget = new QWidget();
        QLabel* labelIP;
        QLabel* statusIcon;
        labelIP = new QLabel("127.0.0.1", this);

        QVBoxLayout* labelIPLayout = new QVBoxLayout();
        labelIPLayout->addWidget(labelIP);
        labelIPLayout->setMargin( 0 );
        labelIPLayout->setSpacing( 0 );

        // Create info pane
        statusIcon = new QLabel(this);
        statusIcon->setFixedSize(10, 10);
        statusIcon->setStyleSheet("border:0px solid grey; border-radius: 5px;background-color: #00ff00;");

        QHBoxLayout* generalLayout = new QHBoxLayout(this);
        generalLayout->addWidget(statusIcon);
        generalLayout->addLayout(labelIPLayout, 1);
        generalLayout->setMargin( 5);
        generalLayout->setSpacing( 4 );


        widget->setLayout(generalLayout);
        widget->setStyleSheet("background-color: #575757;");

        widgetItem->setSizeHint(QSize(widgetItem->sizeHint().width(), 30));
        ui->connectedList->addItem(widgetItem);
        ui->connectedList->setItemWidget(widgetItem, widget);
    } catch(...) {
        QMessageBox msgBox;
        msgBox.setText("Controller not available or already connected");
        msgBox.exec();
    }
}

void Configurator::resetButtons() {
    ui->tasksButton->setStyleSheet("");
    ui->comandsButton->setStyleSheet("");
    ui->updateButton->setStyleSheet("");
}
void Configurator::on_tasksButton_clicked() {
    resetButtons();
    ui->tasksButton->setStyleSheet("color:black;font-size:20px;");
}
void Configurator::on_comandsButton_clicked() {
    resetButtons();
    ui->comandsButton->setStyleSheet("color:black;font-size:20px;");
}
void Configurator::on_updateButton_clicked() {
    resetButtons();
    ui->updateButton->setStyleSheet("color:black;font-size:20px;");
}
Configurator::~Configurator()
{
    delete ui;
}
