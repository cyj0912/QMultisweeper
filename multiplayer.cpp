#include "multiplayer.h"
#include "ui_multiplayer.h"

extern QString IPAddress;
extern int Port;
extern int Mode;

Multiplayer::Multiplayer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Multiplayer)
{
    ui->setupUi(this);
    ui->comboBox->addItem("Client");
    ui->comboBox->addItem("Server");
    ui->comboBox->setCurrentIndex(0);
}

Multiplayer::~Multiplayer()
{
    delete ui;
}

void Multiplayer::on_buttonBox_accepted()
{
    if(ui->comboBox->currentIndex() == 0)
        Mode = 0;
    else
        Mode = 1;
    IPAddress = ui->ipEdit->text();
    Port = ui->portEdit->text().toInt();
    delete this;
}

void Multiplayer::on_buttonBox_rejected()
{
    Mode = -1;
    delete this;
}
