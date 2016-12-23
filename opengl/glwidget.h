#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <vector>

#define DEG2RAD (3.14159265358979 / 180.0)

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);

signals:
    // 変位ピクセル量の通知
    void NotifyEyeXdiff(int p_x);
    void NotifyEyeYdiff(int p_y);
    void NotifyClicked(bool flag = false);

public slots:
    // スライダ量が変化したら更新
    inline void eyeXChanged(int p_x)
    // p_x = [0, 360]
    {
        m_eyeX = (double)p_x;
        update();
    }
    inline void eyeYChanged(int p_y)
    // p_y = [10, 170]
    {
        m_eyeY = (double)p_y;
        update();
    }
    // 状態が変化したら更新
    inline void setColorVec(std::vector<char> &p_colors)
    {
        m_colors = p_colors;
        update();
    }

private:
    int m_r;
    int m_eyeX;
    int m_eyeY;

    // color buffer
    std::vector<char> m_colors;

    // mouse drag
    QPoint m_lastPos;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

protected:
    void initializeGL();
    void resizeGL(int p_width, int p_height);
    void paintGL();
};

#endif // GLWIDGET_H
