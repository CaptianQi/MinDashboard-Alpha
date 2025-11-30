#include "realtimeareachartwidget.h"

#include <QPainter>
#include <QPaintEvent>
#include<QPainterPath>
#include <QtMath>

RealtimeAreaChartWidget::RealtimeAreaChartWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_OpaquePaintEvent, false);

    setMinimumSize(260, 160);

    // 初始化定时器
    m_timer = new QTimer(this);
    m_timer->setInterval(m_intervalMs);
    connect(m_timer, &QTimer::timeout,
            this, &RealtimeAreaChartWidget::onTimerTick);

    // 默认不开启，交给外部 setSampleIntervalMs 来决定
    // m_timer->start();
}

// ========== 外部接口 ==========

void RealtimeAreaChartWidget::setMaxPoints(int maxPoints)
{
    if (maxPoints < 2)
        maxPoints = 2;

    m_maxPoints = maxPoints;

    // 如果当前点数超过上限，裁掉最老的
    if (m_samples.size() > m_maxPoints) {
        int removeCount = m_samples.size() - m_maxPoints;
        m_samples.erase(m_samples.begin(),
                        m_samples.begin() + removeCount);
    }

    updateBounds();
    update();
}

void RealtimeAreaChartWidget::setSampleIntervalMs(int ms)
{
    m_intervalMs = ms;

    if (!m_timer)
        return;

    if (ms <= 0) {
        m_timer->stop();
        return;
    }

    m_timer->setInterval(m_intervalMs);
    if (!m_timer->isActive())
        m_timer->start();
}

int RealtimeAreaChartWidget::sampleIntervalMs() const
{
    if (!m_timer || !m_timer->isActive())
        return 0;
    return m_intervalMs;
}

void RealtimeAreaChartWidget::pushExternalValue(double y)
{
    // 外部每来一个数据，就更新“最新值”
    m_latestValue = y;
    m_hasLatest   = true;
}

void RealtimeAreaChartWidget::clear()
{
    m_samples.clear();
    m_minY = 0.0;
    m_maxY = 1.0;
    m_hasLatest   = false;
    m_hasNextTime = false;
    m_nextSampleTime = QDateTime();
    update();
}

void RealtimeAreaChartWidget::setLineColor(const QColor &c)
{
    m_lineColor = c;
    update();
}

void RealtimeAreaChartWidget::setFixedYRange(double minY, double maxY)
{
    if (qFuzzyCompare(minY, maxY)) {
        minY = 0.0;
        maxY = 1.0;
    }

    m_fixedY    = true;
    m_fixedMinY = qMin(minY, maxY);
    m_fixedMaxY = qMax(minY, maxY);

    m_minY = m_fixedMinY;
    m_maxY = m_fixedMaxY;

    update();
}

void RealtimeAreaChartWidget::clearFixedYRange()
{
    m_fixedY = false;
    updateBounds();
    update();
}

void RealtimeAreaChartWidget::setGridYCount(int count)
{
    if (count < 1)
        count = 1;
    m_gridYCount = count;
    update();
}

// ========== 内部逻辑 ==========

void RealtimeAreaChartWidget::updateBounds()
{
    if (m_fixedY) {
        m_minY = m_fixedMinY;
        m_maxY = m_fixedMaxY;
        return;
    }

    if (m_samples.isEmpty()) {
        m_minY = 0.0;
        m_maxY = 1.0;
        return;
    }

    m_minY = m_maxY = m_samples.first().y;
    for (const Sample &s : m_samples) {
        m_minY = qMin(m_minY, s.y);
        m_maxY = qMax(m_maxY, s.y);
    }

    double span = m_maxY - m_minY;
    if (qFuzzyIsNull(span)) {
        span = qMax(qAbs(m_maxY), 1.0);
        if (qFuzzyIsNull(span))
            span = 1.0;
    }

    m_minY -= span * 0.05;
    m_maxY += span * 0.05;
}

QPointF RealtimeAreaChartWidget::mapIndexToPixel(int index,
                                                 const QRectF &chartRect) const
{
    int n = m_samples.size();
    if (n <= 0)
        return chartRect.center();

    double xRatio;
    if (n == 1) {
        xRatio = 0.5;     // 只有一个点时放中间
    } else {
        xRatio = double(index) / double(n - 1);   // 0 ~ 1
    }

    double ySpan = m_maxY - m_minY;
    if (ySpan <= 0.0) ySpan = 1.0;

    double y = m_samples[index].y;
    double yRatio = (y - m_minY) / ySpan;   // 0 ~ 1

    double px = chartRect.left()  + xRatio * chartRect.width();
    double py = chartRect.bottom() - yRatio * chartRect.height();

    return QPointF(px, py);
}

// 定时器：每到一个节拍，就生成一个点
void RealtimeAreaChartWidget::onTimerTick()
{
    // 如果从来没收到外部值，就不画
    if (!m_hasLatest)
        return;

    // 1) 生成本次采样的时间戳（严格等间隔）
    QDateTime ts;

    if (!m_hasNextTime) {
        // 第一次：以当前时间为起点，把毫秒清零，方便看
        ts = QDateTime::currentDateTime();
        ts = ts.addMSecs(-ts.time().msec());
        m_nextSampleTime = ts;
        m_hasNextTime    = true;
    } else {
        // 后续：在上一次基础上 + intervalMs
        if (m_intervalMs <= 0)
            m_intervalMs = 1000;

        ts = m_nextSampleTime.addMSecs(m_intervalMs);
        m_nextSampleTime = ts;
    }

    // 2) 使用“这个时间 + 最新值”生成一个样本
    Sample s;
    s.ts = ts;
    s.y  = m_latestValue;   // 这一刻的最新值

    m_samples.append(s);

    // 3) 只保留最近 m_maxPoints 个
    if (m_samples.size() > m_maxPoints) {
        int removeCount = m_samples.size() - m_maxPoints;
        m_samples.erase(m_samples.begin(),
                        m_samples.begin() + removeCount);
    }

    // 4) 更新 Y 范围 & 重绘
    updateBounds();
    update();

    // ✅ 本次采样完成后清除“有新数据”标记
    //    如果下一次定时器触发前没有再 pushExternalValue()，
    //    下一个 onTimerTick() 会直接 return，不再往前画。
    m_hasLatest = false;
}

