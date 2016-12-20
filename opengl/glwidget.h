#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <vector>

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);

signals:

public slots:
    void eyeX_Changed(int p_x);
    void eyeY_Changed(int p_y);
    // カラー通知
    void setColorVec(std::vector<char> &p_colors);

private:
    int m_r;
    double m_eyeX;
    double m_eyeY;
    // カラーbuffer
    std::vector<char> m_colors;

protected:
    void initializeGL();
    void resizeGL(int p_width, int p_height);
    void paintGL();
};

#endif // GLWIDGET_H
