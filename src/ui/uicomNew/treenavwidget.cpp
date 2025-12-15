#include "treenavwidget.h"
#include"treefilterproxymodel.h"

#include<QVBoxLayout>
#include<QDebug>
#include<QMenu>
#include<QAction>
#include<QCursor>
#include<QMessageBox>
TreeNavWidget::TreeNavWidget(QWidget *parent)
    : QWidget{parent}
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(6);
    //搜索框
    m_search = new QLineEdit(this);
    m_search->setPlaceholderText(QStringLiteral("搜索..."));
    root->addWidget(m_search);
    //总策略： 视图绑定代理  代理绑定模型
    //视图
    m_tree = new QTreeView(this);
    root->addWidget(m_tree,1);
    //源模型
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(1);

    //代理 过滤
    m_proxy = new TreeFilterProxyModel(this);
    m_proxy->setSourceModel(m_model);
    m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);  //设置过滤器大小写敏感性
    m_proxy->setFilterKeyColumn(0);  //设置筛选关键字列   以第一列为筛选

    //View 显示 proxy
    m_tree->setModel(m_proxy);
    m_tree->setHeaderHidden(true);
    m_tree->setUniformRowHeights(true);
    m_tree->setEditTriggers(QAbstractItemView::EditKeyPressed);  //F2
    m_tree->setContextMenuPolicy(Qt::CustomContextMenu);  //自定义菜单

    //信号连接
    connect(m_tree,&QTreeView::clicked,this,&TreeNavWidget::onTreeClicked);
    connect(m_search,&QLineEdit::textChanged,this,&TreeNavWidget::onFilterTextChanged);
    connect(m_tree,&QTreeView::customContextMenuRequested,this,&TreeNavWidget::onContextMenuRequested);

    //测试
    buildDemoData();
    m_tree->expandToDepth(0);  //展开到指定深度

}

void TreeNavWidget::buildDemoData()
{
    m_model->clear();

    auto *rootItem = m_model->invisibleRootItem();
    //模拟 文件夹
    auto *projectA = new QStandardItem(QStringLiteral("ProjectA"));
    projectA->setData(1,RoleNodeId);
    projectA->setData(Folder,RoleNodeType);
    projectA->setData(QStringLiteral("/ProjectA/src"),RoleNodePath);

    // 子：src（文件夹）
    auto *src = new QStandardItem(QStringLiteral("src"));
    src->setData(2, RoleNodeId);
    src->setData(Folder, RoleNodeType);
    src->setData(QStringLiteral("/ProjectA/src"), RoleNodePath);

    // 子：main.cpp（文件）
    auto *mainCpp = new QStandardItem(QStringLiteral("main.cpp"));
    mainCpp->setData(3, RoleNodeId);
    mainCpp->setData(File, RoleNodeType);
    mainCpp->setData(QStringLiteral("/ProjectA/src/main.cpp"), RoleNodePath);

    src->appendRow(mainCpp);
    projectA->appendRow(src);

    // 模拟：README.md（文件）
    auto *readme = new QStandardItem(QStringLiteral("README.md"));
    readme->setData(4, RoleNodeId);
    readme->setData(File, RoleNodeType);
    readme->setData(QStringLiteral("/ProjectA/README.md"), RoleNodePath);

    projectA->appendRow(readme);
    rootItem->appendRow(projectA);
}

void TreeNavWidget::setSourceModel(QStandardItemModel *model)
{
    if(!model) return;

    m_model = model;
    m_proxy->setSourceModel(model);

    //视图状态重置
    m_tree->setCurrentIndex(QModelIndex());
    m_tree->collapseAll();
    m_tree->expandToDepth(0);
}

void TreeNavWidget::expandToFirstVisibleLeaf()
{
    // 从 proxy 顶层开始找：第一个可见叶子（rowCount==0）
    QModelIndex found;

    std::function<bool(const QModelIndex&)> dfs = [&](const QModelIndex &parent)->bool {
        const int rows = m_proxy->rowCount(parent);
        if (rows == 0) {
            if (parent.isValid()) { found = parent; return true; }
            return false;
        }
        for (int r = 0; r < rows; ++r) {
            QModelIndex child = m_proxy->index(r, 0, parent);
            if (!child.isValid()) continue;
            if (dfs(child)) return true;
        }
        return false;
    };

    dfs(QModelIndex());
    if (!found.isValid()) return;

    // 展开它的祖先链（只展开到它为止）
    QModelIndex p = found.parent();
    while (p.isValid()) {
        m_tree->expand(p);
        p = p.parent();
    }
    // 定位到该节点
    m_tree->setCurrentIndex(found);
    m_tree->scrollTo(found, QAbstractItemView::PositionAtCenter);
}

