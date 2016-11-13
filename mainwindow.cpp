#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "board.h"
#include "multiplayer.h"

#include <QGraphicsView>
#include <QtWidgets>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGradient>
#include <QtNetwork>

board* TheBoard = nullptr;
QString IPAddress;
int Port;
int Mode;
bool IsInMP = false;
QTcpSocket* OtherP;
QGraphicsScene* MyScene;

class QTile : public QGraphicsRectItem {
public:
    int Row, Col;
    QGraphicsSimpleTextItem* text;
    QTile(int x, int y, int w, int h, int row, int col) : QGraphicsRectItem(x, y, w, h)
    {
        setPen(QPen(Qt::white));
        Row = row;
        Col = col;
        text = new QGraphicsSimpleTextItem(this);
        text->setPos(x + 15, y + 15);
        text->setBrush(Qt::white);
        UpdateFromBoard();
    }

    void UpdateFromBoard() {
        if(TheBoard->gameState == gameover) {
            if(IsInMP && OtherP) {
                char send[5] = "!!GG";
                OtherP->write(send, 4);
            }
            setBrush(Qt::red);
            text->setVisible(false);
            return;
        } else if(TheBoard->gameState == finished) {
            if(IsInMP && OtherP) {
                char send[5] = "!!FF";
                OtherP->write(send, 4);
            }
            setBrush(Qt::green);
            text->setVisible(false);
            return;
        }
        QLinearGradient linearGrad(boundingRect().topLeft(), boundingRect().bottomLeft());
        switch(TheBoard->tiles[Row][Col].flagType) {
        case normal:
            linearGrad.setColorAt(0, Qt::lightGray);
            linearGrad.setColorAt(0.5, QColor(20, 20, 190));
            linearGrad.setColorAt(1, QColor(20, 20, 255));
            setBrush(linearGrad);
            text->setVisible(false);
            break;
        case flag:
            linearGrad.setColorAt(0, Qt::lightGray);
            linearGrad.setColorAt(1, QColor(255, 100, 98));
            setBrush(linearGrad);
            text->setVisible(true);
            text->setText("!");
            break;
        case complete:
            linearGrad.setColorAt(0, QColor(128, 128, 200));
            linearGrad.setColorAt(1, QColor(70, 70, 255));
            setBrush(linearGrad);
            text->setText(QString::number(TheBoard->tiles[Row][Col].numMines));
            text->setVisible(true);
            break;
        }
    }

    // QGraphicsItem interface
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override
    {
        if(event->button() == Qt::LeftButton) {
            TheBoard->lmbPressed(Row, Col);
            if(IsInMP && OtherP) {
                char send[4] = "LD";
                send[2] = Row;
                send[3] = Col;
                OtherP->write(send, 4);
            }
        } else if(event->button() == Qt::MidButton) {
            TheBoard->mmbPressed(Row, Col);
            if(IsInMP && OtherP) {
                char send[4] = "MD";
                send[2] = Row;
                send[3] = Col;
                OtherP->write(send, 4);
            }
        } else if(event->button() == Qt::RightButton) {
            TheBoard->rmbPressed(Row, Col);
            if(IsInMP && OtherP) {
                char send[4] = "RD";
                send[2] = Row;
                send[3] = Col;
                OtherP->write(send, 4);
            }
        }
        auto* sc = scene();
        sc->invalidate(0,0,40*TheBoard->rows,40*TheBoard->cols);
    }
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override
    {
        if(event->button() == Qt::LeftButton) {
            TheBoard->lmbReleased(Row, Col);
            if(IsInMP && OtherP) {
                char send[4] = "LU";
                send[2] = Row;
                send[3] = Col;
                OtherP->write(send, 4);
            }
        } else if(event->button() == Qt::MidButton) {
            TheBoard->mmbReleased(Row, Col);
            if(IsInMP && OtherP) {
                char send[4] = "MU";
                send[2] = Row;
                send[3] = Col;
                OtherP->write(send, 4);
            }
        } else if(event->button() == Qt::RightButton) {
            TheBoard->rmbReleased(Row, Col);
            if(IsInMP && OtherP) {
                char send[4] = "RU";
                send[2] = Row;
                send[3] = Col;
                OtherP->write(send, 4);
            }
        }
        auto* sc = scene();
        sc->invalidate(0,0,40*TheBoard->rows,40*TheBoard->cols);
    }

