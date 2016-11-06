#include "solverthread.h"

#include "solver/idastarsearch.h"
#include "solver/ordinalcube.h"
#include "solver/groupcube.h"
#include "solver/cubeparser.h"

// run前にsetTimeOutとsetStrCubeStateを設定する
void SolverThread::run()
{
    // 解探索を開始
    emit notifyMessage("Start to solve the cube.");

    // 入力データをparseする
    // groupCubeには54 blocksの色情報が格納される
    CGroupCube groupCube;
    CCubeParser::InputError inputStatus;
    if ((inputStatus = CCubeParser::ParseInput(m_message.trimmed().toStdString(), groupCube)) != CCubeParser::VALID){
        //std::cout << CCubeParser::GetErrorText(inputStatus) << std::endl;
        emit notifySolverMessage(QString::fromStdString(CCubeParser::GetErrorText(inputStatus)));
        // 失敗を通知
        emit notifyCompleted(false, "");
        return;
        //exit(1);
    }

    // groupCubeが解けるか(群かどうか)をcheckしながらKociemba's Algorithmで使用する置換
    // (Corner,EdgeそれぞれのOrientations,Permutation)とParityを設定する
    // 解くことができる(群をなしている)のであればCubeに4状態を格納する
    COrdinalCube cube;
    CGroupCube::CubeError cubeStatus;
    if ((cubeStatus = groupCube.SetCubeState(cube)) != CGroupCube::VALID){
        //std::cout << groupCube.GetErrorText(cubeStatus) << std::endl;
        emit notifySolverMessage(QString::fromStdString(groupCube.GetErrorText(cubeStatus)));
        // 失敗を通知
        emit notifyCompleted(false, "");
        return;
        //exit(1);
    }

    CIDAstarSearch idaStarSearch;
    idaStarSearch.InitializeTables();
    idaStarSearch.Solve(cube, m_timeOut);

    emit notifySolverMessage(QString::fromStdString(idaStarSearch.GetSolution()).trimmed());
    QString strSolution = QString::fromStdString(idaStarSearch.GetSolution()).trimmed();

    emit notifyMessage("Finish solving the cube.");

    // Solutionを送信
    emit notifySolution(strSolution);

    if(strSolution.toStdString()[0] == 'S'){   // "Solution was not found"
        emit notifySolverMessage("Solution was not found");
        // 失敗を通知
        emit notifyCompleted(false, "");
    }else{
        // 成功を通知
        emit notifyCompleted(true, strSolution);
    }
}
