#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTimer>
#define INTERVAL 100
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
    void onCompleted(bool isSuccess = false, QString solution = "");
    void appendMessage(QString p_message);
    void appendSolverMessage(QString p_message);

    void on_pushButtonStart_clicked();

    void on_pushButtonStop_clicked();

    void on_lineEditCubeState_textChanged(const QString &arg1);

    void on_pushButtonSolve_clicked();

    void on_checkBoxDetail_stateChanged(int arg1);

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

    // BEACONトレース用バッファ
    QString beaconSolution;
    QStringList beaconStrList;
    char m_colorsArray[9 * 6];
    void applyBeaconMove(const QString p_moveStr);
    void turnFrontalFace(const int p_centerNum, const int p_applyTimes);
    void rotateSideFace(const char p_face, const int p_applyTimes);


    bool solve(int p_timeOut, QString p_message);
    void sendData(QString p_message);
    void setColor(char p_color, int p_pos);
};

#endif // WIDGET_H
