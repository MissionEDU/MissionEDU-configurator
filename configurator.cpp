#include "configurator.h"
#include "ui_configurator.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <limits.h>
#include <float.h>
using namespace nlohmann;
using namespace std;

Configurator::Configurator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Configurator)
{
    ui->setupUi(this);
    ui->commandWidget->hide();
    ui->tasksWidget->hide();
    connect(ui->editChoose,SIGNAL(currentIndexChanged(const QString&)),
            this,SLOT(commandSelectionChanged(const QString&)));
    connect(ui->editChooseCombobox,SIGNAL(currentIndexChanged(const QString&)),
            this,SLOT(taskSelectionChanged(const QString&)));
    connect(ui->editParameterChoose,SIGNAL(currentIndexChanged(const QString&)),
            this,SLOT(editParamChanged(const QString&)));
    connect(ui->addParameterChoose,SIGNAL(currentIndexChanged(const QString&)),
            this,SLOT(addParamChanged(const QString&)));

    ui->editCommandCode->setTabStopWidth(20);
    ui->addCommandCode->setTabStopWidth(20);
    resetMinMax();
    ui->addMinMaxInt->show();

    ui->addMaxInt->setMinimum(INT_MIN);
    ui->addMaxInt->setMaximum(INT_MAX);
    ui->addMinInt->setMinimum(INT_MIN);
    ui->addMinInt->setMaximum(INT_MAX);
    ui->editMaxInt->setMinimum(INT_MIN);
    ui->editMaxInt->setMaximum(INT_MAX);
    ui->editMinInt->setMinimum(INT_MIN);
    ui->editMinInt->setMaximum(INT_MAX);

    ui->addMaxDouble->setMinimum(DBL_MIN);
    ui->addMaxDouble->setMaximum(DBL_MAX);
    ui->addMinDouble->setMinimum(DBL_MIN);
    ui->addMinDouble->setMaximum(DBL_MAX);
    ui->editMaxDouble->setMinimum(DBL_MIN);
    ui->editMaxDouble->setMaximum(DBL_MAX);
    ui->editMinDouble->setMinimum(DBL_MIN);
    ui->editMinDouble->setMaximum(DBL_MAX);
}

void Configurator::on_addClientButton_clicked() {
    bool ok;
    QString address_qs = QInputDialog::getText(this, tr("Connect to Controller"),
                                         tr("IP Address"), QLineEdit::Normal,
                                         "127.0.0.1", &ok);
    string address = address_qs.toStdString();
    if (ok && !address_qs.isEmpty())
        try{
            ConfiguratorHandler* handle = new ConfiguratorHandler();
            Swockets* swocket = new Swockets(SwocketMode::ISCLIENT, handle, address);
            swockets.push_back(swocket);

            QListWidgetItem* widgetItem = new QListWidgetItem();
            QWidget* widget = new QWidget();
            QPushButtonIndexed* labelIP;
            QLabel* statusIcon;
            QPushButtonIndexed* disconnect;

            // Create Disconnect Button and surrounding pane
            disconnect = new QPushButtonIndexed();
            disconnect->setText("x");
            disconnect->setStyleSheet("border:0px solid grey; border-radius: 0px; color: red;");
            disconnect->setMinimumWidth(10);
            disconnect->setMaximumWidth(10);
            disconnect->index = swockets.size()-1;
            QVBoxLayout* disconnectButtonLayout = new QVBoxLayout();
            disconnectButtonLayout->addWidget(disconnect);
            disconnectButtonLayout->setMargin(0);
            disconnectButtonLayout->setSpacing(0);

            QObject::connect(disconnect, SIGNAL (clicked()), this, SLOT (on_disconnectButton_clicked()));

            // Create IP label and surrounding pane
            labelIP = new QPushButtonIndexed();
            labelIP->setText(address.c_str());
            labelIP->setStyleSheet("border:0px solid grey; border-radius: 0px;text-align:left;");
            labelIP->index = swockets.size()-1;
            QVBoxLayout* labelIPLayout = new QVBoxLayout();
            labelIPLayout->addWidget(labelIP);
            labelIPLayout->setMargin( 0 );
            labelIPLayout->setSpacing( 0 );

            QObject::connect(labelIP, SIGNAL (clicked()), this, SLOT (edit_this()));

            // Create Status Icon and surrounding pane
            statusIcon = new QLabel(this);
            statusIcon->setFixedSize(10, 10);
            statusIcon->setStyleSheet("border:0px solid grey; border-radius: 5px;background-color: #00ff00;");

            QHBoxLayout* generalLayout = new QHBoxLayout(this);
            generalLayout->addWidget(statusIcon);
            generalLayout->addLayout(labelIPLayout, 1);
            generalLayout->addLayout(disconnectButtonLayout,1);
            generalLayout->setMargin( 5);
            generalLayout->setSpacing( 4 );


            widget->setLayout(generalLayout);
            widget->setStyleSheet("background-color: #575757;");

            widgetItem->setSizeHint(QSize(widgetItem->sizeHint().width(), 30));
            ui->connectedList->addItem(widgetItem);
            ui->connectedList->setItemWidget(widgetItem, widget);

            selectedController++;
            fillData();
        } catch(...) {
            QMessageBox msgBox;
            msgBox.setText("Controller not available or already connected");
            msgBox.exec();
        }
}

