#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QObject>
#include<QVariant>
#include<QVariantMap>
#include<QMap>
#include<QString>

/**
 * @brief The DataModel class
 * 保存解析出的键值对  有新值时才更新
 * 当某个值更新时发出信号
 * 为UI层，提供实时数据
 */
class DataModel : public QObject
{
    Q_OBJECT
public:
    explicit DataModel(QObject *parent = nullptr);

    //更新数据
    void updateFrame(const QVariantMap &frame);
    //返回所有键 调试用
    QStringList keys() const;

signals:
    //当某键的值改变时发出
    void valueChanged(const QString &key,const QVariant &value);
    //更新整帧
    void frameUpdated(const QVariantMap &frame);
private:
    QMap<QString,QVariant> m_data;
};

#endif // DATAMODEL_H
