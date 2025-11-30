#include "arcgaugewidget.h"

#include <QPainter>
#include <QtMath>
#include <QPolygonF>
#include <QRadialGradient>
#include<QPropertyAnimation>
ArcGaugeWidget::ArcGaugeWidget(QWidget *parent)
    : QWidget(parent),
    m_minimum(0.0),
    m_maximum(100.0),
    m_value(0.0),
    m_startDeg(210.0),
    m_endDeg(-30.0),
    m_unit("rpm")
{
    setAttribute(Qt::WA_TranslucentBackground);//背景透明
    setAutoFillBackground(false);    //不自动填充背景
    setAttribute(Qt::WA_OpaquePaintEvent, false);//防止父类背景干扰
    setMinimumSize(180,140);
}

void ArcGaugeWidget::setAngle(double value)
{
    double bounded = qBound(m_minimum,value,m_maximum);  //限制值的范围
    if(qFuzzyCompare(bounded,m_value)) return;

    m_value = bounded;
    update();
}

void ArcGaugeWidget::setRange(double minimum, double maximum)
{
    if(minimum>=maximum) return;

    m_minimum =minimum;
    m_maximum = maximum;

    if(m_value<m_minimum) m_value = m_minimum;
    if(m_value>m_maximum) m_value = m_maximum;
    //计算刻度数
    autoConfigureTicks();

    update();  //重绘 触发paintEvent
}

void ArcGaugeWidget::setMinimum(double minimum)
{
    if(qFuzzyCompare(m_minimum,minimum)) return;

    m_minimum = minimum;
    update();
}

void ArcGaugeWidget::setMaximum(double maximum)
{
    if(qFuzzyCompare(m_maximum,maximum)) return;
    m_maximum = maximum;
    update();
}

void ArcGaugeWidget::setUnit(const QString &unit)
{
    if(m_unit!=unit){
        m_unit = unit;
        update();
    }
}

void ArcGaugeWidget::setValue(double value)
{
    if(qFuzzyCompare(m_value,value)) return;

    auto *ani = new QPropertyAnimation(this,"angle",this);
    ani->setDuration(250);
    ani->setStartValue(m_value);
    ani->setEndValue(value);
    ani->setEasingCurve(QEasingCurve::OutCubic);
    ani->start(QAbstractAnimation::DeleteWhenStopped);

}

void ArcGaugeWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing); //开启抗锯齿

  //  p.fillRect(rect(),QColor(245,246,248)); //背景色 浅灰


    int w = width();
    int h = height();
    int side= qMin(w,h);  //取短边，保持正方形
    p.translate(w/2.0,h/2.0);   //虚拟坐标原点  窗口中心
    p.scale(side/200.0,side/200.0);//建立虚拟坐标 200x200

    //1.定义半圆角度
    double startDeg = m_startDeg;
    double endDeg = m_endDeg;
    double spanDeg = endDeg -startDeg;  //负值->顺时针化

    //2.定义弧形大小和位置
    const int radius = 80;  //半径
    QRectF arcRect(-radius,-radius,radius*2,radius*2);

    //3.设置画笔： 浅灰 线宽、 圆角端点
    QPen basePen(QColor(232,236,241),18,Qt::SolidLine,Qt::RoundCap,Qt::MiterJoin);
    p.setPen(basePen);
    p.setBrush(Qt::NoBrush);

    //4. 画弧 灰色弧底
    int start16 = int(startDeg*16);
    int span16 = int(spanDeg*16);
    p.drawArc(arcRect,start16,span16);

    //画彩色进度
    //1.算出当前值在范围内的比例 0-1
    double ratio = 0.0;
    if(m_maximum>m_minimum)
        ratio = (m_value-m_minimum)/(m_maximum-m_minimum); //占比
    ratio = qBound(0.0,ratio,1.0); //限制
    //2.总角度*比例 = 实际需要画的角度
    double progressSpanDeg = spanDeg*ratio;
    int progressSpan16 = int(progressSpanDeg*16);

    QPen progPen(QColor(0,190,150),18,Qt::SolidLine,Qt::RoundCap,Qt::MiterJoin);
    p.setPen(progPen);
    p.drawArc(arcRect,start16,progressSpan16);

    drawTicks(p);
    drawTickLabels(p);
    drawNeedle(p);
    drawCenterValueText(p);

}

