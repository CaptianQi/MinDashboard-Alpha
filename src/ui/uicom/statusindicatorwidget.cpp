#include "statusindicatorwidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QFontMetrics>

StatusIndicatorWidget::StatusIndicatorWidget(QWidget *parent)
    : QWidget(parent)
{
    // 背景透明
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);

}

void StatusIndicatorWidget::setStatus(StatusIndicatorWidget::Status s)
{
    if (m_status == s)
        return;
    m_status = s;
    emit statusChanged(s);
    update();   // 重新绘制
}

void StatusIndicatorWidget::setText(const QString &text)
{
    if (m_text == text)
        return;
    m_text = text;
    emit textChanged(m_text);
    update();
}

QSize StatusIndicatorWidget::sizeHint() const
{
    // 根据文字长度估算一个合适的大小
    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(m_text.isEmpty() ? "状态" : m_text);

    int w = 16   // 左边 padding
            + 16 // 灯的直径
            + 8  // 灯和文字之间的间距
            + textWidth
            + 16; // 右边 padding

    int h = qMax(28, fm.height() + 12); // 给文字上下留点空间

    return QSize(w, h);
}

void StatusIndicatorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    const QRectF r = rect();

    // ================= 背景半透明矩形 =================
    p.save();
    // 比较深一点的半透明底（可以按主界面颜色调整）
    QColor bg(180, 200, 255, 120); // (R,G,B,Alpha)
    p.setPen(Qt::NoPen);
    p.setBrush(bg);
    // 圆角矩形更好看一点
    p.drawRoundedRect(r.adjusted(0.5, 0.5, -0.5, -0.5), 6, 6);
    p.restore();

    // ================= 计算布局：灯 + 文字 =================
    const int leftPadding  = 14;
    const int rightPadding = 14;
    const int lightDiameter = 14;    // 灯的直径
    const int spacing       = 8;     // 灯与文字之间间距

    QPointF centerLight(leftPadding + lightDiameter / 2.0,
                        r.center().y());

    QRectF textRect(
        centerLight.x() + lightDiameter / 2.0 + spacing,
        r.top() + 4,
        r.width() - (centerLight.x() + lightDiameter / 2.0 + spacing) - rightPadding,
        r.height() - 8
        );

    // ================= 绘制指示灯 =================
    p.save();

    // 根据状态选择颜色
    QColor mainColor;   // 灯主体颜色
    QColor glowColor;   // 外圈光晕
    QColor borderColor; // 外边框

    switch (m_status) {
    case Off:
        mainColor   = QColor(80, 85, 95);    // 暗灰
        glowColor   = QColor(0, 0, 0, 0);    // 基本无光晕
        borderColor = QColor(40, 45, 55);    // 深一点的边框
        break;
    case Ok:
        mainColor   = QColor(46, 204, 113);  // 绿色
        glowColor   = QColor(46, 204, 113, 100);
        borderColor = QColor(20, 120, 70);
        break;
    case Warning:
        mainColor   = QColor(241, 196, 15);  // 黄色
        glowColor   = QColor(241, 196, 15, 90);
        borderColor = QColor(160, 120, 10);
        break;
    case Error:
        mainColor   = QColor(231, 76, 60);   // 红色
        glowColor   = QColor(231, 76, 60, 100);
        borderColor = QColor(150, 30, 30);
        break;
    }

    // 1) 外圈光晕（稍大一点的圆，半透明）
    if (glowColor.alpha() > 0) {
        p.setPen(Qt::NoPen);
        p.setBrush(glowColor);
        double glowRadius = lightDiameter / 2.0 + 3.0;
        p.drawEllipse(centerLight, glowRadius, glowRadius);
    }

    // 2) 内部主体（用径向渐变做一点立体感）
    {
        QRadialGradient g(centerLight,
                          lightDiameter / 2.0);
        // 中心更亮，边缘稍深
        QColor centerCol = mainColor.lighter(130);
        QColor edgeCol   = mainColor.darker(130);
        g.setColorAt(0.0, centerCol);
        g.setColorAt(0.7, mainColor);
        g.setColorAt(1.0, edgeCol);

        p.setBrush(g);
        p.setPen(QPen(borderColor, 1.0));
        double radius = lightDiameter / 2.0;
        p.drawEllipse(centerLight, radius, radius);
    }

    p.restore();

    // ================= 绘制文字 =================
    p.save();

    // 文字颜色：根据状态略微变化
    QColor textColor(0, 25, 25);
    // if (m_status == Off) {
    //     textColor = QColor(160, 165, 180);
    // }

    p.setPen(textColor);
    QFont f = p.font();
    f.setPointSize(10);
    f.setBold(true);
    p.setFont(f);

    p.drawText(textRect,
               Qt::AlignVCenter | Qt::AlignLeft,
               m_text);

    p.restore();
}
