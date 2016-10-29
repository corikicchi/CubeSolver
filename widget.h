#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>

#include "solverthread.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void acceptConnection();
    void disConnection();
    void receiveData();
    void onCompleted(bool isSuccess);
    void appendMessage(QString p_message);

    void on_pushButtonStart_clicked();

    void on_pushButtonStop_clicked();

    void on_lineEditCubeState_textChanged(const QString &arg1);

    void on_pushButtonSolve_clicked();

private:
    Ui::Widget *ui;
    SolverThread worker;

    bool ServerIsValid;
    bool busy;

    int m_port;
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;

    bool solve(QString p_message);
    void sendData(QString p_message);
    void setColor(char p_color, int p_pos);
};

#endif // WIDGET_H