void Configurator::fillData() {
    Swockets* selected = swockets[selectedController];
    ConfiguratorHandler* handle = (ConfiguratorHandler*)(selected->handle_);
    cout << handle->tsl << endl;
    cout << handle->cml << endl;
}

void Configurator::resetButtons() {
    ui->tasksButton->setStyleSheet("");
    ui->comandsButton->setStyleSheet("");
    //ui->updateButton->setStyleSheet("");
    ui->commandWidget->hide();
    ui->tasksWidget->hide();
}
void Configurator::on_tasksButton_clicked() {
    resetButtons();
    ui->tasksButton->setStyleSheet("color:black;font-size:20px;");
    ui->tasksWidget->show();
}
void Configurator::on_comandsButton_clicked() {
    resetButtons();
    ui->comandsButton->setStyleSheet("color:black;font-size:20px;");
    ui->commandWidget->show();
}
void Configurator::on_updateButton_clicked() {
    //resetButtons();
    //ui->updateButton->setStyleSheet("color:black;font-size:20px;");
    QMessageBox msgBox;
    msgBox.setText("Updating not yet available, check missionedu.org/updates for updates");
    msgBox.exec();
}
void Configurator::on_disconnectButton_clicked() {
    QObject *senderObj = sender();
    QPushButton *senderButton = qobject_cast<QPushButton*>(senderObj);
    QPushButtonIndexed *sender = dynamic_cast<QPushButtonIndexed*>(senderButton);

    cout << "disconnect clicked" << sender->index << endl;
    Swockets* swocket = swockets[sender->index];
    swocket->stop();
    ui->connectedList->setRowHidden(sender->index,true);
}

void Configurator::add_commands_to_list(ConfiguratorHandler* handle, QListWidget* list, vector<QCheckBox*>* checkboxes) {
    list->clear();
    checkboxes->clear();
    for (json::iterator it = handle->cml.begin(); it != handle->cml.end(); ++it) {
        json command = *it;
        string command_name = command["CommandName"];
        QListWidgetItem* widgetItem = new QListWidgetItem();
        QWidget* widget = new QWidget();
        QCheckBox* checkBox;

        checkBox = new QCheckBox();
        checkboxes->push_back(checkBox);
        checkBox->setText(QString::fromStdString(command_name));
        QVBoxLayout* checkBoxLayout = new QVBoxLayout();
        checkBoxLayout->addWidget(checkBox);
        checkBoxLayout->setMargin(0);
        checkBoxLayout->setSpacing(0);

        widget->setLayout(checkBoxLayout);

        widgetItem->setSizeHint(QSize(widgetItem->sizeHint().width(), 30));
        list->addItem(widgetItem);
        list->setItemWidget(widgetItem, widget);
    }
}

