#include "solverthread.h"

// run前にm_timeOutとm_messageを設定する
void SolverThread::run()
{
    emit notifyMessage("Start to parse a cube state data.");

    // Cube Stateのparseを行う
    QString strCubeState = "";
    QStringList strList = m_message.split(" "); // Spaceで分割
    if(strList.size() == 7){
        // タイムアウト情報があると判断
        m_timeOut = QString(strList.at(0)).toInt();
        if(m_timeOut == 0){
            // Syntax Error
            emit notifyMessage("Couldn't parse a cube state data. (Invalid Time Out Value)");
            // 失敗を通知
            emit notifyCompleted(false, "");
            return;
        }
        // strListを連結する
        for(int i = 1; i < 7; i++){
            strCubeState += QString(strList.at(i)) + " ";
        }
    }
    else if(strList.size() == 6){
        // タイムアウト情報がないと判断
        // strListを連結する
        for(int i = 0; i < 6; i++){
            strCubeState += QString(strList.at(i)) + " ";
        }
    }
    else{
        // Syntax Error
        emit notifyMessage("Couldn't parse a cube state data. (Syntax Error)");
        // 失敗を通知
        emit notifyCompleted(false, "");
        return;
    }

    // timeOutの値を送信
    emit notifyTimeOutValue(QString::number(m_timeOut));
    // Cube Stateを表示
    strCubeState = strCubeState.trimmed();  // 両端をトリム
    // Cube Stateを送信
    emit notifyCubeState(strCubeState);

    emit notifyMessage("Finish parsing a cube state data.");

    // 解探索を開始
    emit notifyMessage("Start to solve the cube.");

    msleep(m_timeOut);
    QString strSolution = "This is a test message.";

    emit notifyMessage("Finish solving the cube.");

    // Solutionを送信
    emit notifySolution(strSolution);
    // 成功を通知
    emit notifyCompleted(true, strSolution);
}
