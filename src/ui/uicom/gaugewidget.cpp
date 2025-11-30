#include "gaugewidget.h"
#include <QPainter>
#include <QtMath>

GaugeWidget::GaugeWidget(QWidget *parent)
    : QWidget(parent),
    m_minimum(0.0),
    m_maximum(120.0),               // 默认 0~120，更贴近你的截图
    m_value(0.0),
    m_startAngleDeg(225.0),
    m_endAngleDeg(-45.0),
    m_backgroundColor(242, 244, 248) // 很淡的灰白色底色
{
    // 让背景透明，这样只看到圆形仪表
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setMinimumSize(140, 140);
}

void GaugeWidget::setRange(double minimum, double maximum)
{
    if (minimum >= maximum)
        return;

    m_minimum = minimum;
    m_maximum = maximum;

    if (m_value < m_minimum)
        m_value = m_minimum;
    if (m_value > m_maximum)
        m_value = m_maximum;

    update();
}

void GaugeWidget::setMinimum(double minimum)
{
    setRange(minimum, m_maximum);
}

void GaugeWidget::setMaximum(double maximum)
{
    setRange(m_minimum, maximum);
}

void GaugeWidget::setBackgroundColor(const QColor &c)
{
    if (c == m_backgroundColor)
        return;
    m_backgroundColor = c;
    update();
}

void GaugeWidget::setValue(double value)
{
    // 槽函数：指针目标值
    double bounded = qBound(m_minimum, value, m_maximum);
    if (qFuzzyCompare(bounded, m_value))
        return;

    m_value = bounded;
    emit valueChanged(m_value);
    update();   // 刷新绘制
}

void GaugeWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::TextAntialiasing);

    int side = qMin(width(), height());

    // 先画整体淡色背景（工业面板底色）
    //drawOuterBackground(painter);

    // 建立以中心为原点、[-100,100] 的虚拟坐标系
    painter.translate(width() / 2.0, height() / 2.0);
    painter.scale(side / 200.0, side / 200.0);

    drawDialBackground(painter);
    drawScale(painter);
    drawNeedle(painter);
    drawCenterCap(painter);
}

// ---------- 画控件外面的底色 ----------
// void GaugeWidget::drawOuterBackground(QPainter &p)
// {
//     p.save();
//     p.fillRect(rect(), m_backgroundColor);
//     p.restore();
// }

// ---------- 画表盘本身（黑色圆盘 + bezel） ----------
void GaugeWidget::drawDialBackground(QPainter &p)
{
    p.save();

    // 外圈金属边（浅灰）
    QRadialGradient bezelGrad(0, 0, 100);
    bezelGrad.setColorAt(0.0, QColor(210, 210, 210));
    bezelGrad.setColorAt(1.0, QColor(120, 120, 120));

    p.setBrush(bezelGrad);
    p.setPen(QPen(QColor(90, 90, 90), 3));
    p.drawEllipse(QPointF(0, 0), 95, 95);

    // 内圈表盘（黑色略有渐变）
    QRadialGradient dialGrad(0, -20, 80);
    dialGrad.setColorAt(0.0, QColor(60, 60, 60));
    dialGrad.setColorAt(0.7, QColor(20, 20, 20));
    dialGrad.setColorAt(1.0, QColor(5, 5, 5));

    p.setBrush(dialGrad);
    p.setPen(Qt::NoPen);
    p.drawEllipse(QPointF(0, 0), 80, 80);

    // 顶部一点高光，让它有一点“玻璃感”
    QRadialGradient highlightGrad(0, -40, 60);
    highlightGrad.setColorAt(0.0, QColor(255, 255, 255, 60));
    highlightGrad.setColorAt(1.0, QColor(255, 255, 255, 0));

    p.setBrush(highlightGrad);
    p.drawEllipse(QPointF(0, 0), 70, 70);

    p.restore();
}

// ---------- 刻度和数字 ----------
void GaugeWidget::drawScale(QPainter &p)
{
    p.save();

    const int majorTickCount = 13;  // 默认 0~120，每 10 一格 => 13 个主刻度
    const int minorPerMajor = 5;    // 每大格之间再分 5 个小刻度
    const double span = m_endAngleDeg - m_startAngleDeg;
    const double totalMinor = (majorTickCount - 1) * minorPerMajor;

    // 小刻度线
    p.setPen(QPen(QColor(110, 110, 110), 1));
    for (int i = 0; i <= totalMinor; ++i) {
        double ratio = double(i) / totalMinor;      // 0~1
        double angle = m_startAngleDeg + span * ratio;

        p.save();
        p.rotate(angle);
        p.drawLine(QPointF(0, -70), QPointF(0, -66));
        p.restore();
    }

    // 主刻度 + 数字
    QFont font = p.font();
    font.setPointSize(7);
    font.setBold(false);
    p.setFont(font);

    for (int i = 0; i < majorTickCount; ++i) {
        double ratio = double(i) / (majorTickCount - 1);
        double angle = m_startAngleDeg + span * ratio;
        // 当前主刻度对应的数值
        double v = m_minimum + (m_maximum - m_minimum) * ratio;

        p.save();
        p.rotate(angle);

        // 主刻度线稍微长一点、亮一点
        p.setPen(QPen(QColor(220, 220, 220), 2));
        p.drawLine(QPointF(0, -72), QPointF(0, -64));

        // 数字放在主刻度内侧一点
        p.translate(0, -54);
        p.rotate(-angle);  // 文字水平
        p.setPen(QColor(230, 230, 230));
        QRectF textRect(-12, -8, 24, 16);
        p.drawText(textRect, Qt::AlignCenter,
                   QString::number(int(v)));
        p.restore();
    }

    p.restore();
}

// ---------- 红色指针 ----------
void GaugeWidget::drawNeedle(QPainter &p)
{
    p.save();

    // 当前值 → 0~1 比例 → 对应角度
    double ratio = (m_value - m_minimum) / (m_maximum - m_minimum);
    ratio = qBound(0.0, ratio, 1.0);
    double angle = m_startAngleDeg + (m_endAngleDeg - m_startAngleDeg) * ratio;

    p.rotate(angle);

    // 指针形状：细长三角形（工业仪表那种）
    QPolygonF needle;
    needle << QPointF(0,   6)   // 尾部
           << QPointF(-2.5, 0)
           << QPointF(0,  -62)  // 尖端
           << QPointF(2.5,  0);

    p.setBrush(QColor(220, 60, 60));
    p.setPen(QPen(QColor(120, 20, 20), 1));
    p.drawPolygon(needle);

    p.restore();
}

// ---------- 中心帽（小圆盖） ----------
void GaugeWidget::drawCenterCap(QPainter &p)
{
    p.save();
    QRadialGradient g(0, 0, 9);
    g.setColorAt(0.0, QColor(255, 255, 255));
    g.setColorAt(0.5, QColor(200, 200, 200));
    g.setColorAt(1.0, QColor(120, 120, 120));

    p.setBrush(g);
    p.setPen(QPen(QColor(40, 40, 40), 1));
    p.drawEllipse(QPointF(0, 0), 9, 9);
    p.restore();
}
