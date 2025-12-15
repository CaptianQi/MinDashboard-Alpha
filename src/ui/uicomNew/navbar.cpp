#include "navbar.h"
#include<QDebug>
#include<QPushButton>
#include<QToolButton>
#include<QList>
#include<QSpacerItem>
#include<QFile>
NavBar::NavBar(QWidget *parent)
    : QWidget{parent}
{
    this->setContentsMargins(0,0,0,0);
    QVBoxLayout *root = new QVBoxLayout;
    this->setLayout(root);
    m_content  = new QWidget(this);
    root->addWidget(m_content);
    root->setContentsMargins(0,0,0,0);

    m_contentLayout =  new QVBoxLayout;
    m_contentLayout->setSpacing(4);
    m_contentLayout->setContentsMargins(0,0,0,0);
    m_content->setLayout(m_contentLayout);

    m_group = new QButtonGroup(this);
    m_group->setExclusive(true);

    connect(m_group, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &NavBar::toggle);

    this->setObjectName("NavBar");
    m_content->setObjectName("navContent");

    QFile f(":/ui.qss");
    if(f.open(QFile::ReadOnly)){
        this->setStyleSheet(f.readAll());
    }

}

void NavBar::setItems(const QStringList &itmes)
{
    if(itmes.isEmpty()){
        qDebug() << QStringLiteral("导航项为空");
        return;
    }
    m_itemsFirst = itmes;
    m_isExpanded = false;
    setContent();
}

void NavBar::setItems(const QVariantHash &items)
{

    if(items.isEmpty()){
        qDebug() << QStringLiteral("导航项为空");
        return;
    }
    m_itemsSecond = items;
    m_isExpanded = true;
    setContent();
}

void NavBar::setContent()
{
    clearWidgets();

    // 重新构建时，清掉 group 里的按钮引用
    const auto buttons = m_group->buttons();
    for (auto *b : buttons) m_group->removeButton(b);

    if(!m_isExpanded){
        //只有一级菜单栏
        for (int i = 0; i < m_itemsFirst.size(); ++i) {
            auto *btn = new QPushButton(m_itemsFirst[i], m_content);
            btn->setCheckable(true);
            btn->setMinimumHeight(40);
            btn->setObjectName("navItem");
        //    btn->setStyleSheet("text-align: left;");
            m_contentLayout->addWidget(btn);
            m_group->addButton(btn, i);
        }

    }else{
        //二级菜单栏
        int id = 0;
        m_headers.clear();
        m_arrows.clear();
        m_bodies.clear();

        const auto oldBtns = m_group->buttons();
        for(auto *b: oldBtns) m_group->removeButton(b);

        for(auto it = m_itemsSecond.begin(); it!=m_itemsSecond.end();++it){
            const QString sectionTitle = it.key();
            //子
            QStringList items = it.value().toStringList();

            //一级头
            auto *header = new QToolButton(m_content);
            header->setObjectName("navHeader");
            header->setText(sectionTitle);
            header->setChecked(false);
            header->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            header->setArrowType(Qt::RightArrow);  //箭头
            header->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
            header->setMinimumHeight(40);

            //二级项容器   用Widget包裹
            auto *body = new QWidget(m_content);
            body->setObjectName("navBody");
            auto *bodyLayout = new QVBoxLayout(body);
            bodyLayout->setContentsMargins(12,6,12,6);
            bodyLayout->setSpacing(6);

            //默认收起
            body->setVisible(false);

            //二级按钮
            for(const auto &text: items){
                auto *btn = new QPushButton(text,body);
                btn->setCheckable(true);
                btn->setMinimumHeight(34);
                btn->setCursor(Qt::PointingHandCursor);
                btn->setObjectName("navSubItem");
                bodyLayout->addWidget(btn);

                m_group->addButton(btn,id);
                id++;
            }
          //  bodyLayout->addStretch();

            //加入导航中
            m_contentLayout->addWidget(header);
            m_contentLayout->addWidget(body);

            // 展开/收起
            connect(header,&QToolButton::clicked,this,[=](){
                bool on = !body->isVisible();

                // 先全部收起
                for (int i = 0; i < m_bodies.size(); ++i) {
                    m_bodies[i]->setVisible(false);
                    m_headers[i]->setArrowType(Qt::RightArrow);
                }

                // 再展开当前
                body->setVisible(on);
                header->setArrowType(on ? Qt::DownArrow : Qt::RightArrow);
            });


            m_headers.push_back(header);
            m_bodies.push_back(body);
        }
    }

    m_contentLayout->addStretch();

    // todo 默认展开项
}

void NavBar::clearWidgets()
{
    if(!m_contentLayout) return;

    QLayoutItem* item = nullptr;
    while ((item = m_contentLayout->takeAt(0)) != nullptr) {
        if (auto w = item->widget()) w->deleteLater();
        delete item;
    }
}
