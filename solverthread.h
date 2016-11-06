#ifndef SOLVERTHREAD_H
#define SOLVERTHREAD_H

#include <QThread>

class SolverThread : public QThread
{
    Q_OBJECT
public:
    void setTimeOut(qint64 p_timeOut)
    {
        m_timeOut = p_timeOut;
    }
    void setStrCubeState(QString p_message)
    {
        m_message = p_message;
    }

protected:
    // run前にsetTimeOutとsetStrCubeStateを設定する
    virtual void run();

signals:
    void notifyCompleted(bool isSuccess, QString solution);
    void notifyMessage(QString p_message);
    void notifyTimeOutValue(QString p_message);
    void notifyCubeState(QString p_message);
    void notifySolution(QString p_message);
    void notifyProgress(int p_progress);
    void notifySolverMessage(QString p_message);

private:
    qint64 m_timeOut;
    QString m_message;
};

#endif // SOLVERTHREAD_H