void TreeNavWidget::onTreeClicked(const QModelIndex &proxyIndex)
{
    if(!proxyIndex.isValid()) return;
    //映射回模型索引
    const QModelIndex srcIndex = m_proxy->mapToSource(proxyIndex);
    if(!srcIndex.isValid()) return;

    const int nodeId = srcIndex.data(RoleNodeId).toInt();
    const int type = srcIndex.data(RoleNodeType).toInt();
    const QString path = srcIndex.data(RoleNodePath).toString();

    emit nodeActivated(nodeId);
}
//过滤
void TreeNavWidget::onFilterTextChanged(const QString &text)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_proxy->setFilterRegularExpression(QRegularExpression(text, QRegularExpression::CaseInsensitiveOption));
#else
    m_proxy->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive, QRegExp::FixedString));
#endif
    // 先恢复默认折叠状态（避免 expandAll 遗留）
    m_tree->collapseAll();
    m_tree->expandToDepth(0);

    if (text.trimmed().isEmpty())
        return;
    // 只展开到“第一个可见叶子/命中项”为止
    expandToFirstVisibleLeaf();
}

void TreeNavWidget::onContextMenuRequested(const QPoint &pos)
{
    //逻辑，ContextMenu信号返回视图坐标系的坐标， 相对于viewport()视口的坐标
    //需转化为 屏幕全局坐标系

    const QModelIndex proxyIndex = m_tree->indexAt(pos);


    QStandardItem *parentItem = nullptr;    //父节点
    QModelIndex srcIndex;   //点击节点
    //若右键空白处： 以root 为父节点
    if(proxyIndex.isValid()){
        srcIndex = m_proxy->mapToSource(proxyIndex);
        parentItem = m_model->itemFromIndex(srcIndex);
    }else{
        parentItem = m_model->invisibleRootItem();
    }

    //菜单
    QMenu menu(this);
    QAction *actNewFolder = menu.addAction(QStringLiteral("新建文件夹"));
    QAction *actNewFile = menu.addAction(QStringLiteral("新建文件"));
    menu.addSeparator();

    QAction *actRename = menu.addAction(QStringLiteral("重命名"));
    QAction *actDelete = menu.addAction(QStringLiteral("删除"));

    //空白处，不能删除和命名
    if(!proxyIndex.isValid()){
        actRename->setEnabled(false);
        actDelete->setEnabled(false);
    }

    //文件处，不能新建文件夹和文件
    if(proxyIndex.data(RoleNodeType)== NodeType::File){
        actNewFile->setEnabled(false);
        actNewFolder->setEnabled(false);
    }

    //显示菜单
    QAction *picked = menu.exec(m_tree->viewport()->mapToGlobal(pos));
    if(!picked) return;

    //-------新建文件夹-------
    if(picked == actNewFolder){
        auto *item = new QStandardItem(QStringLiteral("New Folder"));
        item->setData(allocId(),RoleNodeId);
        item->setData(Folder,RoleNodeType);
        item->setData(QStringLiteral(""),RoleNodePath);

        parentItem->appendRow(item);

        //新建后： 展开父节点，选中并进入编辑
        if(proxyIndex.isValid()){
            m_tree->expand(proxyIndex);
        }
        const QModelIndex newSrcIndex = item->index(); //获取新项的源索引
        const QModelIndex newProxyIndex = m_proxy->mapFromSource(newSrcIndex); // 获取代理索引
        m_tree->setCurrentIndex(newProxyIndex);
        m_tree->scrollTo(newProxyIndex);
        m_tree->edit(newProxyIndex);
        return;
    }

    //-------新建文件-------
    if(picked == actNewFile){
        auto *item = new QStandardItem(QStringLiteral("New File.txt"));
        item->setData(allocId(),RoleNodeId);
        item->setData(File,RoleNodeType);
        item->setData(QStringLiteral(""),RoleNodePath);

        parentItem->appendRow(item);
        //展开
        if(proxyIndex.isValid()){
            m_tree->expand(proxyIndex);
        }

        const QModelIndex newSrcIndex = m_model->indexFromItem(item);
        const QModelIndex newProxyIndex = m_proxy->mapFromSource(newSrcIndex);
        m_tree->setCurrentIndex(newProxyIndex);
        m_tree->scrollTo(newProxyIndex);
        m_tree->edit(newProxyIndex);
        return;
    }

    if(!proxyIndex.isValid()) return;
    // -------重命名-------
    if(picked == actRename){
        m_tree->edit(proxyIndex);
        return;
    }
    // -------删除-------
    if(picked == actDelete){
        const QString name = srcIndex.data(Qt::DisplayRole).toString();
        const auto ret = QMessageBox::question(
            this,QStringLiteral("删除确认"),QStringLiteral("确定删除%1吗？").arg(name)
            );
        if(ret != QMessageBox::Yes) return;

        QStandardItem *item = m_model->itemFromIndex(srcIndex);
        QStandardItem *p = item ? item->parent():nullptr;
        if(!p) p = m_model->invisibleRootItem();

        p->removeRow(item->row());  //自动删除孙子节点
        return;
    }
}
