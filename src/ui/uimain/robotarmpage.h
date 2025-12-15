#ifndef ROBOTARMPAGE_H
#define ROBOTARMPAGE_H

#include <QWidget>
#include<QQuickWidget>
class RobotArmPage : public QWidget
{
    Q_OBJECT
public:
    explicit RobotArmPage(QWidget *parent = nullptr);

signals:

private:
    QQuickWidget *m_robot = nullptr;
};

#endif // ROBOTARMPAGE_H
