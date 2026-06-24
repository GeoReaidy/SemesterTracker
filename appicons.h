#ifndef APPICONS_H
#define APPICONS_H

#include <QColor>
#include <QIcon>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPixmap>
#include <QRectF>

#include <cmath>
#include <functional>

class AppIcons final
{
public:
    enum class SidebarIcon
    {
        Dashboard,
        Semesters,
        Courses,
        Assignments,
        Calendar,
        Grades,
        Settings,
        Logout
    };

    static QIcon sidebar(SidebarIcon type)
    {
        if (type == SidebarIcon::Logout)
        {
            return createStatefulIcon(
                type,
                QColor(QStringLiteral("#dc2626")),
                QColor(QStringLiteral("#b91c1c"))
            );
        }

        return createStatefulIcon(
            type,
            QColor(QStringLiteral("#334155")),
            QColor(QStringLiteral("#2563eb"))
        );
    }

private:
    AppIcons() = delete;

    static QIcon createStatefulIcon(
        SidebarIcon type,
        const QColor &normalColor,
        const QColor &activeColor)
    {
        QIcon icon;

        const QPixmap normal = drawIcon(type, normalColor);
        const QPixmap active = drawIcon(type, activeColor);
        const QPixmap disabled = drawIcon(
            type,
            QColor(QStringLiteral("#94a3b8"))
        );

        icon.addPixmap(normal, QIcon::Normal, QIcon::Off);
        icon.addPixmap(active, QIcon::Active, QIcon::Off);
        icon.addPixmap(active, QIcon::Normal, QIcon::On);
        icon.addPixmap(active, QIcon::Active, QIcon::On);
        icon.addPixmap(disabled, QIcon::Disabled, QIcon::Off);
        icon.addPixmap(disabled, QIcon::Disabled, QIcon::On);

        return icon;
    }

    static QPixmap drawIcon(
        SidebarIcon type,
        const QColor &color)
    {
        QPixmap pixmap(24, 24);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QPen pen(color);
        pen.setWidthF(2.0);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);

        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        switch (type)
        {
        case SidebarIcon::Dashboard:
            drawDashboard(painter);
            break;
        case SidebarIcon::Semesters:
            drawSemesters(painter);
            break;
        case SidebarIcon::Courses:
            drawCourses(painter);
            break;
        case SidebarIcon::Assignments:
            drawAssignments(painter);
            break;
        case SidebarIcon::Calendar:
            drawCalendar(painter);
            break;
        case SidebarIcon::Grades:
            drawGrades(painter);
            break;
        case SidebarIcon::Settings:
            drawSettings(painter);
            break;
        case SidebarIcon::Logout:
            drawLogout(painter);
            break;
        }

        return pixmap;
    }

    static void drawDashboard(QPainter &painter)
    {
        painter.drawRoundedRect(QRectF(4, 4, 6, 6), 1.2, 1.2);
        painter.drawRoundedRect(QRectF(14, 4, 6, 6), 1.2, 1.2);
        painter.drawRoundedRect(QRectF(4, 14, 6, 6), 1.2, 1.2);
        painter.drawRoundedRect(QRectF(14, 14, 6, 6), 1.2, 1.2);
    }

    static void drawSemesters(QPainter &painter)
    {
        painter.drawRoundedRect(QRectF(3.5, 5, 17, 15.5), 2.2, 2.2);
        painter.drawLine(QPointF(3.5, 9), QPointF(20.5, 9));
        painter.drawLine(QPointF(8, 3.5), QPointF(8, 6.5));
        painter.drawLine(QPointF(16, 3.5), QPointF(16, 6.5));

        painter.drawPoint(QPointF(8, 13));
        painter.drawPoint(QPointF(12, 13));
        painter.drawPoint(QPointF(16, 13));
        painter.drawPoint(QPointF(8, 17));
        painter.drawPoint(QPointF(12, 17));
    }

    static void drawCourses(QPainter &painter)
    {
        QPainterPath leftPage;
        leftPage.moveTo(4, 5);
        leftPage.quadTo(8.5, 4.5, 12, 7);
        leftPage.lineTo(12, 20);
        leftPage.quadTo(8.5, 17.5, 4, 18);
        leftPage.closeSubpath();

        QPainterPath rightPage;
        rightPage.moveTo(20, 5);
        rightPage.quadTo(15.5, 4.5, 12, 7);
        rightPage.lineTo(12, 20);
        rightPage.quadTo(15.5, 17.5, 20, 18);
        rightPage.closeSubpath();

        painter.drawPath(leftPage);
        painter.drawPath(rightPage);
        painter.drawLine(QPointF(12, 7), QPointF(12, 20));
    }

    static void drawAssignments(QPainter &painter)
    {
        painter.drawRoundedRect(QRectF(5, 4.5, 14, 16), 2, 2);
        painter.drawRoundedRect(QRectF(9, 2.8, 6, 4), 1.3, 1.3);

        QPainterPath check;
        check.moveTo(8, 13);
        check.lineTo(11, 16);
        check.lineTo(16.5, 10);
        painter.drawPath(check);
    }

    static void drawCalendar(QPainter &painter)
    {
        painter.drawRoundedRect(QRectF(3.5, 5, 17, 15.5), 2.2, 2.2);
        painter.drawLine(QPointF(3.5, 9), QPointF(20.5, 9));
        painter.drawLine(QPointF(8, 3.5), QPointF(8, 6.5));
        painter.drawLine(QPointF(16, 3.5), QPointF(16, 6.5));

        painter.drawLine(QPointF(8, 12), QPointF(8, 17.5));
        painter.drawLine(QPointF(12, 12), QPointF(12, 17.5));
        painter.drawLine(QPointF(16, 12), QPointF(16, 17.5));
        painter.drawLine(QPointF(6.5, 14.7), QPointF(17.5, 14.7));
    }

    static void drawGrades(QPainter &painter)
    {
        painter.drawLine(QPointF(4, 20), QPointF(20, 20));
        painter.drawRoundedRect(QRectF(5, 13, 3.5, 7), 0.8, 0.8);
        painter.drawRoundedRect(QRectF(10.25, 9, 3.5, 11), 0.8, 0.8);
        painter.drawRoundedRect(QRectF(15.5, 5, 3.5, 15), 0.8, 0.8);
    }

    static void drawSettings(QPainter &painter)
    {
        painter.drawEllipse(QRectF(8.5, 8.5, 7, 7));

        constexpr double center = 12.0;
        constexpr double innerRadius = 5.5;
        constexpr double outerRadius = 9.0;

        for (int index = 0; index < 8; ++index)
        {
            const double angle =
                (3.14159265358979323846 / 4.0) * index;

            const QPointF inner(
                center + innerRadius * std::cos(angle),
                center + innerRadius * std::sin(angle)
            );

            const QPointF outer(
                center + outerRadius * std::cos(angle),
                center + outerRadius * std::sin(angle)
            );

            painter.drawLine(inner, outer);
        }
    }

    static void drawLogout(QPainter &painter)
    {
        painter.drawRoundedRect(QRectF(4, 4, 9, 16), 1.8, 1.8);
        painter.drawLine(QPointF(10, 12), QPointF(21, 12));
        painter.drawLine(QPointF(17, 8), QPointF(21, 12));
        painter.drawLine(QPointF(17, 16), QPointF(21, 12));
    }
};

#endif // APPICONS_H
