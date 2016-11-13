#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include <QDialog>

namespace Ui {
class Multiplayer;
}

class Multiplayer : public QDialog
{
    Q_OBJECT

public:
    explicit Multiplayer(QWidget *parent = 0);
    ~Multiplayer();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::Multiplayer *ui;
};

#endif // MULTIPLAYER_H