void Configurator::edit_this() {
    QObject *senderObj = sender();
    QPushButton *senderButton = qobject_cast<QPushButton*>(senderObj);
    QPushButtonIndexed *sender = dynamic_cast<QPushButtonIndexed*>(senderButton);
    currentInd = sender->index;
    Swockets* swocket = swockets[sender->index];
    currentSwocket = swocket;
    ConfiguratorHandler* handle = (ConfiguratorHandler*)swocket->handle_;
    currentHandler = handle;

    cout << "edit: " << sender->index << endl;

    json tsl = handle->tsl;
    json cml = handle->cml;

    QStringList tasks{};
    QStringList commands{};

    for (json::iterator it = tsl.begin(); it != tsl.end(); ++it) {
        json task = *it;
        string task_name = task["TaskName"];
        tasks<<QString::fromStdString(task_name);
        cout << "addded task" << endl;
    }

    for (json::iterator it = cml.begin(); it != cml.end(); ++it) {
        json command = *it;
        string command_name = command["CommandName"];
        commands<<QString::fromStdString(command_name);
    }

    add_commands_to_list(handle, ui->addSelectFunctions, &add_check_boxes);
    add_commands_to_list(handle, ui->editSelectFunctions, &edit_check_boxes);
    ui->editChooseCombobox->clear();
    ui->editChooseCombobox->addItems(tasks);
    ui->editChoose->clear();
    ui->editChoose->addItems(commands);
}

void Configurator::commandSelectionChanged(const QString&) {
    cout << "command selection changed" << endl;

    if(ui->editChoose->currentIndex() >= 0) {
        json currentCommand = currentHandler->cml[ui->editChoose->currentIndex()];
        string command_code = currentCommand["CommandCode"];
        string command_return_data_type = currentCommand["CommandReturn"];
        string command_name = currentCommand["CommandName"];
        string command_param_data_type = currentCommand["CommandParams"][0]["CommandParamDataType"];

        if(command_param_data_type != "void"){
            string command_param_name = currentCommand["CommandParams"][0]["CommandParamName"];
            ui->editParameterName->setText(QString::fromStdString(command_param_name));
            if(command_param_data_type == "double"){
                ui->editMinDouble->setValue(currentCommand["CommandParams"][0]["CommandParamRanges"]["min"]);
                ui->editMaxDouble->setValue(currentCommand["CommandParams"][0]["CommandParamRanges"]["max"]);
                ui->editMinMaxInt->hide();
                ui->editMinMaxDouble->show();
            }else if(command_param_data_type == "int"){
                ui->editMinInt->setValue(currentCommand["CommandParams"][0]["CommandParamRanges"]["min"]);
                ui->editMaxInt->setValue(currentCommand["CommandParams"][0]["CommandParamRanges"]["max"]);
                ui->editMinMaxDouble->hide();
                ui->editMinMaxInt->show();
            } else {
                ui->editMinMaxInt->hide();
                ui->editMinMaxDouble->hide();
            }
        } else {
            ui->editMinMaxInt->hide();
            ui->editMinMaxDouble->hide();
        }

        ui->editCommandCode->document()->setPlainText(QString::fromStdString(command_code));
        ui->editCommandName->setText(QString::fromStdString(command_name));

        int index = ui->editParameterChoose->findText(QString::fromStdString(command_param_data_type));
        if ( index != -1 ) {
           ui->editParameterChoose->setCurrentIndex(index);
        }
        index = ui->editReturnChoose->findText(QString::fromStdString(command_return_data_type));
        if ( index != -1 ) {
           ui->editReturnChoose->setCurrentIndex(index);
        }
    }
}

