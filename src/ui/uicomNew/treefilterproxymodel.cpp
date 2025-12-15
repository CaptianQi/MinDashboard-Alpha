#include "treefilterproxymodel.h"
#include<QAbstractItemModel>
#include<QRegularExpression>
#include<QRegExp>
TreeFilterProxyModel::TreeFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

bool TreeFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    if (!index.isValid())
        return false;

    // 1) 自己匹配 → 保留
    if (rowMatches(index))
        return true;

    // 2) 任意后代匹配 → 父也必须保留
    if (hasAcceptedChildren(index))
        return true;

    // 3) 否则过滤掉
    return false;
}

bool TreeFilterProxyModel::hasAcceptedChildren(const QModelIndex &parentIndex) const
{
    const int rows = sourceModel()->rowCount(parentIndex);
    for (int r = 0; r < rows; ++r) {
        const QModelIndex child = sourceModel()->index(r, 0, parentIndex);
        if (!child.isValid()) continue;

        // 子节点自己匹配，或者子节点下面还有匹配的
        if (rowMatches(child) || hasAcceptedChildren(child))
            return true;
    }
    return false;
}

bool TreeFilterProxyModel::rowMatches(const QModelIndex &index) const
{
    //用 DisplayRole 做匹配
    const QString text = index.data(Qt::DisplayRole).toString();
    if(text.isEmpty()) return false;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QRegularExpression re = filterRegularExpression();
    if (!re.isValid() || re.pattern().isEmpty())
        return true;
    return re.match(text).hasMatch();
#else
    QRegExp re = filterRegExp();
    if (re.isEmpty())
        return true;
    return re.indexIn(text) != -1;
#endif

}
