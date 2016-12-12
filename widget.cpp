#include "widget.h"
#include "ui_widget.h"

#include <cstring>
#include <cctype>
#include <algorithm>
#include <QTcpSocket>
#include <QDateTime>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 初期設定

    // ServerとSolverをStop
    ServerIsValid = false;
    busy = false;
    //timer = nullptr;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateProgress()));

    // GUIの初期設定
    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);
    ui->lineEditPort->setEnabled(true);
    ui->lineEditCubeState->clear();
    ui->lineEditSolution->clear();
    ui->textBrowser->setFontPointSize(10);

    // CubeViewerの初期設定
    for(int i = 0; i < 54; i++) setColor('I', i);

    // ポート番号の取得
    m_port = ui->lineEditPort->text().toInt();

    // 別スレッドとのパイプを設定
    connect(&worker, SIGNAL(notifyMessage(QString)), this, SLOT(appendMessage(QString)));
    connect(&worker, SIGNAL(notifyTimeOutValue(QString)), ui->lineEditTimeOut, SLOT(setText(QString)));
    connect(&worker, SIGNAL(notifyCubeState(QString)), ui->lineEditCubeState, SLOT(setText(QString)));
    connect(&worker, SIGNAL(notifySolution(QString)), ui->lineEditSolution, SLOT(setText(QString)));
    connect(&worker, SIGNAL(notifyCompleted(bool,QString)), this, SLOT(onCompleted(bool,QString)));
    connect(&worker, SIGNAL(notifySolverMessage(QString)), this, SLOT(appendSolverMessage(QString)));

    // GUI connection
    connect(ui->pushButtonSolverClear, SIGNAL(clicked(bool)), ui->textBrowserSolver, SLOT(clear()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::acceptConnection()
{
    // クライアントのソケットを取得
    tcpSocket = tcpServer->nextPendingConnection();
    // クライアントのアドレスを取得
    QString clientAddr = tcpSocket->localAddress().toString();
    int clientPort = tcpSocket->localPort();

    appendMessage("The client has connected from [Address | " + clientAddr + "] [Port | " + QString::number(clientPort) + "].");

    // 接続終了時の処理
    connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(disConnection()));
    // データが発生した時の処理
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(receiveData()));
}

void Widget::disConnection()
{
    tcpSocket->deleteLater();
    tcpSocket = NULL;

    appendMessage("The client has been disconnected.");
    ui->checkBoxBeacon->setChecked(false);
    ui->lineEditBeaconNum->clear();
    ui->lineEditBeaconStr->clear();
}

