#ifndef READPROJECTCONFIG_H
#define READPROJECTCONFIG_H

#include <QObject>
#include<QString>
#include<QList>
#include"../StructType.h"
/**
 * @brief The ReadProjectConfig class
 * 读取并保存项目配置文件JSON
 * 程序启动时，调用json文件
 *
 */

class ReadProjectConfig : public QObject
{
    Q_OBJECT
public:
    explicit ReadProjectConfig(QObject *parent = nullptr);

    //从指定路径加载JSON配置文件
    // todo:后续通过配置文件指定
    bool loadFromFile(const QString &filePath);
    //判断当前配置是否已经成功加载
    bool isValid() const{return m_valid;}

    //项目信息
    QString name() const {return m_name;}
    QString description() const{ return m_description;}
    //所有仪表信息
    const QList<FieldConfig> &fields()const{return m_fields;}
private:
    //清空当前所有数据
    void clear();
private:
    bool m_valid = false;  //加载标记
    //项目信息
    QString m_name;
    QString m_description;
    //仪表字段配置
    QList<FieldConfig> m_fields;
};

#endif // READPROJECTCONFIG_H
