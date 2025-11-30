#ifndef REALTIMEAREACHARTWIDGET_H
#define REALTIMEAREACHARTWIDGET_H

#include <QWidget>
#include <QDateTime>
#include <QVector>
#include <QColor>
#include <QTimer>

/*!
 * \brief RealtimeAreaChartWidget
 *
 * 功能说明（这一版完全按你说的来）：
 *  1. 外部高频发数据：调用 pushExternalValue(double y)
 *     - 内部只保存“最新的这个值”
 *
 *  2. 内部用 QTimer 控制更新节奏：
 *     - setSampleIntervalMs(ms)：设置每隔多少毫秒采样一次
 *     - 每次定时器触发时：
 *          a) 生成一个严格等间隔的时间点（内部维护）
 *          b) 用“最新值 + 这个时间”追加到曲线中
 *
 *  3. 图上最多显示 maxPoints 个点：
 *     - setMaxPoints(n) 设置
 *     - 内部只保留最近 n 个点
 *     - X 轴只有 n 个时间刻度
 *     - 每个时间刻度 = 对应点的时间，垂直一一对应
 *
 *  4. Y 轴：
 *     - 默认根据当前这些点自动缩放
 *     - 可以用 setFixedYRange(minY, maxY) 固定范围
 */
class RealtimeAreaChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RealtimeAreaChartWidget(QWidget *parent = nullptr);

    // ========== 外部接口 ==========

    /*!
     * \brief 设置图上最多显示多少个点（X轴点数 / 曲线点数）
     * \param maxPoints 最少为 2
     *
     * 说明：
     *  - 图上永远只保留最近 maxPoints 个采样点
     *  - X 轴下面也只画 maxPoints 个时间刻度（当前点数不足时，就画当前这么多）
     */
    void setMaxPoints(int maxPoints);
    int  maxPoints() const { return m_maxPoints; }

    /*!
     * \brief 设置内部采样周期（毫秒）
     * \param ms 毫秒数，<=0 表示关闭内部定时采样
     *
     * 例子：
     *  - setSampleIntervalMs(1000);  // 1 秒采样一次最新值
     *  - setSampleIntervalMs(2000);  // 2 秒采样一次最新值
     */
    void setSampleIntervalMs(int ms);
    int  sampleIntervalMs() const;

    /*!
     * \brief pushExternalValue 外部高频数据入口
     * \param y 外部当前时刻的最新值
     *
     * 用法：
     *  - 后端每收到一帧（不管多快），都调用 pushExternalValue(value)
     *  - 控件不会马上画，而是只缓存到 m_latestValue
     *  - 等到内部定时器“到点”时，再用这个最新值追加一个点
     */
    void pushExternalValue(double y);

    /*!
     * \brief 清空当前所有点，重新开始
     */
    void clear();

    /*!
     * \brief 设置曲线主颜色
     */
    void setLineColor(const QColor &c);
    QColor lineColor() const { return m_lineColor; }

    /*!
     * \brief 固定 Y 轴范围
     */
    void setFixedYRange(double minY, double maxY);

    /*!
     * \brief 清除固定 Y 轴范围，恢复自动缩放
     */
    void clearFixedYRange();

    /*!
     * \brief 设置 Y 轴网格线数量（等分数）
     */
    void setGridYCount(int count);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    // 单个采样点（一个点 = 一个时间 + 一个值）
    struct Sample {
        QDateTime ts; // 时间
        double    y;  // 值
    };

    // 当前曲线上要画的点（最多 m_maxPoints 个）
    QVector<Sample> m_samples;

    int    m_maxPoints    = 10;              // 默认最多 10 个点
    QColor m_lineColor    = QColor("#1976D2");
    int    m_gridYCount   = 4;

    // Y 轴范围
    double m_minY         = 0.0;
    double m_maxY         = 1.0;
    bool   m_fixedY       = false;
    double m_fixedMinY    = 0.0;
    double m_fixedMaxY    = 100.0;

    // 内部定时器 & 采样周期
    QTimer *m_timer       = nullptr;
    int     m_intervalMs  = 1000;   // 默认 1 秒采样一次

    // 外部最新值缓存
    double  m_latestValue = 0.0;
    bool    m_hasLatest   = false;

    // 用于生成“严格等间隔”的时间轴
    QDateTime m_nextSampleTime;   // 下一次要使用的时间戳
    bool      m_hasNextTime = false;

    // 内部工具：更新 Y 范围
    void updateBounds();

    // 把样本索引映射到屏幕坐标
    QPointF mapIndexToPixel(int index, const QRectF &chartRect) const;

private slots:
    // 定时器回调：到点了，取最新值，生成一个 Sample 追加进去
    void onTimerTick();
};

#endif // REALTIMEAREACHARTWIDGET_H