void Widget::receiveData()
{
    char buffer[4096] = {'\0'};
    qint64 len = 0;

    while((len = tcpSocket->bytesAvailable()) != 0){
        // データの取得
        tcpSocket->read(buffer, tcpSocket->bytesAvailable());
        buffer[len] = '\0';
    }

    // "BEACON"を探索する
    if(!std::strncmp(buffer, "BEACON", 6)){
        appendMessage("The server has received a BEACON message.");
        QStringList strList = QString(buffer).trimmed().split(" "); // trimしてSpaceで分割

        if(strList.size() != 2){
            // BEACON形式がおかしい
            appendMessage("Invalid BEACON.");
            return;
        }

        if(QString(strList.at(1)).trimmed() == "START"){
            // BEACON START
            appendMessage("Start BEACON.");
            ui->checkBoxBeacon->setChecked(true);

            // Solutionを保存
            beaconSolution = ui->lineEditSolution->text().trimmed();
            beaconStrList = beaconSolution.split(" ");

            if(QString(beaconStrList.at(0)).toInt()){
                // 初めがtimeOutの値だったら削除する
                beaconStrList.removeAt(0);
            }

            // timeOutと'.'を削除したStringを再作成する
            beaconSolution = "";
            for(int i = 0; i < beaconStrList.size(); i++){
                if(QString(beaconStrList.at(i)) == ".") continue;
                beaconSolution += QString(beaconStrList.at(i)) + " ";
            }
            beaconSolution.trimmed();
            appendMessage(beaconSolution);
            beaconStrList = beaconSolution.split(" ");

            return;
        }
        else if(QString(strList.at(1)).trimmed() == "STOP"){
            // BEACON STOP
            appendMessage("Stop BEACON.");
            ui->checkBoxBeacon->setChecked(false);
            ui->lineEditBeaconNum->clear();
            ui->lineEditBeaconStr->clear();

            return;
        }
        else{
            bool num_flag = true;

            for(int i = 0; i < QString(strList.at(1)).trimmed().size(); i++){
                if(!std::isdigit(QString(strList.at(1)).trimmed().toStdString().c_str()[i])){
                    num_flag = false;
                }
            }

            if(num_flag && ui->checkBoxBeacon->isChecked()){
                // BEACON num を使って更新
                int beacon_num = QString(strList.at(1)).toInt();
                appendMessage("BEACON " + QString(strList.at(1)));

                // 該当番号の回転記号を取得して操作を加える
                // 番号を表示
                ui->lineEditBeaconNum->setText(QString(strList.at(1)));
                if(beacon_num < beaconStrList.size()){
                    // 回転記号を表示
                    ui->lineEditBeaconStr->setText(beaconStrList.at(beacon_num));
                }
                else {
                    ui->lineEditBeaconStr->setText("ERR");
                }
            }
            else if(!ui->checkBoxBeacon->isChecked()){
                appendMessage("Beacon Mode is not started.");
            }
            else{
                // BEACON形式がおかしい
                appendMessage("Invalid BEACON.");
            }

            return;
        }
    }
    else{
        appendMessage("The server has received a cube state.");
        // 処理を続ける
    }

    // 受信できたのでデータをparse
    if(busy || worker.isRunning()){
        appendMessage("Solver is busy now.");
        if(ServerIsValid){
            sendData("busy");
        }
        return;
    }
    appendMessage("Start to parse a cube state data.");
    ui->progressBar->setValue(0);
    ui->checkBoxBeacon->setChecked(false);
    // とりあえず現在のtimeOutを取得
    int timeOut = ui->lineEditTimeOut->text().toInt();

    // Cube Stateのparseを行う
    QString strCubeState = "";
    QStringList strList = QString(buffer).trimmed().split(" "); // Spaceで分割
    if(strList.size() == 7){
        // タイムアウト情報があると判断
        timeOut = QString(strList.at(0)).toInt();
        if(timeOut == 0){
            // Syntax Error
            appendMessage("Couldn't parse a cube state data. (Invalid Time Out Value)");
            // 失敗を通知
            onCompleted(false, "");
            return;
        }
        // strListを連結する
        for(int i = 1; i < 7; i++){
            strCubeState += QString(strList.at(i)) + " ";
        }
    }
    else if(strList.size() == 6){
        // タイムアウト情報がないと判断
        // timeOutはそのまま
        // strListを連結する
        for(int i = 0; i < 6; i++){
            strCubeState += QString(strList.at(i)) + " ";
        }
    }
    else{
        // Syntax Error
        appendMessage("Couldn't parse a cube state data. (Syntax Error)");
        // 失敗を通知
        onCompleted(false, "");
        return;
    }

    // timeOutの値を表示
    ui->lineEditTimeOut->setText(QString::number(timeOut));
    // Cube Stateを表示
    strCubeState = strCubeState.trimmed();  // 両端をトリム
    ui->lineEditCubeState->setText(strCubeState);

    appendMessage("Finish parsing a cube state data.");

    // スルーモードの時はそのまま解きます
    if(ui->checkBoxThrough->isChecked()){
        solve(timeOut, strCubeState.trimmed());
    }
}

