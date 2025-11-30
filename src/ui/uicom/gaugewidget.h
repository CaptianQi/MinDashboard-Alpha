#ifndef GAUGEWIDGET_H
#define GAUGEWIDGET_H

#include <QWidget>
#include <QColor>

class GaugeWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

public:
    explicit GaugeWidget(QWidget *parent = nullptr);

    // 范围
    void setRange(double minimum, double maximum);
    void setMinimum(double minimum);
    void setMaximum(double maximum);
    double minimum() const { return m_minimum; }
    double maximum() const { return m_maximum; }

    // 当前值（指针指向的数值）
    double value() const { return m_value; }

    // 背景色（表盘外面的底色，比如白色/淡灰）
    QColor backgroundColor() const { return m_backgroundColor; }
    void setBackgroundColor(const QColor &c);

public slots:
    // ★ 槽函数：外部信号可以直接 connect 到这里来驱动指针
    void setValue(double value);

signals:
    void valueChanged(double value);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
  //  void drawOuterBackground(QPainter &painter);
    void drawDialBackground(QPainter &painter);
    void drawScale(QPainter &painter);
    void drawNeedle(QPainter &painter);
    void drawCenterCap(QPainter &painter);

    double m_minimum;
    double m_maximum;
    double m_value;

    // 仪表角度范围（单位：度）
    // 比如 225° 到 -45°，一共 270°
    double m_startAngleDeg;
    double m_endAngleDeg;

    QColor m_backgroundColor;   // 外部底色：白色 / 淡灰
};

#endif // GAUGEWIDGET_H

