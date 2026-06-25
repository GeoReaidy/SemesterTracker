#ifndef GPATRENDCHART_H
#define GPATRENDCHART_H

#include "gradeprojection.h"

#include <QWidget>

#include <vector>

class GpaTrendChart : public QWidget
{
public:
    explicit GpaTrendChart(QWidget *parent = nullptr);

    void setPoints(
        const std::vector<GpaTrendPoint> &points
    );

    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::vector<GpaTrendPoint> points;
};

#endif // GPATRENDCHART_H
