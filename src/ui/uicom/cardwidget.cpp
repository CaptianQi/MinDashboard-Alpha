#include "cardwidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>

CardWidget::CardWidget(QWidget *parent)
    : QWidget(parent)
{
    // 取消背景
    setAttribute(Qt::WA_StyledBackground, false);
    setAutoFillBackground(false);

    setMinimumSize(120, 80);
}

void CardWidget::setFooterText(const QString &text)
{
    if (m_footerText == text)
        return;

    m_footerText = text;
    update();
}

QSize CardWidget::sizeHint() const
{
    // 默认给一个适中的大小，实际会被布局管理器调整
    return QSize(220, 140);
}

void CardWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    QRectF r = rect();

    // ========= 1. 画基础白色卡片 =========
    p.save();

    const qreal radius = 8.0;

    // 白色背景
    QColor bgColor(255, 255, 255);
    // 边框浅灰色
    QColor borderColor(220, 224, 230);

    p.setBrush(bgColor);
    p.setPen(QPen(borderColor, 1.0));

    // 稍微往内缩半个像素，避免抗锯齿造成的模糊
    QRectF cardRect = r.adjusted(0.5, 0.5, -0.5, -0.5);
    p.drawRoundedRect(cardRect, radius, radius);

    p.restore();

    // ========= 2. 底部说明文字（可选） =========
    if (!m_footerText.isEmpty()) {
        p.save();

        // 底部文字颜色 & 字体
        QColor textColor(140, 145, 155);
        p.setPen(textColor);
        QFont f = p.font();
        f.setPointSize(9);
        p.setFont(f);

        // 文字的矩形区域：
        //  - 水平居中
        //  - 垂直位置：略高于底边一点（比如底边往上 8 像素）
        const int bottomMargin = 4;   // 距离底部的偏移
        const int textHeight   = 18;  // 文字区域高度

        QRectF textRect(
            cardRect.left() + 12,
            cardRect.bottom() - bottomMargin - textHeight,
            cardRect.width() - 24,
            textHeight
            );

        p.drawText(textRect,
                   Qt::AlignHCenter | Qt::AlignVCenter,
                   m_footerText);

        p.restore();
    }
}