bool Widget::solve(int p_timeOut, QString p_message)
{
    // Solver処理中は処理を行わないようにする
    if(!busy && !worker.isRunning()){
        busy = true;

        /*
        appendMessage("Start to parse a cube state data.");

        // とりあえず現在のtimeOutを取得
        int timeOut = ui->lineEditTimeOut->text().toInt();

        // Cube Stateのparseを行う
        QString strCubeState = "";
        QStringList strList = p_message.split(" "); // Spaceで分割
        if(strList.size() == 7){
            // タイムアウト情報があると判断
            timeOut = QString(strList.at(0)).toInt();
            if(timeOut == 0){
                // Syntax Error
                appendMessage("Couldn't parse a cube state data. (Invalid Time Out Value)");
                // 失敗を通知
                onCompleted(false, "");
                return false;
            }
            // strListを連結する
            for(int i = 1; i < 7; i++){
                strCubeState += QString(strList.at(i)) + " ";
            }
        }
        else if(strList.size() == 6){
            // タイムアウト情報がないと判断
            // timeOutはそのまま
            // strListを連結する
            for(int i = 0; i < 6; i++){
                strCubeState += QString(strList.at(i)) + " ";
            }
        }
        else{
            // Syntax Error
            appendMessage("Couldn't parse a cube state data. (Syntax Error)");
            // 失敗を通知
            onCompleted(false, "");
            return false;
        }

        // timeOutの値を表示
        ui->lineEditTimeOut->setText(QString::number(timeOut));
        // Cube Stateを表示
        strCubeState = strCubeState.trimmed();  // 両端をトリム
        ui->lineEditCubeState->setText(strCubeState);

        appendMessage("Finish parsing a cube state data.");
        */
        // Solverの初期設定
        worker.setTimeOut(p_timeOut);
        worker.setStrCubeState(p_message.trimmed());
        // Solverスタート
        worker.start();

        // timerスタート
        //timer = new QTimer(this);
        //connect(timer, SIGNAL(timeout()), this, SLOT(updateProgress()));
        m_timerCount = 0;
        ui->progressBar->setValue(0);
        timer->start(INTERVAL);

        // timeOutを変更できないようにする
        ui->lineEditTimeOut->setEnabled(false);

        return true;
    }
    else{
        appendMessage("Solver is busy now.");
        if(ServerIsValid && ui->checkBoxThrough->isChecked()){
            sendData("busy");
        }
        return false;
    }
}

void Widget::updateProgress()
{
    double progress = ((double)(m_timerCount) / ui->lineEditTimeOut->text().toDouble()) * 100;
    m_timerCount += (INTERVAL);
    ui->progressBar->setValue((int)progress + 10);
}

void Widget::onCompleted(bool isSuccess, QString solution)
{
    busy = false;
    if(isSuccess){
        if(ServerIsValid){
            sendData(solution.trimmed());
        }
        ui->progressBar->setValue(100);
    }
    else{
        if(!solution.isEmpty()){
            appendMessage(solution);
        }
        appendMessage("Failed to parse/solve the cube.");
        if(ServerIsValid){
            sendData("failed");
        }
        ui->progressBar->setValue(0);
    }

    timer->stop();
    //disconnect(timer, SIGNAL(timeout()), this, SLOT(updateProgress()));
    m_timerCount = 0;
    ui->lineEditTimeOut->setEnabled(true);
}

void Widget::appendMessage(QString p_message)
{
    // 現在の日時を取得
    QDateTime dt = QDateTime::currentDateTime();
    dt = dt.toLocalTime();
    QString str = dt.toString("hh:mm:ss.zzz");
    ui->textBrowser->append("[" + QString(qPrintable(str)) + "] " + p_message);
}

void Widget::appendSolverMessage(QString p_message)
{
    // 現在の日時を取得
    QDateTime dt = QDateTime::currentDateTime();
    dt = dt.toLocalTime();
    QString str = dt.toString("hh:mm:ss.zzz");
    ui->textBrowserSolver->append("[" + QString(qPrintable(str)) + "] " + p_message);
}

void Widget::sendData(QString p_message)
{
    if(tcpSocket != NULL){
        if(!p_message.isEmpty()){
            // Solutionをクライアントに送信
            p_message += '\0';
            tcpSocket->write(p_message.toStdString().c_str(), p_message.toStdString().size());

            appendMessage("Packet has been sent.");
        }
        else{
            // Solutionが無い
            appendMessage("Solution is empty.");
        }
    }
    else{
        // Socketが無い
        appendMessage("Couldn't get client's socket.");
    }
}