void ArcGaugeWidget::drawTicks(QPainter &p)
{
    p.save();

    const int majorCount = m_majorTicksCount;
    const int minorPerSection =  m_minorTicksPerSection;

    //至少有两个主刻度 才能形成区间
    if(majorCount<2){
        p.restore();
        return;
    }

    //参数
    const int sections = majorCount - 1;  //主区间数
    const int slotsPerSect = minorPerSection+1;  //次区间数
    const int totalSlots = sections*slotsPerSect;  //总区间数

    //视觉参数
    const double outerRadius = 60.0; //刻度半径
    const double majorTickLength = 5.0; //主刻度向内的长度
    const double minorTickLength = 2.0; //次刻度向内的长度

    //总弧度
    const double spanDeg = m_endDeg - m_startDeg;

    //设置画笔
    QPen pen(QColor(200,204,210)); //浅灰色
    pen.setWidthF(1.5);
    pen.setCapStyle(Qt::RoundCap); //圆头线
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);

    for(int k =0; k<=totalSlots; ++k){
        int seg = k/slotsPerSect; //0...sections-1
        int m = k%slotsPerSect; // 0...slotsPerSect

        //处理最后一个主刻度特殊情况
        if(k == totalSlots){
            seg=sections;
            m= 0;
        }

        bool isMajor = false;
        //判断是否是主刻度
        if(m==0 || m==slotsPerSect){
            //m = 0  区间起点 主刻度
            // m = slotsPerSect  区间终点 主刻度
            isMajor = true;
        }else{
            isMajor =false;
        }

        //归一化  0~1比例
        double t = (seg + double(m)/slotsPerSect)
                   /double(majorCount-1);
        //t 映射到角度
        double angleDeg = m_startDeg+spanDeg*t;
        double rad = qDegreesToRadians(angleDeg); // 度 -> 弧度
        double cosA  = qCos(rad);
        double sinA = qSin(rad);
        //当前刻度线的长度
        double len = isMajor ? majorTickLength : minorTickLength;
        //计算刻度线的两个端点（外端->内端）
        double x1 = outerRadius*cosA;
        double y1 = -outerRadius*sinA;

        double x2 = (outerRadius-len)*cosA;
        double y2 = -(outerRadius-len)*sinA;

        p.drawLine(QPointF(x1,y1),QPoint(x2,y2));
    }
    p.restore();

}

void ArcGaugeWidget::drawTickLabels(QPainter &p)
{
    p.save();

    const int majorCount = m_majorTicksCount;
    if (majorCount < 2)
    {
        p.restore();
        return;
    }

    const double spanDeg = m_endDeg - m_startDeg;
    const double spanVal = m_maximum - m_minimum;

    // 数字所在的半径：在刻度线内侧一点.
    const double labelRadius = 45.0;   // 比外圈小一截

    // 设置字体、颜色
    QFont font = p.font();
    font.setPointSize(5);
    p.setFont(font);
    p.setPen(QColor(120, 125, 135));   // 比刻度稍深一点的灰色

    QFontMetrics fm(font);

    // 只遍历主刻度：i = 0..majorCount-1
    for (int i = 0; i < majorCount; ++i)
    {
        // 0~1 的线性比例，和画主刻度时一样
        double t = (majorCount == 1)
                       ? 0.0
                       : double(i) / (majorCount - 1);

        // 角度（度）
        double angleDeg = m_startDeg + spanDeg * t;
        double rad      = qDegreesToRadians(angleDeg);
        double cosA     = qCos(rad);
        double sinA     = qSin(rad);

        // 对应的数值
        double value = m_minimum + spanVal * t;

        // 这里先按整数显示，要 1 位小数就改成 'f',1
        QString text = QString::number(value, 'f', 0);

        // 数字的中心点坐标（注意 y 要取反，和刻度一致）
        double x = labelRadius * cosA;
        double y = -labelRadius * sinA;

        // 让文本以 (x,y) 为中心对齐
        QSize  size  = fm.size(Qt::TextSingleLine, text);
        QRectF rect(x - size.width()  / 2.0,
                    y - size.height() / 2.0,
                    size.width(),
                    size.height());

        p.drawText(rect, Qt::AlignCenter, text);
    }

    p.restore();
}

