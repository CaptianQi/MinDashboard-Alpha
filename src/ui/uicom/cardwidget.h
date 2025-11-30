#ifndef CARDWIDGET_H
#define CARDWIDGET_H


#include <QWidget>
#include <QString>
/**
 * @brief 简单卡片容器
 *
 * - 白色背景，细边框，圆角
 * - 像普通 QWidget 一样，可以 setLayout() 放子控件
 * - 底部中间可以显示一行说明文字（可选）
 */
class CardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CardWidget(QWidget *parent = nullptr);

    // 底部说明文字（可选）。为空字符串时不显示。
    QString footerText() const { return m_footerText; }

public slots:
    void setFooterText(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override;

private:
    QString m_footerText;
};
#endif // CARDWIDGET_H
