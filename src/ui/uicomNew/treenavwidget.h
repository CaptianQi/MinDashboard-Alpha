#ifndef TREENAVWIDGET_H
#define TREENAVWIDGET_H

#include <QWidget>
#include<QStandardItemModel>
#include<QStandardItem>
#include<QModelIndex>
#include<QLineEdit>
#include<QSortFilterProxyModel>
#include<QTreeView>
#include<QPoint>
class TreeFilterProxyModel;
class TreeNavWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TreeNavWidget(QWidget *parent = nullptr);

    void buildDemoData();  //测试数据
    //外部模型中数据角色要符合内部角色配置要求
    void setSourceModel(QStandardItemModel *model);  //外部源模型

signals:
    void nodeActivated(int nodeId);
private:
    //业务角色
    enum Role{
        RoleNodeId = Qt::UserRole+1,
        RoleNodeType = Qt::UserRole+2,
        RoleNodePath = Qt::UserRole+3
    };

    //节点种类
    enum NodeType{
        Folder =1,
        File = 2
    };
private:
    int allocId(){return m_nextId++;}
    void expandToFirstVisibleLeaf();
    // todo 数据量大时，可实现懒加载，注意对过滤产生的影响
private slots:
    void onTreeClicked(const QModelIndex &proxyIndex);
    void onFilterTextChanged(const QString &text);
    void onContextMenuRequested(const QPoint &pos);
private:
    QLineEdit *m_search = nullptr;
    QTreeView *m_tree = nullptr;

    QStandardItemModel *m_model = nullptr;
    TreeFilterProxyModel *m_proxy = nullptr;
    int m_nextId = 1000;
};

#endif // TREENAVWIDGET_H
