#include "gpatrendchart.h"

#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QPen>
#include <QSizePolicy>

#include <algorithm>

namespace
{
constexpr int LeftMargin = 48;
constexpr int RightMargin = 24;
constexpr int TopMargin = 38;
constexpr int BottomMargin = 52;

QPointF pointForValue(
    int index,
    int count,
    double value,
    const QRectF &plotArea)
{
    const double x =
        count <= 1
            ? plotArea.center().x()
            : plotArea.left() +
              static_cast<double>(index) /
                  static_cast<double>(count - 1) *
                  plotArea.width();

    const double normalized =
        std::clamp(value / 4.0, 0.0, 1.0);

    const double y =
        plotArea.bottom() -
        normalized * plotArea.height();

    return QPointF(x, y);
}

void drawMarker(
    QPainter &painter,
    const QPointF &point,
    const QColor &color,
    bool projected)
{
    painter.save();

    QPen pen(color);
    pen.setWidthF(2.0);
    painter.setPen(pen);

    painter.setBrush(
        projected
            ? QColor(Qt::white)
            : color
    );

    painter.drawEllipse(point, 4.5, 4.5);
    painter.restore();
}
}

GpaTrendChart::GpaTrendChart(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(220);
    setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );
}

void GpaTrendChart::setPoints(
    const std::vector<GpaTrendPoint> &newPoints)
{
    points = newPoints;
    update();
}

QSize GpaTrendChart::minimumSizeHint() const
{
    return QSize(520, 220);
}

