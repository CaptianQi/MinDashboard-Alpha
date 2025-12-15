#include "robotarmpage.h"

#include <QQmlContext>
#include<QHBoxLayout>
#include<QUrl>
RobotArmPage::RobotArmPage(QWidget *parent)
    : QWidget{parent}
{

    auto *hLayout = new QHBoxLayout(this);

    // m_robot = new QQuickWidget;
    // m_robot->setSource(QUrl("qrc:/src/ui/qml/RobotArmPrimitive.qml"));
    // m_robot->setResizeMode(QQuickWidget::SizeRootObjectToView);

    // QWidget *container = QWidget::createWindowContainer(m_robot,this);
    // hLayout->addWidget(container);


}