// ========== 绘制 ==========

void RealtimeAreaChartWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    QRectF fullRect = rect();

    const double leftMargin   = 40.0;
    const double rightMargin  = 16.0;
    const double topMargin    = 16.0;
    const double bottomMargin = 32.0;

    if (fullRect.width()  < leftMargin + rightMargin + 20 ||
        fullRect.height() < topMargin  + bottomMargin  + 20) {
        return;
    }

    QRectF chartRect = fullRect.adjusted(
        leftMargin,
        topMargin,
        -rightMargin,
        -bottomMargin
        );

    // -------- 1. Y 轴网格线 & 刻度 --------
    p.save();
    int gridCount = m_gridYCount;
    if (gridCount < 1) gridCount = 1;

    double ySpan = m_maxY - m_minY;
    if (ySpan <= 0.0) ySpan = 1.0;

    for (int i = 0; i <= gridCount; ++i) {
        double t = double(i) / gridCount;     // 0 ~ 1
        double value = m_minY + ySpan * t;
        double py = chartRect.bottom() - t * chartRect.height();

        // 网格线
        p.setPen(QPen(QColor(225, 228, 235), 1));
        p.drawLine(QPointF(chartRect.left(),  py),
                   QPointF(chartRect.right(), py));

        // 左边刻度文字
        QString label = QString::number(value, 'f', 0);
        QRectF textRect(
            fullRect.left(),
            py - 8,
            leftMargin - 6,
            16
            );
        p.setPen(QColor(150, 150, 160));
        p.drawText(textRect,
                   Qt::AlignRight | Qt::AlignVCenter,
                   label);
    }
    p.restore();

    int n = m_samples.size();
    if (n <= 0)
        return;

    // -------- 2. X 轴时间刻度（严格和样本数量一致） --------
    p.save();
    p.setPen(QColor(150, 150, 160));
    QFont xFont = p.font();
    xFont.setPointSize(8);
    p.setFont(xFont);

    const qreal labelWidth  = 70.0;
    const qreal labelHeight = bottomMargin - 4.0;

    for (int i = 0; i < n; ++i) {
        const Sample &s = m_samples[i];
        QString label = s.ts.toString("HH:mm:ss");

        double xRatio;
        if (n == 1) {
            xRatio = 0.5;
        } else {
            xRatio = double(i) / double(n - 1);
        }

        double px = chartRect.left() + xRatio * chartRect.width();

        QRectF textRect;
        if (i == 0) {
            // 第一个刻度：靠左，避免出左边界
            textRect = QRectF(
                chartRect.left(),
                chartRect.bottom() + 4,
                labelWidth,
                labelHeight
                );
            p.drawText(textRect,
                       Qt::AlignLeft | Qt::AlignTop,
                       label);
        } else if (i == n - 1) {
            // 最后一个刻度：靠右，避免溢出右侧
            textRect = QRectF(
                chartRect.right() - labelWidth,
                chartRect.bottom() + 4,
                labelWidth,
                labelHeight
                );
            p.drawText(textRect,
                       Qt::AlignRight | Qt::AlignTop,
                       label);
        } else {
            // 中间刻度：居中
            textRect = QRectF(
                px - labelWidth / 2.0,
                chartRect.bottom() + 4,
                labelWidth,
                labelHeight
                );
            p.drawText(textRect,
                       Qt::AlignHCenter | Qt::AlignTop,
                       label);
        }
    }
    p.restore();

    if (n < 2)
        return;

    // -------- 3. 构建折线 & 面积路径 --------
    QPainterPath linePath;
    bool first = true;
    for (int i = 0; i < n; ++i) {
        QPointF pt = mapIndexToPixel(i, chartRect);
        if (first) {
            linePath.moveTo(pt);
            first = false;
        } else {
            linePath.lineTo(pt);
        }
    }

    QPainterPath areaPath = linePath;
    {
        QPointF lastBottom = mapIndexToPixel(n - 1, chartRect);
        lastBottom.setY(chartRect.bottom());
        QPointF firstBottom = mapIndexToPixel(0, chartRect);
        firstBottom.setY(chartRect.bottom());

        areaPath.lineTo(lastBottom);
        areaPath.lineTo(firstBottom);
        areaPath.closeSubpath();
    }

    // -------- 4. 填充区域 --------
    p.save();
    QColor fill = m_lineColor;
    fill.setAlpha(50);
    p.setBrush(fill);
    p.setPen(Qt::NoPen);
    p.drawPath(areaPath);
    p.restore();

    // -------- 5. 折线 --------
    p.save();
    QPen linePen(m_lineColor, 2);
    linePen.setCapStyle(Qt::RoundCap);
    linePen.setJoinStyle(Qt::RoundJoin);
    p.setPen(linePen);
    p.setBrush(Qt::NoBrush);
    p.drawPath(linePath);
    p.restore();
}
