#ifndef NAVBAR_H
#define NAVBAR_H

#include <QWidget>
#include<QPushButton>
#include<QToolButton>
#include<QVariantHash>
#include<QStringList>
#include<QString>
#include<QVBoxLayout>
#include<QButtonGroup>
#include<QList>
#include<QLabel>
class NavBar : public QWidget
{
    Q_OBJECT
public:
    explicit NavBar(QWidget *parent = nullptr);

    void setItems(const QStringList &itmes);
    void setItems(const QVariantHash &items);


private:
    void setContent();
    void clearWidgets();

signals:
    void toggle(int index);

private:
    QStringList m_itemsFirst;
    QVariantHash m_itemsSecond;
    bool m_isExpanded = false;
    QWidget *m_content = nullptr;
    QVBoxLayout *m_contentLayout = nullptr;
    QButtonGroup* m_group = nullptr;
    QList<QToolButton*> m_headers;
    QList<QWidget*> m_bodies;
    QList<QLabel*> m_arrows;   //箭头
};

#endif // NAVBAR_H
