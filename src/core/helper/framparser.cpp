#include "framparser.h"
#include"../log/logger.h"
#include<QJsonDocument>
#include<QJsonObject>
#include<QString>
FramParser::FramParser(QObject *parent)
    : QObject{parent}
{}

void FramParser::parseFrame(const QByteArray &data)
{
    QByteArray frame = data.trimmed();
    if(frame.isEmpty()) return;

    QVariantMap map;

    //判断格式
    if(frame.startsWith('{')&&frame.endsWith('}')){
        //JSON格式
        map = parseJson(frame);
    }else if(frame.contains(':')){
        //key:value格式
        map = parseKeyValue(frame);
    }else if(frame.contains(',')){
        //CSV格式
        map = parseCsv(frame);
    }else{
        LOG_WARN(QStringLiteral("未识别格式: %1")
                 .arg(QString::fromUtf8(frame)));
        //Todo: 抛出异常
        return;
    }

    if(!map.isEmpty()){
        //解析成功    发信号
        emit frameParsed(map);
    }else{
        //解析失败
        LOG_WARN(QStringLiteral("解析失败：%1")
                 .arg(QString::fromUtf8(frame)));
        //Todo: 抛出异常
    }
}

QVariantMap FramParser::parseJson(const QByteArray &raw)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(raw,&err);

    if(err.error!=QJsonParseError::NoError){
        LOG_WARN(QStringLiteral("JSON解析错误：%1")
                 .arg(err.errorString()));
        return {};//返回空
    }
    if(!doc.isObject()){
        LOG_WARN(QStringLiteral("JSON不是对象：%1")
                 .arg(QString::fromUtf8(raw)));
        return {};
    }
    QVariantMap map;
    map = doc.object().toVariantMap();
    return map;
}

QVariantMap FramParser::parseKeyValue(const QByteArray &raw)
{
    QVariantMap map;
    QString text = QString::fromUtf8(raw);

    const QStringList pairs= text.split(',',Qt::SkipEmptyParts);
    for(const QString &pair: pairs){
        const QStringList kv = pair.split(':',Qt::SkipEmptyParts);
        if(kv.size()==2){
            QString key = kv[0].trimmed();
            QString value = kv[1].trimmed();
            map[key] = value.toDouble();  //值转换为数字
        }
    }
    return map;
}

QVariantMap FramParser::parseCsv(const QByteArray &raw)
{
    QVariantMap map;
    const QStringList parts = QString::fromUtf8(raw)
        .split(',',Qt::SkipEmptyParts);
    for(int i=0;i<parts.size();i++){
        //用数字最为键
        map[QString::number(i+1)]= parts[i].trimmed().toDouble();
    }

    return map;
}