void Widget::on_pushButtonStart_clicked()
{
    if(busy){
        appendMessage("Solving now.");
        return;
    }
    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonStop->setEnabled(true);
    ui->lineEditPort->setEnabled(false);

    //ui->lineEditCubeState->clear();
    //ui->lineEditSolution->clear();

    // listenの開始
    tcpServer = new QTcpServer(this);
    tcpSocket = NULL;
    m_port = ui->lineEditPort->text().toInt();

    if(!tcpServer->listen(QHostAddress::Any, m_port)){
        appendMessage("The server couldn't start listening at port " + QString::number(tcpServer->serverPort()) + ".");
        appendMessage(tcpServer->errorString());
        close();
        return;
    }
    appendMessage("The server has started listening at port " + QString::number(tcpServer->serverPort()) + ".");
    ServerIsValid = true;
    // 接続要求があったときの処理
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
}

void Widget::on_pushButtonStop_clicked()
{
    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);
    ui->lineEditPort->setEnabled(true);

    tcpServer->close();
    delete tcpServer;
    appendMessage("The server has finished listening.");
    ServerIsValid = false;
}

void Widget::on_lineEditCubeState_textChanged(const QString &arg1)
{
    // show cube colors
    QStringList strList = arg1.split(" ");
    if(strList.size() <= 6){
        for(int i = 0; i < strList.size(); i++){
            // どこの面か取得
            char faceOrder[7] = "UDLRFB";
            char *name;
            if (!(name = strchr(faceOrder, QString(strList.at(i)).toStdString().c_str()[0]))) {
                return;
            }
            int face = (int)(name - faceOrder); // faceOrder[face]が今の面

            for(int j = 0; j < 11 - 2; j++){
                char ch;
                if(j < QString(strList.at(i)).size() - 2){
                    ch = QString(strList.at(i)).toStdString().c_str()[j + 2];
                }
                else{
                    ch = 'I';
                }
                setColor(ch, face * 9 + j);
            }
        }
    }
}

