#include "configurator.h"
#include "ui_configurator.h"

Configurator::Configurator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Configurator)
{
    ui->setupUi(this);
}

Configurator::~Configurator()
{
    delete ui;
}