void GpaTrendChart::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(
        QPainter::Antialiasing,
        true
    );

    painter.fillRect(
        rect(),
        QColor(QStringLiteral("#ffffff"))
    );

    const QRectF plotArea(
        LeftMargin,
        TopMargin,
        std::max(
            1,
            width() -
                LeftMargin -
                RightMargin
        ),
        std::max(
            1,
            height() -
                TopMargin -
                BottomMargin
        )
    );

    QFont labelFont = font();
    labelFont.setPointSize(9);
    painter.setFont(labelFont);

    QPen gridPen(
        QColor(QStringLiteral("#e2e8f0"))
    );
    gridPen.setWidthF(1.0);

    QPen axisPen(
        QColor(QStringLiteral("#94a3b8"))
    );
    axisPen.setWidthF(1.2);

    for (int value = 0; value <= 4; ++value)
    {
        const double y =
            plotArea.bottom() -
            static_cast<double>(value) /
                4.0 *
                plotArea.height();

        painter.setPen(gridPen);
        painter.drawLine(
            QPointF(plotArea.left(), y),
            QPointF(plotArea.right(), y)
        );

        painter.setPen(
            QColor(QStringLiteral("#64748b"))
        );

        painter.drawText(
            QRectF(
                4,
                y - 10,
                LeftMargin - 10,
                20
            ),
            Qt::AlignRight |
                Qt::AlignVCenter,
            QString::number(value)
        );
    }

    painter.setPen(axisPen);
    painter.drawLine(
        plotArea.bottomLeft(),
        plotArea.bottomRight()
    );

    if (points.empty())
    {
        painter.setPen(
            QColor(QStringLiteral("#94a3b8"))
        );

        QFont emptyFont = font();
        emptyFont.setPointSize(11);
        emptyFont.setWeight(QFont::DemiBold);
        painter.setFont(emptyFont);

        painter.drawText(
            plotArea,
            Qt::AlignCenter,
            tr("Complete or project courses to build your GPA trend.")
        );
        return;
    }

    const QColor semesterColor(
        QStringLiteral("#2563eb")
    );

    const QColor cumulativeColor(
        QStringLiteral("#16a34a")
    );

    auto drawSeries =
        [&](bool semesterSeries,
            const QColor &color)
        {
            QPointF previousPoint;
            bool hasPrevious = false;
            bool previousProjected = false;

            for (int index = 0;
                 index < static_cast<int>(
                     points.size()
                 );
                 ++index)
            {
                const GpaTrendPoint &data =
                    points[static_cast<std::size_t>(
                        index
                    )];

                const bool hasValue =
                    semesterSeries
                        ? data.hasSemesterGpa
                        : data.hasCumulativeCgpa;

                if (!hasValue)
                {
                    hasPrevious = false;
                    continue;
                }

                const double value =
                    semesterSeries
                        ? data.semesterGpa
                        : data.cumulativeCgpa;

                const QPointF currentPoint =
                    pointForValue(
                        index,
                        static_cast<int>(
                            points.size()
                        ),
                        value,
                        plotArea
                    );

                if (hasPrevious)
                {
                    QPen linePen(color);
                    linePen.setWidthF(2.2);

                    if (data.projected ||
                        previousProjected)
                    {
                        linePen.setStyle(
                            Qt::DashLine
                        );
                    }

                    painter.setPen(linePen);
                    painter.drawLine(
                        previousPoint,
                        currentPoint
                    );
                }

                drawMarker(
                    painter,
                    currentPoint,
                    color,
                    data.projected
                );

                previousPoint = currentPoint;
                hasPrevious = true;
                previousProjected =
                    data.projected;
            }
        };

    drawSeries(true, semesterColor);
    drawSeries(false, cumulativeColor);

    painter.setPen(
        QColor(QStringLiteral("#64748b"))
    );

    const QFontMetrics metrics(painter.font());
    const int count =
        static_cast<int>(points.size());

    for (int index = 0; index < count; ++index)
    {
        const QPointF anchor =
            pointForValue(
                index,
                count,
                0.0,
                plotArea
            );

        QString label =
            QString::fromStdString(
                points[static_cast<std::size_t>(
                    index
                )].label
            );

        if (label.size() > 16)
        {
            label =
                label.left(15) +
                QStringLiteral("…");
        }

        const int labelWidth =
            std::min(
                110,
                std::max(
                    54,
                    metrics.horizontalAdvance(
                        label
                    ) + 8
                )
            );

        QRectF textRect(
            anchor.x() -
                labelWidth / 2.0,
            plotArea.bottom() + 8,
            labelWidth,
            BottomMargin - 12
        );

        painter.drawText(
            textRect,
            Qt::AlignHCenter |
                Qt::AlignTop |
                Qt::TextWordWrap,
            label
        );
    }

    const int legendY = 10;

    painter.setPen(
        QPen(semesterColor, 2.4)
    );
    painter.drawLine(
        QPointF(LeftMargin, legendY + 7),
        QPointF(LeftMargin + 22, legendY + 7)
    );

    painter.setPen(
        QColor(QStringLiteral("#334155"))
    );
    painter.drawText(
        LeftMargin + 28,
        legendY,
        110,
        18,
        Qt::AlignVCenter,
        tr("Semester GPA")
    );

    const int cumulativeX =
        LeftMargin + 140;

    painter.setPen(
        QPen(cumulativeColor, 2.4)
    );
    painter.drawLine(
        QPointF(cumulativeX, legendY + 7),
        QPointF(cumulativeX + 22, legendY + 7)
    );

    painter.setPen(
        QColor(QStringLiteral("#334155"))
    );
    painter.drawText(
        cumulativeX + 28,
        legendY,
        120,
        18,
        Qt::AlignVCenter,
        tr("Cumulative CGPA")
    );

    QPen projectedPen(
        QColor(QStringLiteral("#64748b"))
    );
    projectedPen.setStyle(Qt::DashLine);
    projectedPen.setWidthF(2.0);

    const int projectedX =
        cumulativeX + 166;

    painter.setPen(projectedPen);
    painter.drawLine(
        QPointF(projectedX, legendY + 7),
        QPointF(projectedX + 22, legendY + 7)
    );

    painter.setPen(
        QColor(QStringLiteral("#64748b"))
    );
    painter.drawText(
        projectedX + 28,
        legendY,
        120,
        18,
        Qt::AlignVCenter,
        tr("Projected")
    );
}