void Configurator::taskSelectionChanged(const QString&) {
    cout << "task selection changed" << endl;

    if(ui->editChooseCombobox->currentIndex() >= 0) {
        json currentTask = currentHandler->tsl[ui->editChooseCombobox->currentIndex()];
        string task_name = currentTask["TaskName"];
        string task_short_desc = currentTask["TaskDesc"]["ShortDesc"];
        string task_long_desc = currentTask["TaskDesc"]["LongDesc"];
        string task_uid = currentTask["TaskUID"];
        json task_commands_available = currentTask["CommandsAvailable"];

        ui->editTaskLongDesc->document()->setPlainText(QString::fromStdString(task_long_desc));
        ui->editTaskShortDesc->setText(QString::fromStdString(task_short_desc));
        ui->editTaskName->setText(QString::fromStdString(task_name));
        ui->editUIDLabel->setText(QString::fromStdString(task_uid));

        for(int i = 0; i < ui->editSelectFunctions->count(); ++i)
        {
            QListWidgetItem* item = ui->editSelectFunctions->item(i);
            QWidget* widget = dynamic_cast<QWidget*>(ui->editSelectFunctions->itemWidget(item));
            QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(widget->layout());
            cout << layout->count() << endl;
            QCheckBox* checkBox = dynamic_cast<QCheckBox*>(layout->itemAt(0)->widget());
            checkBox->setChecked(false);
        }
        for (json::iterator it = task_commands_available.begin(); it != task_commands_available.end(); ++it) {
           string command = (*it);
            for(int i = 0; i < ui->editSelectFunctions->count(); ++i)
            {
                QListWidgetItem* item = ui->editSelectFunctions->item(i);
                QWidget* widget = dynamic_cast<QWidget*>(ui->editSelectFunctions->itemWidget(item));
                QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(widget->layout());
                cout << layout->count() << endl;
                QCheckBox* checkBox = dynamic_cast<QCheckBox*>(layout->itemAt(0)->widget());
                if(checkBox->text().toStdString() == command)
                    checkBox->setChecked(true);
            }
        }
    }
}
void Configurator::resetMinMax() {
    ui->addMinMaxDouble->hide();
    ui->addMinMaxInt->hide();
    ui->editMinMaxDouble->hide();
    ui->editMinMaxInt->hide();
}
void Configurator::addParamChanged(const QString&) {
    if(ui->addParameterChoose->currentIndex() >= 0) {
        if (ui->addParameterChoose->currentText().toStdString() == "int") {
            ui->addMinMaxDouble->hide();
            ui->addMinMaxInt->show();
        } else if (ui->addParameterChoose->currentText().toStdString() == "double") {
            ui->addMinMaxInt->hide();
            ui->addMinMaxDouble->show();
        } else {
            ui->addMinMaxDouble->hide();
            ui->addMinMaxInt->hide();
        }
    }
}
void Configurator::editParamChanged(const QString&) {
    if (ui->editParameterChoose->currentIndex() >= 0) {
        if (ui->editParameterChoose->currentText().toStdString() == "int") {
            ui->editMinMaxDouble->hide();
            ui->editMinMaxInt->show();
        } else if (ui->editParameterChoose->currentText().toStdString() == "double") {
            ui->editMinMaxInt->hide();
            ui->editMinMaxDouble->show();
        } else {
            ui->editMinMaxDouble->hide();
            ui->editMinMaxInt->hide();
        }
    }
}