void ArcGaugeWidget::drawNeedle(QPainter &p)
{
    p.save();

    // ================== 1. 计算当前值对应的角度 ==================
    //
    // ratio: 当前值在区间 [m_minimum, m_maximum] 内的线性比例（0~1）
    //        比如范围 0~100，值是 25，则 ratio = 0.25
    double spanVal = m_maximum - m_minimum;
    if (spanVal <= 0.000001) {
        p.restore();
        return;                 // 范围非法，直接不画指针
    }

    double ratio = (m_value - m_minimum) / spanVal;
    ratio = qBound(0.0, ratio, 1.0);        // 限制到 [0,1]

    // angleDeg: 对应的角度（度）
    // 和外圈弧线、刻度保持完全一致的映射：
    //   ratio = 0  -> m_startDeg
    //   ratio = 1  -> m_endDeg
    double spanDeg  = m_endDeg - m_startDeg;
    double angleDeg = m_startDeg + spanDeg * ratio;
    double rad      = qDegreesToRadians(angleDeg);

    // ================== 2. 建立指针用的单位向量 ==================
    //
    // 这里使用和刻度一样的坐标体系：
    //   x = cos(θ)
    //   y = -sin(θ)
    //
    // 这样可以保证指针、刻度、外弧都在同一个“方向系统”里，
    // 不会出现指针方向反了的问题。
    double cosA = qCos(rad);
    double sinA = qSin(rad);

    // 指针朝向的单位向量（沿弧线切线方向）
    double ux = cosA;
    double uy = -sinA;

    // 垂直于指针方向的单位向量（用来控制指针宽度）
    // v = 旋转 u 90° 得到： (vx, vy)
    double vx = -uy;            // = sinA
    double vy =  ux;            // = cosA


    // ================== 3. 一些“样式参数”（以后想改样式就调这里） ==================
    //
    // 这些参数你可以根据视觉效果自己微调：
    //
    // outerRadius    : 指针尖端距离中心的半径（越大指针越长）
    // innerRadius    : 指针尾部中心距离（要比 capRadius 小一点，这样会被中心圆盖住）
    // halfWidth      : 指针半宽（总宽度 ≈ 2 * halfWidth）
    // capRadius      : 中心圆半径（越大越像一个纽扣）
    // needleColor    : 指针颜色
    // capColorCenter : 纽扣中心颜色
    // capColorEdge   : 纽扣边缘颜色

    const double outerRadius = 67.0;                // 指针长度（对应外圈刻度内侧）
    const double innerRadius = 4.0;                 // 指针从接近中心开始（小于 capRadius）
    const double halfWidth   = 4.0;                 // 半宽 → 指针总宽度大约 8 像素

    const double capRadius      = 8.0;              // 中心圆“纽扣”的半径
    const QColor needleColor    = QColor(70, 72, 76);    // 深灰色指针
    const QColor capColorCenter = QColor(255, 255, 255); // 纽扣中心偏亮
    const QColor capColorEdge   = QColor(210, 210, 210); // 纽扣边缘偏暗（有立体感）


    // ================== 4. 计算指针三角形的三个点 ==================
    //
    // 结构：
    //   tip        : 尖端（靠外）
    //   tailCenter : 尾部中心点（靠里，位于 innerRadius 处，会被中心圆盖住一部分）
    //   tailLeft / tailRight : 尾部左右两侧，形成一个细长三角形
    //
    // 注意：我们使用 (ux,uy) 和 (vx,vy) 来构造三角形，
    //      保证不论角度如何变化，指针总是“贴着弧线方向”旋转。
    QPointF tip(ux * outerRadius,  uy * outerRadius);

    QPointF tailCenter(ux * innerRadius,
                       uy * innerRadius);

    QPointF tailLeft(
        tailCenter.x() + vx * halfWidth,
        tailCenter.y() + vy * halfWidth
        );
    QPointF tailRight(
        tailCenter.x() - vx * halfWidth,
        tailCenter.y() - vy * halfWidth
        );

    QPolygonF needlePoly;
    needlePoly << tailLeft << tip << tailRight;

    // 画指针主体
    p.setPen(Qt::NoPen);
    p.setBrush(needleColor);
    p.drawPolygon(needlePoly);

    p.restore();   // 恢复到进入函数时的状态（但原点仍在表盘中心）


    // ================== 5. 画中心圆“纽扣”，盖在指针根部上 ==================
    //
    // 这里专门再画一个小圆，盖住指针尾部，让视觉上看起来：
    //   指针是“压在纽扣下面”的，整体在转动。
    // 使用径向渐变做一点轻微立体效果。
    p.save();

    QRadialGradient grad(0, 0, capRadius);
    grad.setColorAt(0.0, capColorCenter); // 中心亮
    grad.setColorAt(1.0, capColorEdge);   // 边缘稍暗

    p.setPen(Qt::NoPen);
    p.setBrush(grad);
    p.drawEllipse(QPointF(0, 0), capRadius, capRadius);

    p.restore();
}

