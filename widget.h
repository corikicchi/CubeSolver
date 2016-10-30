#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTimer>
#define INTERVAL 50
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
    void updateProgress();
    void onCompleted(bool isSuccess, QString solution);
    void appendMessage(QString p_message);

    void on_pushButtonStart_clicked();

    void on_pushButtonStop_clicked();

    void on_lineEditCubeState_textChanged(const QString &arg1);

    void on_pushButtonSolve_clicked();

private:
    // GUIクラス
    Ui::Widget *ui;
    // マルチスレッド管理
    SolverThread worker;
    // Progress計測用タイマー
    QTimer *timer;
    int m_timerCount;

    // サーバーの起動フラグ
    bool ServerIsValid;
    // Solverの処理フラグ
    bool busy;

    // ネットワーク関連変数
    int m_port;
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;

    bool solve(QString p_message);
    void sendData(QString p_message);
    void setColor(char p_color, int p_pos);
};

#endif // WIDGET_H
