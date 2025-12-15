#ifndef TREEFILTERPROXYMODEL_H
#define TREEFILTERPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>
#include<QModelIndex>
class TreeFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit TreeFilterProxyModel(QObject *parent = nullptr);
protected:
    bool filterAcceptsRow(int source_row,const QModelIndex &source_parent) const override;
private:
    bool hasAcceptedChildren(const QModelIndex &parentIndex) const;
    bool rowMatches(const QModelIndex &index) const;
};

#endif // TREEFILTERPROXYMODEL_H