void ArcGaugeWidget::drawCenterValueText(QPainter &p)
{
    p.save();

    // ========= 1. 当前值 & 单位字符串 =========
    //
    // valueStr : 实际显示的数值字符串（比如 "23"）
    // unitStr  : 单位字符串（比如 "°C"、"rpm"）
    //
    // 小数位数你可以按需要改：'f',0 → 整数；'f',1 → 一位小数
    QString valueStr = QString::number(m_value, 'f', 0);
    QString unitStr  = m_unit;


    // ========= 2. 样式参数（你可以自由调整） =========
    //
    // baseLineY       : 数值/单位所在的纵向位置（0 是中心，向下为正）
    // valueFontSize   : 数字的字号
    // unitFontSize    : 单位的字号
    // spacing         : 数字框与单位之间的固定间距
    //
    const double baseLineY       = 55.0;    // 数值整体在底部的位置
    const int    valueFontSize   = 13;      // 主数字字号
    const int    unitFontSize    = 7;      // 单位字号
    const double spacing         = 6.0;     // 数字框与单位之间的固定间距

    const QColor valueColor      = QColor(40, 40, 40);        // 主数字颜色：深灰
    const QColor unitColor       = QColor(130, 135, 145);     // 单位颜色：浅灰


    // ========= 3. 准备字体 & 测量宽度 =========
    QFont valueFont = p.font();
    valueFont.setPointSize(valueFontSize);
    valueFont.setBold(true);                 // 数字加粗一点更显眼

    QFont unitFont = p.font();
    unitFont.setPointSize(unitFontSize);
    unitFont.setBold(false);

    QFontMetrics valueFm(valueFont);
    QFontMetrics unitFm(unitFont);

    // ---- 3.1 为“数字框”预留一个固定宽度 ----
    //
    // 我们用“最大可能值”来估算数字宽度，比如范围 [0, 999] 就按 999 来预留。
    // 这样无论当前值是 1、23 还是 456，都在同一个宽度里右对齐，
    // 单位的 x 坐标就不会跟着跳。
    //
    double maxAbs = qMax(qAbs(m_minimum), qAbs(m_maximum));
    QString templateStr = QString::number(maxAbs, 'f', 0);

    // 如果有可能出现负数，预留一个 '-' 的宽度
    if (m_minimum < 0.0) {
        templateStr.prepend('-');
    }

    int    wValueField = valueFm.horizontalAdvance(templateStr);  // 数字框固定宽度
    int    wUnit       = unitFm.horizontalAdvance(unitStr);       // 单位宽度

    // 整体宽度 = 数字框 + 间距 + 单位（这一整块始终居中）
    double totalWidth  = wValueField + spacing + wUnit;


    // ========= 4. 计算“数字框”的矩形位置（固定宽度 + 居中） =========
    //
    // 数字框的左边界：-totalWidth/2
    // 数字框宽度 ： wValueField（固定）
    //
    double valueFieldX = -totalWidth / 2.0;
    double valueFieldY = baseLineY - valueFm.ascent();

    QRectF valueFieldRect(valueFieldX,
                          valueFieldY,
                          wValueField,
                          valueFm.height());


    // ========= 5. 在数字框中“右对齐”绘制当前值 =========
    //
    // 注意：这里用 AlignRight，这样数字越多只是往左扩展，
    // 数字框右边界保持不动 → 单位的位置始终不变。
    p.setFont(valueFont);
    p.setPen(valueColor);
    p.drawText(valueFieldRect,
               Qt::AlignRight | Qt::AlignVCenter,
               valueStr);


    // ========= 6. 在数字框右侧固定位置绘制单位 =========
    //
    // 单位左边界 = 数字框右边界 + spacing
    // 这样，只要数字框宽度固定，单位的位置就是固定的。
    double unitX = valueFieldX + wValueField + spacing;
    double unitY = baseLineY - unitFm.ascent();

    QRectF unitRect(unitX,
                    unitY,
                    wUnit,
                    unitFm.height());

    p.setFont(unitFont);
    p.setPen(unitColor);
    p.drawText(unitRect,
               Qt::AlignLeft | Qt::AlignVCenter,
               unitStr);

    p.restore();
}

void ArcGaugeWidget::autoConfigureTicks()
{
    double span = m_maximum - m_minimum;
    //范围非法或太小，退化成两端一个主刻度 0和max,每段4个小刻度
    if(span<=0.0){
        m_majorTicksCount =2;
        m_minorTicksPerSection =4;
        return;
    }
    //1.计算一个合适的主刻度间距
    //整个范围大概被分成5段左右；
    double roughStep = span/5.0;
    // 2.应用 1-2-5规则，选一个好看的刻度间距
    double log10v = qLn(roughStep)/qLn(10.0);  //log10(roughStep)
    double k  = qFloor(log10v);  //幂次
    double pow10 = qPow(10.0,k);   //10^k
    double norm = roughStep / pow10;   //归一化

    double niceNorm;
    if(norm<=1.0)   niceNorm = 1.0;
    else if(norm<=2.0)  niceNorm=2.0;
    else if(norm<=5.0)  niceNorm=5.0;
    else    niceNorm =10.0;

    double majorStep = niceNorm*pow10;  //最终主刻度间距
    //反推一共有多少个主刻度间隔
    int intervals = int(qCeil(span/majorStep));
    if(intervals<4) intervals =4;
    if(intervals>8) intervals = 8;

    //主刻度数量 = 间距+1
    m_majorTicksCount = intervals +1;

    //次刻度数
    m_minorTicksPerSection =4;
}
