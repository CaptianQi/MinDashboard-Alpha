#include "readprojectconfig.h"
#include"../../log/logger.h"

#include<QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QByteArray>
ReadProjectConfig::ReadProjectConfig(QObject *parent)
    : QObject{parent}
{}

bool ReadProjectConfig::loadFromFile(const QString &filePath)
{
    //先清空旧内容
    clear();
    //打开文件
    QFile file(filePath);
    if(!file.exists()){
        LOG_ERROR(QStringLiteral("文件不存在：%1")
                  .arg(filePath));
        return false;
    }

    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        LOG_ERROR(QStringLiteral("无法打开文件：%1,错误：%2")
                  .arg(filePath)
                  .arg(file.errorString()));
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    if(jsonData.isEmpty()){
        LOG_ERROR(QStringLiteral("文件为空：%1")
                  .arg(filePath));
        return false;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData,&err);

    if(err.error!=QJsonParseError::NoError){
        LOG_ERROR(QStringLiteral("JSON解析错误：%1,位置：%2")
                  .arg(err.errorString())
                  .arg(err.offset));
        return false;
    }

    if(!doc.isObject()){
        LOG_ERROR(QStringLiteral("根节点不是JSON对象: %1")
                  .arg(filePath));
        return false;
    }

    QJsonObject rootObj = doc.object();
    //项目信息 可无
    if(rootObj.contains("project")
        &&rootObj.value("project").isObject()){
        QJsonObject projObj = rootObj.value("project").toObject();

        m_name = projObj.value("name").toString();
        m_description = projObj.value("description").toString();
    }else{
        LOG_WARN(QStringLiteral(" 未找到项目信息：%1")
                 .arg(filePath));
    }

    //仪表项
    if(!rootObj.contains("dashboard")||!rootObj.value("dashboard").isArray()){
        LOG_ERROR(QStringLiteral("未找到仪表数组：%1")
                  .arg(filePath));
        return false;
    }

    QJsonArray dashArray = rootObj.value("dashboard").toArray();

    //遍历所有仪表
    for(const QJsonValue &val: dashArray){
        if(!val.isObject()) continue;

        QJsonObject itemObj = val.toObject();
        FieldConfig field;
        //必须有key
        field.key = itemObj.value("key").toString().trimmed();
        if(field.key.isEmpty()){
            LOG_WARN(QStringLiteral("某仪表缺少key,跳过"));
            continue;
        }

        //读取 label 可选  没有默认用key
        field.label = itemObj.value("label").toString().trimmed();
        if(field.label.isEmpty())
            field.label = field.key;

        //单位 可选
        field.unit = itemObj.value("unit").toString();

        //范围  可选
        if(itemObj.contains("ming")&&itemObj.value("min").isDouble())
            field.min = itemObj.value("min").toDouble();
        if(itemObj.contains("max")&&itemObj.value("max").isDouble())
            field.max = itemObj.value("max").toDouble();

        // 组件类型  默认NumericLabel
        field.widget = itemObj.value("widget").toString().trimmed();
        if(field.widget.isEmpty()){
            field.widget = QStringLiteral("NumbericLabel");
        }

        m_fields.append(field);
    }

    if(m_fields.isEmpty()){
        LOG_ERROR(QStringLiteral("未解析到任何字段：%1")
                  .arg(filePath));
        return false;
    }

    m_valid = true;
    LOG_INFO(QStringLiteral("成功加载配置项目：%1，仪表数：%2")
                 .arg((m_name.isEmpty()?QString("未命名"):m_name))
                 .arg(m_fields.size()));

    return true;

}

void ReadProjectConfig::clear()
{
    m_valid = false;
    m_name.clear();
    m_description.clear();
    m_fields.clear();
}