void Widget::setColor(char p_color, int p_pos)
{
    if(p_pos < 0 || 54 <= p_pos) return;    // Erroe check
    QString color;
    switch (p_color) {
    case 'O':
        color = "#ff8c00";
        break;
    case 'W':
        color = "#ffffff";
        break;
    case 'B':
        color = "#0000ff";
        break;
    case 'G':
        color = "#228b22";
        break;
    case 'R':
        color = "#ff0000";
        break;
    case 'Y':
        color = "#ffff00";
        break;
    case 'I':   // 黒にする
        color = "#000000";
        break;
    default:    // おかしな値
        return;
    }

    switch (p_pos){
    case 0	 : ui->color_0->setStyleSheet("background-color : " + color + ";"); break;
    case 1	 : ui->color_1->setStyleSheet("background-color : " + color + ";"); break;
    case 2	 : ui->color_2->setStyleSheet("background-color : " + color + ";"); break;
    case 3	 : ui->color_3->setStyleSheet("background-color : " + color + ";"); break;
    case 4	 : ui->color_4->setStyleSheet("background-color : " + color + ";"); break;
    case 5	 : ui->color_5->setStyleSheet("background-color : " + color + ";"); break;
    case 6	 : ui->color_6->setStyleSheet("background-color : " + color + ";"); break;
    case 7	 : ui->color_7->setStyleSheet("background-color : " + color + ";"); break;
    case 8	 : ui->color_8->setStyleSheet("background-color : " + color + ";"); break;
    case 9	 : ui->color_9->setStyleSheet("background-color : " + color + ";"); break;
    case 10	 : ui->color_10->setStyleSheet("background-color : " + color + ";"); break;
    case 11	 : ui->color_11->setStyleSheet("background-color : " + color + ";"); break;
    case 12	 : ui->color_12->setStyleSheet("background-color : " + color + ";"); break;
    case 13	 : ui->color_13->setStyleSheet("background-color : " + color + ";"); break;
    case 14	 : ui->color_14->setStyleSheet("background-color : " + color + ";"); break;
    case 15	 : ui->color_15->setStyleSheet("background-color : " + color + ";"); break;
    case 16	 : ui->color_16->setStyleSheet("background-color : " + color + ";"); break;
    case 17	 : ui->color_17->setStyleSheet("background-color : " + color + ";"); break;
    case 18	 : ui->color_18->setStyleSheet("background-color : " + color + ";"); break;
    case 19	 : ui->color_19->setStyleSheet("background-color : " + color + ";"); break;
    case 20	 : ui->color_20->setStyleSheet("background-color : " + color + ";"); break;
    case 21	 : ui->color_21->setStyleSheet("background-color : " + color + ";"); break;
    case 22	 : ui->color_22->setStyleSheet("background-color : " + color + ";"); break;
    case 23	 : ui->color_23->setStyleSheet("background-color : " + color + ";"); break;
    case 24	 : ui->color_24->setStyleSheet("background-color : " + color + ";"); break;
    case 25	 : ui->color_25->setStyleSheet("background-color : " + color + ";"); break;
    case 26	 : ui->color_26->setStyleSheet("background-color : " + color + ";"); break;
    case 27	 : ui->color_27->setStyleSheet("background-color : " + color + ";"); break;
    case 28	 : ui->color_28->setStyleSheet("background-color : " + color + ";"); break;
    case 29	 : ui->color_29->setStyleSheet("background-color : " + color + ";"); break;
    case 30	 : ui->color_30->setStyleSheet("background-color : " + color + ";"); break;
    case 31	 : ui->color_31->setStyleSheet("background-color : " + color + ";"); break;
    case 32	 : ui->color_32->setStyleSheet("background-color : " + color + ";"); break;
    case 33	 : ui->color_33->setStyleSheet("background-color : " + color + ";"); break;
    case 34	 : ui->color_34->setStyleSheet("background-color : " + color + ";"); break;
    case 35	 : ui->color_35->setStyleSheet("background-color : " + color + ";"); break;
    case 36	 : ui->color_36->setStyleSheet("background-color : " + color + ";"); break;
    case 37	 : ui->color_37->setStyleSheet("background-color : " + color + ";"); break;
    case 38	 : ui->color_38->setStyleSheet("background-color : " + color + ";"); break;
    case 39	 : ui->color_39->setStyleSheet("background-color : " + color + ";"); break;
    case 40	 : ui->color_40->setStyleSheet("background-color : " + color + ";"); break;
    case 41	 : ui->color_41->setStyleSheet("background-color : " + color + ";"); break;
    case 42	 : ui->color_42->setStyleSheet("background-color : " + color + ";"); break;
    case 43	 : ui->color_43->setStyleSheet("background-color : " + color + ";"); break;
    case 44	 : ui->color_44->setStyleSheet("background-color : " + color + ";"); break;
    case 45	 : ui->color_45->setStyleSheet("background-color : " + color + ";"); break;
    case 46	 : ui->color_46->setStyleSheet("background-color : " + color + ";"); break;
    case 47	 : ui->color_47->setStyleSheet("background-color : " + color + ";"); break;
    case 48	 : ui->color_48->setStyleSheet("background-color : " + color + ";"); break;
    case 49	 : ui->color_49->setStyleSheet("background-color : " + color + ";"); break;
    case 50	 : ui->color_50->setStyleSheet("background-color : " + color + ";"); break;
    case 51	 : ui->color_51->setStyleSheet("background-color : " + color + ";"); break;
    case 52	 : ui->color_52->setStyleSheet("background-color : " + color + ";"); break;
    case 53	 : ui->color_53->setStyleSheet("background-color : " + color + ";"); break;
    }
}

void Widget::on_pushButtonSolve_clicked()
{
    // 解きます
    /*
    if(ServerIsValid){
        // サーバーを止める
        on_pushButtonStop_clicked();
    }
    */
    solve(ui->lineEditTimeOut->text().toInt(), QString(ui->lineEditCubeState->text()).trimmed());
}
