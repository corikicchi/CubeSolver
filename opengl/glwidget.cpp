#include "glwidget.h"
#include <GL/glu.h>
#include <QtWidgets>

GLWidget::GLWidget(QWidget *parent):
    QGLWidget(parent)
{
    // 初期値設定
    for(int i = 0; i < 9 * 6; i++){
        m_colors.push_back('I');
    }
}

void GLWidget::initializeGL()
{
    // 背景色指定
    //qglClearColor(Qt::lightGray);
    qglClearColor(Qt::white);
    // for 印面消去
    glEnable(GL_DEPTH_TEST);

    // 初期値
    m_eyeX = 30;
    m_eyeY = 60;
}

void GLWidget::resizeGL(int p_width, int p_height)
{
    // ViewPortの設定
    glViewport(0, 0, p_width, p_height);
}

void GLWidget::paintGL()
{
    // バッファをクリア
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 射影行列
    glMatrixMode( GL_PROJECTION );
    // 変換行列を初期化
    glLoadIdentity();

    gluPerspective(30.0,(double)width()/(double)height(), 1.0, 100.0);
    // 極座標
    const double p_r = 10;
    double p_x = p_r * std::sin(m_eyeY * DEG2RAD) * std::cos(m_eyeX * DEG2RAD);
    double p_y = p_r * std::sin(m_eyeY * DEG2RAD) * std::sin(m_eyeX * DEG2RAD);
    double p_z = p_r * std::cos(m_eyeY * DEG2RAD);
    gluLookAt(p_x, p_y, p_z,    //  視点位置
              0.0, 0.0, 0.0,    //  目標位置
              0.0, 0.0, 1.0);   //  上方向

    glMatrixMode( GL_MODELVIEW );   //  モデルビュー行列
    glLoadIdentity();               //  変換行列を初期化
    qglColor(Qt::black);            //  描画色指定

    // vertex行列の生成
    // blockの各接点の(x,y,z)
    GLdouble vertexCube[16 * 6][3];
    // U面
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            vertexCube[i * 4 + j][0] = -1.5 + 1.0 * i;
            vertexCube[i * 4 + j][1] = -1.5 + 1.0 * j;
            vertexCube[i * 4 + j][2] = 1.5;
        }
    }
    // D面
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            vertexCube[16 * 1 + i * 4 + j][0] = 1.5 - 1.0 * i;
            vertexCube[16 * 1 + i * 4 + j][1] = -1.5 + 1.0 * j;
            vertexCube[16 * 1 + i * 4 + j][2] = -1.5;
        }
    }
    // L面
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            vertexCube[16 * 2 + i * 4 + j][0] = -1.5 + 1.0 * j;
            vertexCube[16 * 2 + i * 4 + j][1] = -1.5;
            vertexCube[16 * 2 + i * 4 + j][2] = 1.5 - 1.0 * i;
        }
    }
    // R面
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            vertexCube[16 * 3 + i * 4 + j][0] = 1.5 - 1.0 * j;
            vertexCube[16 * 3 + i * 4 + j][1] = 1.5;
            vertexCube[16 * 3 + i * 4 + j][2] = 1.5 - 1.0 * i;
        }
    }
    // F面
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            vertexCube[16 * 4 + i * 4 + j][0] = 1.5;
            vertexCube[16 * 4 + i * 4 + j][1] = -1.5 + 1.0 * j;
            vertexCube[16 * 4 + i * 4 + j][2] = 1.5 - 1.0 * i;
        }
    }
    // B面
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            vertexCube[16 * 5 + i * 4 + j][0] = -1.5;
            vertexCube[16 * 5 + i * 4 + j][1] = 1.5 - 1.0 * j;
            vertexCube[16 * 5 + i * 4 + j][2] = 1.5 - 1.0 * i;
        }
    }

    // face配列の生成
    // vertex配列の番号を頂点したものがblock
    int face[9 * 6][4] = {
        {0, 1, 5, 4},
        {1, 2, 6, 5},
        {2, 3, 7, 6},
        {4, 5, 9, 8},
        {5, 6, 10, 9},
        {6, 7, 11, 10},
        {8, 9, 13, 12},
        {9, 10, 14, 13},
        {10, 11, 15, 14}
    };

    // すべての番号を16ずらしながら格納していく
    for(int i = 1; i < 6; i++){
        // 各面の参照
        for(int j = 0; j < 9; j++){
            // 各ブロックの参照
            for(int k = 0; k < 4; k++){
                // 座標番号の参照
                face[i * 9 + j][k] = face[j][k] + 16 * i;
            }
        }
    }

    glBegin(GL_QUADS);

    // face描画
    for (int i = 0; i < 9 * 6; i++) {
        if(i == m_colors.size()) break;

        switch(m_colors.at(i)){
        case 'O':
            glColor3f(0xff / (double)0xff, 0x8c / (double)0xff, 0x00 / (double)0xff);
            break;
        case 'W':
            glColor3f(0xff / (double)0xff, 0xff / (double)0xff, 0xff / (double)0xff);
            break;
        case 'B':
            glColor3f(0x00 / (double)0xff, 0x00 / (double)0xff, 0xff / (double)0xff);
            break;
        case 'G':
            glColor3f(0x22 / (double)0xff, 0x8b / (double)0xff, 0x22 / (double)0xff);
            break;
        case 'R':
            glColor3f(0xff / (double)0xff, 0x00 / (double)0xff, 0x00 / (double)0xff);
            break;
        case 'Y':
            glColor3f(0xff / (double)0xff, 0xff / (double)0xff, 0x00 / (double)0xff);
            break;
        default:
            glColor3f(0x50 / (double)0xff, 0x50 / (double)0xff, 0x50 / (double)0xff);
            break;
        }

        // 四角形を描画
        glVertex3dv(vertexCube[face[i][0]]);
        glVertex3dv(vertexCube[face[i][1]]);
        glVertex3dv(vertexCube[face[i][2]]);
        glVertex3dv(vertexCube[face[i][3]]);
    }
    glEnd();

    // edge行列の生成
    // 各faceの境界線の端点
    int edge[8 * 6][2] = {
        {0, 3},
        {4, 7},
        {8, 11},
        {12, 15},
        {0, 12},
        {1, 13},
        {2, 14},
        {3, 15}
    };
    for(int i = 1; i < 6; i++){
        // 各面の参照
        for(int j = 0; j < 8; j++){
            // 各edgeの参照
            for(int k = 0; k < 2; k++){
                // 座標番号の参照
                edge[i * 8 + j][k] = edge[j][k] + 16 * i;
            }
        }
    }

    qglColor(Qt::black);
    glLineWidth(5);
    glBegin(GL_LINES);
    for (int i = 0; i < 8 * 6; ++i) {
        glVertex3dv(vertexCube[edge[i][0]]);
        glVertex3dv(vertexCube[edge[i][1]]);
    }
    glEnd();

    glFlush();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
    emit NotifyClicked();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        // 変位ピクセル数を通知する
        emit NotifyEyeXdiff(dx);
        emit NotifyEyeYdiff(dy);
    }

    m_lastPos = event->pos();
}