    // QGraphicsItem interface
public:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        UpdateFromBoard();
        QGraphicsRectItem::paint(painter, option, widget);
    }
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    scene(nullptr)
{
    TheBoard = new board(16, 16, 35);
    ui->setupUi(this);
    view = new QGraphicsView(ui->centralWidget);
    ui->verticalLayout_2->addWidget(view);
    PlaceTiles(TheBoard->rows, TheBoard->cols);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::PlaceTiles(int row, int col)
{
    if(scene)
        delete scene;
    scene = new QGraphicsScene();
    for(int i = 0; i < col; i++)
        for(int j = 0; j < row; j++)
        {
            QTile* tile = new QTile(i * 40, j * 40, 40, 40, j + 1, i + 1);
            scene->addItem(tile);
        }
    view->setScene(scene);
    MyScene = scene;
}

void MainWindow::on_actionNew_Game_triggered()
{
    if(TheBoard)
        delete TheBoard;
    TheBoard = new board(16, 16, 35);
    PlaceTiles(TheBoard->rows, TheBoard->cols);
}

void MainWindow::on_actionCo_op_triggered()
{
    Multiplayer* dialog = new Multiplayer(this);
    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->exec();
    if(Mode == 0)
    { //Client
        TheOtherGuy = new QTcpSocket(this);
        OtherP = TheOtherGuy;
        connect(TheOtherGuy, &QIODevice::readyRead, this, &MainWindow::on_dataIn);
        TheOtherGuy->connectToHost(IPAddress, Port);
        ui->statusBar->showMessage("Waiting for server to respond");
    }
    else if(Mode == 1)
    { //Server
        server = new QTcpServer(this);
        if(!server->listen()) {
            ui->statusBar->showMessage("Failed to listen");
            return;
        }
        connect(server, &QTcpServer::newConnection, this, &MainWindow::on_client);
        ui->statusBar->showMessage(tr("Waiting for client to connect at %1").arg(server->serverPort()));
    }
}

void MainWindow::on_client()
{
    IsInMP = true;
    TheOtherGuy = server->nextPendingConnection();
    connect(TheOtherGuy, &QIODevice::readyRead, this, &MainWindow::on_dataIn);
    OtherP = TheOtherGuy;
    TheOtherGuy->write("CCCC", 4);
    ui->statusBar->showMessage("Data transferred");
    on_actionNew_Game_triggered();
}

void MainWindow::on_dataIn()
{
    auto cmd = TheOtherGuy->read(4);
    qDebug() << cmd;
    if(cmd[0] == 'C')
    {
        IsInMP = true;
        ui->statusBar->showMessage("Connection established");
        on_actionNew_Game_triggered();
        return;
    }
    if(cmd[0] == '!' && cmd[2] == 'G')
    {//The other player GG
        ui->statusBar->showMessage("The other player GGed. You won.");
        return;
    }
    if(cmd[0] == '!' && cmd[2] == 'F')
    {//The other player finished
        ui->statusBar->showMessage("The other player finished. You lose.");
        return;
    }
    int row = cmd[2], col = cmd[3];
    switch(cmd[1]){
    case 'U':
        switch(cmd[0]) {
        case 'L':
            TheBoard->lmbReleased(row, col);
            break;
        case 'M':
            TheBoard->mmbReleased(row, col);
            break;
        case 'R':
            TheBoard->rmbReleased(row, col);
            break;
        }
        break;
    case 'D':
        switch(cmd[0]) {
        case 'L':
            TheBoard->lmbPressed(row, col);
            break;
        case 'M':
            TheBoard->mmbPressed(row, col);
            break;
        case 'R':
            TheBoard->rmbPressed(row, col);
            break;
        }
        break;
    }
    MyScene->invalidate(0,0,40*TheBoard->rows,40*TheBoard->cols);
}
