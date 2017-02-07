#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QMainWindow>
#include "json.hpp"
#include "swockets.hpp"

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
private slots:
    void on_addClientButton_clicked();
};

#endif // CONFIGURATOR_H
