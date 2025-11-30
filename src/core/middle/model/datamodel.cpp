#include "datamodel.h"
#include"../../log/logger.h"
DataModel::DataModel(QObject *parent)
    : QObject{parent}
{}

//对于每个键： 如果该键不存在，或新值！=旧值-> 更新并发出信号 valueChanged
//最后发出 frameUpdated信号
void DataModel::updateFrame(const QVariantMap &frame)
{
    if(frame.isEmpty()) return;

    for(auto it=frame.constBegin();it!=frame.constEnd();++it){
        const QString &key = it.key();
        const QVariant &newValue = it.value();

        //不存在  或  有新值
        if(!m_data.contains(key)||m_data.value(key)!=newValue){
            m_data[key]= newValue;

            emit valueChanged(key,newValue);
            LOG_INFO(QStringLiteral("更新值：%1=%2")
                     .arg(key,newValue.toString()));
        }
    }
    //更新整帧
    emit frameUpdated(frame);
}


QStringList DataModel::keys() const
{
    return m_data.keys();
}
