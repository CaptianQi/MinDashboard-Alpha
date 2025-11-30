#ifndef ARCGAUGEWIDGET_H
#define ARCGAUGEWIDGET_H

#include <QWidget>
#include <QColor>

class ArcGaugeWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double angle READ angle WRITE setAngle)
public:
    explicit ArcGaugeWidget(QWidget *parent = nullptr);


    Q_INVOKABLE void setAngle(double value);
    void setRange(double minimum, double maximum);
    void setMinimum(double minimum);
    void setMaximum(double maximum);
    void setUnit(const QString &unit);
    double minimum() const { return m_minimum; }
    double maximum() const { return m_maximum; }
    Q_INVOKABLE double angle()const {return m_value;}

public slots:
    void setValue(double value);  //外部数据用这个槽来驱动

protected:
    void paintEvent(QPaintEvent *event) override;
private:
    void drawTicks(QPainter &p);
    void drawTickLabels(QPainter &p);
    void drawNeedle(QPainter &p);
    void drawCenterValueText(QPainter &p);
    void autoConfigureTicks(); //自动根据范围计算刻度
private:
    double m_minimum;
    double m_maximum;
    double m_value;

    double m_startDeg;   // 半圆起始角度（度）
    double m_endDeg;     // 半圆结束角度（度）

    int m_minorTicksPerSection =4;  //次刻度数量
    int m_majorTicksCount;  //主刻度数量
    QString m_unit; //单位

    QColor m_progressColor;
};

#endif // ARCGAUGEWIDGET_H