void Configurator::on_editCommandButton_clicked() {
    cout << "Edit Command" << endl;
    json ccm = ConfiguratorHandler::pack_message("CCM");

    ccm["Payload"]["CommandName"] = ui->editCommandName->text().toStdString();
    ccm["Payload"]["CommandReturn"] = ui->editReturnChoose->currentText().toStdString();
    ccm["Payload"]["Code"] = ui->editCommandCode->toPlainText().toStdString();

    if(ui->editParameterChoose->currentText().toStdString() == "double"){
        ccm["Payload"]["CommandParams"] = {{{"CommandParamName",ui->editParameterName->text().toStdString()}, {"CommandParamDataType", ui->editParameterChoose->currentText().toStdString()}, {"CommandParamRanges", {{"min", ui->editMinDouble->value()},{"max",ui->editMaxDouble->value()}}}}};
    } else if (ui->editParameterChoose->currentText().toStdString() == "int") {
        ccm["Payload"]["CommandParams"] = {{{"CommandParamName",ui->editParameterName->text().toStdString()}, {"CommandParamDataType", ui->editParameterChoose->currentText().toStdString()}, {"CommandParamRanges", {{"min", ui->editMinInt->value()},{"max",ui->editMaxInt->value()}}}}};
    } else if (ui->editParameterChoose->currentText().toStdString() == "bool") {
        ccm["Payload"]["CommandParams"] = {{{"CommandParamName",ui->editParameterName->text().toStdString()}, {"CommandParamDataType", ui->editParameterChoose->currentText().toStdString()}}};
    } else {
        ccm["Payload"]["CommandParams"] = {{{"CommandParamDataType", ui->editParameterChoose->currentText().toStdString()}}};
    }

    currentSwocket->send(ccm);
    //////////////
    json message = currentHandler->receive();
    string resp_type = message["MessageType"];
    if(resp_type == "CER") {
        string error = message["Payload"]["ErrorDescription"];
        QMessageBox msgBox;
        msgBox.setText(QString::fromStdString(error));
        msgBox.exec();
    } else {
        currentHandler->cml = message["Payload"];
        json cml = currentHandler->cml;

        cout << cml << endl;
        QStringList commands{};

        for (json::iterator it = cml.begin(); it != cml.end(); ++it) {
            json command = *it;
            string command_name = command["CommandName"];
            commands<<QString::fromStdString(command_name);
        }

        ui->editChoose->clear();
        ui->editChoose->addItems(commands);

    }
}
void Configurator::on_addCommandButton_clicked() {
    cout << "Add Command" << endl;
    json acm = ConfiguratorHandler::pack_message("ACM");

    acm["Payload"]["CommandName"] = ui->addCommandName->text().toStdString();
    acm["Payload"]["CommandReturn"] = ui->addReturnChoose->currentText().toStdString();
    acm["Payload"]["Code"] = ui->addCommandCode->toPlainText().toStdString();

    if(ui->addParameterChoose->currentText().toStdString() == "double"){
        acm["Payload"]["CommandParams"] = {{{"CommandParamName",ui->addParameterName->text().toStdString()}, {"CommandParamDataType", ui->addParameterChoose->currentText().toStdString()}, {"CommandParamRanges", {{"min", ui->addMinDouble->value()},{"max",ui->addMaxDouble->value()}}}}};
    } else if (ui->addParameterChoose->currentText().toStdString() == "int") {
        acm["Payload"]["CommandParams"] = {{{"CommandParamName",ui->addParameterName->text().toStdString()}, {"CommandParamDataType", ui->addParameterChoose->currentText().toStdString()}, {"CommandParamRanges", {{"min", ui->addMinInt->value()},{"max",ui->addMaxInt->value()}}}}};
    } else if (ui->addParameterChoose->currentText().toStdString() == "bool") {
        acm["Payload"]["CommandParams"] = {{{"CommandParamName",ui->addParameterName->text().toStdString()}, {"CommandParamDataType", ui->addParameterChoose->currentText().toStdString()}}};
    } else {
        acm["Payload"]["CommandParams"] = {{{"CommandParamDataType", ui->addParameterChoose->currentText().toStdString()}}};
    }

    currentSwocket->send(acm);
    //////////////
    json message = currentHandler->receive();
    string resp_type = message["MessageType"];
    if(resp_type == "CER") {
        string error = message["Payload"]["ErrorDescription"];
        QMessageBox msgBox;
        msgBox.setText(QString::fromStdString(error));
        msgBox.exec();
    } else {
        currentHandler->cml = message["Payload"];
        json cml = currentHandler->cml;

        cout << cml << endl;
        QStringList commands{};

        for (json::iterator it = cml.begin(); it != cml.end(); ++it) {
            json command = *it;
            string command_name = command["CommandName"];
            commands<<QString::fromStdString(command_name);
        }

        ui->editChoose->clear();
        ui->editChoose->addItems(commands);

        add_commands_to_list(currentHandler, ui->addSelectFunctions, &add_check_boxes);
        add_commands_to_list(currentHandler, ui->editSelectFunctions, &edit_check_boxes);

    }
}
void Configurator::on_deleteCommandButton_clicked() {
    cout << "Delete Command" << endl;
    json dcm = ConfiguratorHandler::pack_message("DCM");

    dcm["Payload"]["CommandName"] = ui->editCommandName->text().toStdString();

    currentSwocket->send(dcm);
    //////////////
    json message = currentHandler->receive();
    string resp_type = message["MessageType"];
    if(resp_type == "CER") {
        string error = message["Payload"]["ErrorDescription"];
        QMessageBox msgBox;
        msgBox.setText(QString::fromStdString(error));
        msgBox.exec();
    } else {
        currentHandler->cml = message["Payload"];
        json cml = currentHandler->cml;

        cout << cml << endl;
        QStringList commands{};

        for (json::iterator it = cml.begin(); it != cml.end(); ++it) {
            json command = *it;
            string command_name = command["CommandName"];
            commands<<QString::fromStdString(command_name);
        }

        ui->editChoose->clear();
        ui->editChoose->addItems(commands);

        add_commands_to_list(currentHandler, ui->addSelectFunctions, &add_check_boxes);
        add_commands_to_list(currentHandler, ui->editSelectFunctions, &edit_check_boxes);
    }
}

