#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QGraphicsScene;
class QGraphicsView;
class QTcpSocket;
class QTcpServer;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void PlaceTiles(int row, int col);

private slots:
    void on_actionNew_Game_triggered();
    void on_actionCo_op_triggered();
    void on_client();
    void on_dataIn();

    void on_actionAbout_triggered();

    void on_actionAbout_Qt_triggered();

private:
    Ui::MainWindow *ui;
    QGraphicsScene* scene;
    QGraphicsView* view;

    QTcpSocket *TheOtherGuy;
    QTcpServer* server;
};

#endif // MAINWINDOW_H
