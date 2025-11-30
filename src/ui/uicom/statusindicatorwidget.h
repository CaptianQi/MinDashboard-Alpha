#ifndef STATUSINDICATORWIDGET_H
#define STATUSINDICATORWIDGET_H

#include <QObject>
#include <QWidget>
#include <QColor>
#include <QString>

/**
 * @brief 左侧圆形指示灯 + 右侧文字 的小组件
 *
 * 背景：半透明矩形
 * 灯：4 种状态（灭 / 绿 / 黄 / 红）
 * 文本：描述当前状态的文字
 */
class StatusIndicatorWidget : public QWidget
{
    Q_OBJECT
public:
    enum Status {
        Off = 0,   ///< 灯灭
        Ok,        ///< 正常（绿色）
        Warning,   ///< 警告（黄色）
        Error      ///< 故障（红色）
    };
    Q_ENUM(Status)

    explicit StatusIndicatorWidget(QWidget *parent = nullptr);

    Status status() const { return m_status; }
    QString text() const { return m_text; }

public slots:
    /// 设置状态（可以直接用信号连接到这个槽）
    void setStatus(Status s);

    /// 设置右侧说明文字
    void setText(const QString &text);

signals:
    void statusChanged(Status s);
    void textChanged(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override;

private:
    Status  m_status { Off };
    QString m_text   { QStringLiteral("未命名状态") };
};

#endif // STATUSINDICATORWIDGET_H
