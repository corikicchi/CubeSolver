#include "solverthread.h"

// run前にsetTimeOutとsetStrCubeStateを設定する
void SolverThread::run()
{
    // 解探索を開始
    emit notifyMessage("Start to solve the cube.");

    msleep(m_timeOut);
    QString strSolution = "This is a test message. " + m_message + " " + QString::number(m_timeOut);

    emit notifyMessage("Finish solving the cube.");

    // Solutionを送信
    emit notifySolution(strSolution);
    // 成功を通知
    emit notifyCompleted(true, strSolution);
    // 失敗を通知
    //emit notifyCompleted(false, "");
}
