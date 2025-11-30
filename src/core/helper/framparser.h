#ifndef FRAMPARSER_H
#define FRAMPARSER_H

#include <QObject>
#include<QVariantMap>
#include<QByteArray>

/**
 * @brief The FramParser class
 * 数据解析   将原数据转换为 键值对
 *  支持解析 JSON、Key-Value、CSV
 *  解析成功发送信号  解析为QVariantMap类型
 */
class FramParser : public QObject
{
    Q_OBJECT
public:
    explicit FramParser(QObject *parent = nullptr);
public slots:
    //传入数据
    void parseFrame(const QByteArray &data);

signals:
    //传出数据
    void frameParsed(const QVariantMap &frame);
private:
    //解析
    QVariantMap parseJson(const QByteArray &raw);
    QVariantMap parseKeyValue(const QByteArray &raw);
    QVariantMap parseCsv(const QByteArray &raw);
};

#endif // FRAMPARSER_H