void Configurator::on_editTaskButton_clicked() {
    cout << "Edit Task" << endl;
    json cts = ConfiguratorHandler::pack_message("CTS");

    vector<string> functions{};
    for(int i = 0; i<edit_check_boxes.size(); i++) {
        QCheckBox* box = edit_check_boxes[i];
        if(box->isChecked()) {
            cout << box->text().toStdString() << endl;
            functions.push_back(box->text().toStdString());
        }
    }

    cts["Payload"]["TaskName"] = ui->editTaskName->text().toStdString();
    cts["Payload"]["TaskDesc"] = json::object();
    cts["Payload"]["TaskDesc"]["ShortDesc"] = ui->editTaskShortDesc->text().toStdString();
    cts["Payload"]["TaskDesc"]["LongDesc"] = ui->editTaskLongDesc->toPlainText().toStdString();
    cts["Payload"]["CommandsAvailable"] = functions;
    cts["Payload"]["TaskUID"] = ui->editUIDLabel->text().toStdString();
    currentSwocket->send(cts);
    //////////////
    json message = currentHandler->receive();
    string resp_type = message["MessageType"];
    if(resp_type == "TSE") {
        string error = message["Payload"]["ErrorDescription"];
        QMessageBox msgBox;
        msgBox.setText(QString::fromStdString(error));
        msgBox.exec();
    } else {
        currentHandler->tsl = message["Payload"];
        json tsl = currentHandler->tsl;

        cout << tsl << endl;
        QStringList tasks{};

        for (json::iterator it = tsl.begin(); it != tsl.end(); ++it) {
            json task = *it;
            string task_name = task["TaskName"];
            tasks<<QString::fromStdString(task_name);
            cout << "addded task" << endl;
        }
        ui->editChooseCombobox->clear();
        ui->editChooseCombobox->addItems(tasks);

        add_commands_to_list(currentHandler, ui->addSelectFunctions, &add_check_boxes);
        add_commands_to_list(currentHandler, ui->editSelectFunctions, &edit_check_boxes);
    }
}
void Configurator::on_addTaskButton_clicked() {
    cout << "Add Task" << endl;
    json ats = ConfiguratorHandler::pack_message("ATS");

    vector<string> functions{};
    for(int i = 0; i<add_check_boxes.size(); i++) {
        QCheckBox* box = add_check_boxes[i];
        if(box->isChecked()) {
            cout << box->text().toStdString() << endl;
            functions.push_back(box->text().toStdString());
        }
    }

    ats["Payload"]["TaskName"] = ui->addTaskName->text().toStdString();
    ats["Payload"]["TaskDesc"] = json::object();
    ats["Payload"]["TaskDesc"]["ShortDesc"] = ui->addTaskShortDesc->text().toStdString();
    ats["Payload"]["TaskDesc"]["LongDesc"] = ui->addTaskLongDesc->toPlainText().toStdString();
    ats["Payload"]["CommandsAvailable"] = functions;
    currentSwocket->send(ats);
    //////////////
    currentHandler->tsl = (currentHandler->receive())["Payload"];
    json tsl = currentHandler->tsl;

    cout << tsl << endl;
    QStringList tasks{};

    for (json::iterator it = tsl.begin(); it != tsl.end(); ++it) {
        json task = *it;
        string task_name = task["TaskName"];
        tasks<<QString::fromStdString(task_name);
        cout << "addded task" << endl;
    }
    ui->editChooseCombobox->clear();
    ui->editChooseCombobox->addItems(tasks);
}
void Configurator::on_deleteTaskButton_clicked() {
    cout << "Delete Task" << endl;
    json dts = ConfiguratorHandler::pack_message("DTS");
    dts["Payload"]["TaskUID"] = ui->editUIDLabel->text().toStdString();
    currentSwocket->send(dts);
    //////////////
    json message = currentHandler->receive();
    string resp_type = message["MessageType"];
    if(resp_type == "TSE") {
        string error = message["Payload"]["ErrorDescription"];
        QMessageBox msgBox;
        msgBox.setText(QString::fromStdString(error));
        msgBox.exec();
    } else {
        currentHandler->tsl = message["Payload"];
        json tsl = currentHandler->tsl;

        cout << tsl << endl;
        QStringList tasks{};

        for (json::iterator it = tsl.begin(); it != tsl.end(); ++it) {
            json task = *it;
            string task_name = task["TaskName"];
            tasks<<QString::fromStdString(task_name);
            cout << "addded task" << endl;
        }
        ui->editChooseCombobox->clear();
        ui->editChooseCombobox->addItems(tasks);
    }
}

Configurator::~Configurator()
{
    delete ui;
}
