#ifndef STRUCTTYPE_H
#define STRUCTTYPE_H
#include<QString>
#include<QObject>
struct FieldConfig
{
    Q_GADGET
public:
    QString key;   //数据字段名
    QString label;  //数据显示名称
    QString unit;   //单位
    double min =0.0; // 最小值
    double max = 100.0; //最大值
    QString widget;  //组件类型
};
Q_DECLARE_METATYPE(FieldConfig)

#endif // STRUCTTYPE_H
