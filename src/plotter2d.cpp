#include "plotter2d.h"
#include "fvformathandler.h"

#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QStylePainter>
#include <QtGui/QMouseEvent>
#include <QStyleOptionFocusRect>

#include <cmath>
#include <QDebug>

Plotter2d::Plotter2d(QVector<Function2d *> &functionsList, QWidget *parent)
        : QWidget(parent), functionsList(functionsList)
{
    QPalette p(palette());
    p.setColor(QPalette::Background, plotSettings.bgColor);
    setPalette(p);
    setBackgroundRole(QPalette::Background);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    rubberBandIsShown = false;
    showRubberBand = false;

//    setPlotSettings(PlotSettings());
}

void Plotter2d::setPlotSettings(const PlotSettings2d &settings)
{
    plotSettings = settings;

    QPalette p(palette());
    p.setColor(QPalette::Background, plotSettings.bgColor);
    setPalette(p);

    refreshPixmap();
}

void Plotter2d::zoomOut()
{
    double mx = fabs(plotSettings.minX - plotSettings.maxX) / 2;
    double my = fabs(plotSettings.minY - plotSettings.maxY) / 2;

    plotSettings.minX -= mx;
    plotSettings.maxX += mx;
    plotSettings.minY -= my;
    plotSettings.maxY += my;
    refreshPixmap();
//	}
}

void Plotter2d::zoomIn()
{
    double mx = fabs(plotSettings.minX - plotSettings.maxX) / 4;
    double my = fabs(plotSettings.minY - plotSettings.maxY) / 4;

    plotSettings.minX += mx;
    plotSettings.maxX -= mx;
    plotSettings.minY += my;
    plotSettings.maxY -= my;

    qDebug() << plotSettings.minX << ", " << plotSettings.maxX << ", " << plotSettings.minY << ", " << plotSettings.maxY << "[" << plotSettings.spanX() << ", " << plotSettings.spanY() << "]";
    refreshPixmap();
//    }
}

//void Plotter::setCurveData(int id, const QVector<QPointF> &data)
//{
//    curveMap[id] = data;
//    refreshPixmap();
//}

//void Plotter::clearCurve(int id)
//{
//    curveMap.remove(id);
//    refreshPixmap();
//}

//void Plotter::addFunction(QString expression, QColor color)
//{
//    Function2d *f = new Function2d();
//    f->color =  color;
//    f->expression = expression;
//    f->evaluator.setExpression(f->expression.toStdString());
//    f->recalculate(plotSettings);
//
//    functionsList.append(f);
//
//    refreshPixmap();
//}

QSize Plotter2d::minimumSizeHint() const
{
    return QSize(6 * Margin, 4 * Margin);
}

QSize Plotter2d::sizeHint() const
{
    return QSize(12 * Margin, 8 * Margin);
}

void Plotter2d::paintEvent(QPaintEvent * /* event */)
{
    QStylePainter painter(this);
    painter.drawPixmap(0, 0, pixmap);

    QPen p(QColor(0x444444));
    p.setStyle(Qt::DashLine);

    if (rubberBandIsShown) {
        painter.setPen(p);
        painter.drawRect(rubberBandRect.normalized()
                                       .adjusted(0, 0, -1, -1));
    }
	if (hasFocus()) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.backgroundColor = palette().dark().color();
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, option);
    }
}

void Plotter2d::resizeEvent(QResizeEvent * /* event */)
{
//    int x = width() - (zoomInButton->width()
//                       + zoomOutButton->width() + 10);
//    zoomInButton->move(x, 5);
//    zoomOutButton->move(x + zoomInButton->width() + 5, 5);
    refreshPixmap(false);
}

void Plotter2d::mousePressEvent(QMouseEvent *event)
{
    if(showRubberBand)
    {
        QRect rect(Margin, Margin,
                   width() - 2 * Margin, height() - 2 * Margin);
        if (event->button() == Qt::LeftButton) {
            if (rect.contains(event->pos())) {
                rubberBandIsShown = true;
                rubberBandRect.setTopLeft(event->pos());
                rubberBandRect.setBottomRight(event->pos());
                updateRubberBandRegion();
                setCursor(Qt::CrossCursor);
            }
        }
    }

}

void Plotter2d::mouseMoveEvent(QMouseEvent *event)
{
    if (rubberBandIsShown) {
        updateRubberBandRegion();
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
    }
}

void Plotter2d::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && rubberBandIsShown) {
        updateRubberBandRegion();
        unsetCursor();
        QRect rect = rubberBandRect.normalized();
        if (rect.width() < 4 || rect.height() < 4)
        {
            showRubberBand = rubberBandIsShown = false;
            refreshPixmap(false);
            emit zoomSelectionFinished();
            return;
        }
        rect.translate(-Margin, -Margin);
//        PlotSettings prevSettings = zoomStack[curZoom];
        PlotSettings2d prevSettings = plotSettings;
//        PlotSettings settings;
//        double dx = prevSettings.spanX() / (width() - 2 * Margin);
//        double dy = prevSettings.spanY() / (height() - 2 * Margin);
        double dx = plotSettings.spanX() / (width() - 2 * Margin);
        double dy = plotSettings.spanY() / (height() - 2 * Margin);
        plotSettings.minX = prevSettings.minX + dx * rect.left();
        plotSettings.maxX = prevSettings.minX + dx * rect.right();
        plotSettings.minY = prevSettings.maxY - dy * rect.bottom();
        plotSettings.maxY = prevSettings.maxY - dy * rect.top();
        plotSettings.adjust();
//        zoomStack.resize(curZoom + 1);
//        zoomStack.append(settings);
//        zoomIn();
        refreshPixmap();

        showRubberBand = rubberBandIsShown = false;

        emit zoomSelectionFinished();
    }
}

void Plotter2d::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Left:
        plotSettings.scroll(-1, 0);
        refreshPixmap();
        break;
    case Qt::Key_Right:
        plotSettings.scroll(+1, 0);
        refreshPixmap();
        break;
    case Qt::Key_Down:
        plotSettings.scroll(0, -1);
        refreshPixmap();
        break;
    case Qt::Key_Up:
        plotSettings.scroll(0, +1);
        refreshPixmap();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void Plotter2d::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numTicks = numDegrees / 15;
    if (event->orientation() == Qt::Horizontal) {
    //    zoomStack[curZoom].scroll(numTicks, 0);
        plotSettings.scroll(numTicks, 0);
    } else {
    //    zoomStack[curZoom].scroll(0, numTicks);
        plotSettings.scroll(0, numTicks);
    }
    refreshPixmap();
}

void Plotter2d::updateRubberBandRegion()
{
    QRect rect = rubberBandRect.normalized();
    update(rect.left(), rect.top(), rect.width(), 1);
    update(rect.left(), rect.top(), 1, rect.height());
    update(rect.left(), rect.bottom(), rect.width(), 1);
    update(rect.right(), rect.top(), 1, rect.height());
}

void Plotter2d::refreshPixmap(bool recalculate)
{
    pixmap = QPixmap(size());
    pixmap.fill(this, 0, 0);
    QPainter painter(&pixmap);
    painter.initFrom(this);
    painter.setRenderHint(QPainter::Antialiasing, plotSettings.enableAntialiasing);
    drawGrid(&painter);
    drawCurves(&painter, recalculate);
    if(plotSettings.showLegend)
        drawLegend(&painter);
    update();
}

void Plotter2d::drawLegend(QPainter *painter)
{
    for(int i = 0; i < functionsList.size(); ++i)
    {
        painter->setPen(functionsList[i]->color);
        painter->drawText(Margin + 5, Margin + 15 + 15 * i, functionsList[i]->expression);
    }
}

void Plotter2d::drawGrid(QPainter *painter)
{

    QRect rect(Margin, Margin,
               width() - 2 * Margin, height() - 2 * Margin);
    if (!rect.isValid())
                return;

    QPen gridPen(plotSettings.gridColor);
    QPen labelPen(plotSettings.labelsColor);
    QPen axisPen(plotSettings.axesColor);

    bool x0Visible = true, y0Visible = true;

    // Oś x (x dla kanwy z marginesami)
    int x0 = rect.left() + (0.0 - plotSettings.minX) * (rect.width() - 1) / plotSettings.spanX();

    // Jeśli punkt x0 nie jest widoczny dla danych opcji kreślenia,
    // to oś zostanie naryzowana na górze albo na dole, odpowiednio dla aktualnych
    // opcji kreślenia
    if(x0 < rect.left()) {
        x0 = rect.left();
        x0Visible = false;
    }
    else if(x0 > rect.right()) {
        x0 = rect.right();
        x0Visible = false;
    }

    // Krok dzielący punkty-etykiety na osi X
    double xLabelStep = plotSettings.spanX() / plotSettings.numXTicks;
    qDebug() << "xLabelStep: " << xLabelStep;

    // Od jakiej współrzędnej x zacząć rysowanie (zaokręglenie po to, aby na wykresie uwzględnić oś)
    double startX = ceil(plotSettings.minX / xLabelStep) * xLabelStep;
    qDebug() << "startX: " << startX;

    // Analogicznie oś Y
    int y0 = rect.bottom() - (0.0 - plotSettings.minY) * (rect.height() - 1) / plotSettings.spanY();
    double yLabelStep = plotSettings.spanY() / plotSettings.numYTicks;
    qDebug() << "yLabelStep: " << yLabelStep;
    double startY = ceil(plotSettings.minY / yLabelStep) * yLabelStep;
    qDebug() << "startY: " << startY;

    if(y0 > rect.bottom()) {
        y0 = rect.bottom();
        y0Visible = false;
    }
    else if(y0 < rect.top()) {
        y0 = rect.top();
        y0Visible = false;
    }

    qDebug() << rect.bottom() << " " << rect.top();

    // Siatka

    for(int i = 0; i <= plotSettings.numXTicks; ++i) {
        int x = rect.left() + ((startX + i * xLabelStep) - plotSettings.minX) * (rect.width() - 1) / plotSettings.spanX();
        if(rect.left() > x || rect.right() < x) continue;

        if(plotSettings.showGrid)
        {
            painter->setPen(gridPen);
            painter->drawLine(x, rect.bottom(), x, rect.top());
        }

        if(plotSettings.showLabels)
        {
            painter->setPen(labelPen);
            painter->drawLine(x, y0, x, y0 + 5);

            if(startX + xLabelStep * i == 0) continue;

            painter->drawText(x - 50, y0 + 5, 100, 20, Qt::AlignHCenter | Qt::AlignTop, QString::number(startX + xLabelStep * i));

        }
    }

    for(int i = 0; i <= plotSettings.numYTicks; ++i) {
        int y = rect.bottom() - ((startY + i * yLabelStep) - plotSettings.minY) * (rect.height() - 1) / plotSettings.spanY();
        if(rect.bottom() < y || rect.top() > y) continue;

        if(plotSettings.showGrid)
        {
            painter->setPen(gridPen);
            painter->drawLine(rect.left(), y, rect.right(), y);
        }

        if(plotSettings.showLabels)
        {
            painter->setPen(labelPen);
            painter->drawLine(x0, y, x0 + 5, y);

            if(startY + yLabelStep * i == 0) continue;

            painter->drawText(x0 + 10, y - 6, 100, 20, Qt::AlignLeft | Qt::AlignTop, QString::number(startY + yLabelStep * i));

        }
    }

    // etykieta dla punktu (0, 0)
    if((x0Visible || y0Visible) && plotSettings.showLabels)
        painter->drawText(x0 + 10, y0 + 5, 100, 20, Qt::AlignLeft | Qt::AlignTop, "0");


    // Osie, groty i etykiety

    if(plotSettings.showAxes)
    {
        painter->setPen(axisPen);
        painter->drawLine(x0, rect.bottom() + 30, x0, rect.top() - 30);
        painter->drawLine(x0 + 4, rect.top() - 22, x0, rect.top() - 30);
        painter->drawLine(x0 - 4, rect.top() - 22, x0, rect.top() - 30);
        painter->drawText(x0 + 10, rect.top() - 35, 100, 20,  Qt::AlignLeft | Qt::AlignTop, "Y");

        painter->drawLine(rect.left() - 30, y0, rect.right() + 30, y0);
        painter->drawLine(rect.right() + 22, y0 - 4, rect.right() + 29, y0);
        painter->drawLine(rect.right() + 22, y0 + 4, rect.right() + 29, y0);
        painter->drawText(rect.right() + 35, y0 + 10, 100, 20,  Qt::AlignLeft | Qt::AlignTop, "X");
    }


//    double noOfXLeftTicks = fabs(0.0 - plotSettings.minX) / xLabelStep;
//    qDebug() << "noOfXleftTicks: " << noOfXLeftTicks;
//
//    double noOfXRightTicks = fabs(0.0 - plotSettings.maxX) / xLabelStep;
//    qDebug() << "noOfXleftTicks: " << noOfXRightTicks;

    // Oś y


//    PlotSettings settings = plotSettings;
//
//    QPen gray = QPen(QColor(0xCCCCCC));
//    QPen black = QPen(Qt::black);
//    for (int i = 0; i <= settings.numXTicks; ++i) {
//         int x = rect.left() + (i * (rect.width() - 1)
//                                  / settings.numXTicks);
//         double label = settings.minX + (i * settings.spanX()
//                                           / settings.numXTicks);
//         painter->setPen(gray);
//         painter->drawLine(x, rect.top(), x, rect.bottom());
//         painter->setPen(black);
//         painter->drawLine(x, rect.bottom(), x, rect.bottom() + 5);
//         painter->drawText(x - 50, rect.bottom() + 5, 100, 20,
//                              Qt::AlignHCenter | Qt::AlignTop,
//                              QString::number(label));
//    }
//
//    for (int j = 0; j <= settings.numYTicks; ++j) {
//        int y = rect.bottom() - (j * (rect.height() - 1)
//                                   / settings.numYTicks);
//        double label = settings.minY + (j * settings.spanY()
//                                          / settings.numYTicks);
//        painter->setPen(gray);
//        painter->drawLine(rect.left(), y, rect.right(), y);
//        painter->setPen(black);
//        painter->drawLine(rect.left() - 5, y, rect.left(), y);
//        painter->drawText(rect.left() - Margin, y - 10, Margin - 5, 20,
//                             Qt::AlignRight | Qt::AlignVCenter,
//                             QString::number(label));
//    }
//
//    painter->drawLine(rect.left(), rect.bottom(), rect.left(), rect.top());
//    painter->drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());
}

void Plotter2d::drawCurves(QPainter *painter, bool recalculate)
{
    PlotSettings2d settings = plotSettings;
    QRect rect(Margin, Margin,
               width() - 2 * Margin, height() - 2 * Margin);
    if (!rect.isValid())
        return;
    painter->setClipRect(rect.adjusted(+1, +1, -1, -1));

    QVectorIterator<Function2d*> it(functionsList);
    while (it.hasNext()) {
        Function2d *f = it.next();
        if(!f->active) continue;

        if(recalculate)
            f->recalculate(plotSettings);

        QPointF* data = f->curveData;

        // Tablica zawierająca dopasowane współrzędne
        QPointF* fitData = new QPointF[f->pointsCount];

        // Mała optymalizacja obliczeń (zmniejszy liczbę wywołań funkcji)
        int rect_left = rect.left();
        int rect_bottom = rect.bottom();
        double fit_width = (rect.width() - 1) / settings.spanX();
        double fit_height = (rect.height() - 1) / settings.spanY();

        for (int j = 0; j < f->pointsCount; ++j) {
            double dx = data[j].x() - settings.minX;
            double dy = data[j].y() - settings.minY;
            double x = rect_left + dx * fit_width;
            double y = rect_bottom - dy * fit_height;
            fitData[j] = QPointF(x, y);
            //qDebug() << x << ", " << y;
        }

        painter->setPen(f->color);
        //painter->drawPolyline(polyline);
        for (int j = 0; j < f->pointsCount-1; ++j) {
            /**
             * Wykres jest w pewnych wypadkach nieprawidłowo kreślony,
             * np. dla funkcji tangens przy użyciu funkcji polyline
             * Jeśli różnica wartości pomiędzy dwoma sąsiadującymi
             * punktami jest zbyt duża to są one pomijane.
             * Tu także omijamn NaN-y, np. pierwiastki z liczb ujemnych
             * albo wartości funkcji dla argumentów poza jej dziedziną
             */

            if(fabs(data[j].y() - data[j+1].y()) > plotSettings.spanY() ||
               data[j].y() != data[j].y() ||
               data[j+1].y() != data[j+1].y()) {
            //    qDebug() << "Leaving: " << data[j] << " - " << data[j+1] << ", diff: " << fabs(data[j].y() - data[j+1].y()) << ", span: " << settings.spanY();
                continue;
            }
            painter->drawLine(fitData[j].x(), fitData[j].y(),
                              fitData[j+1].x(), fitData[j+1].y());
        }

        delete[] fitData;
    }
}

void Plotter2d::scrollLeft()
{
    plotSettings.scroll(-1, 0);
    refreshPixmap();
}

void Plotter2d::scrollRight()
{
    plotSettings.scroll(1, 0);
    refreshPixmap();
}

void Plotter2d::scrollUp()
{
    plotSettings.scroll(0, 1);
    refreshPixmap();
}

void Plotter2d::scrollDown()
{
    plotSettings.scroll(0, -1);
    refreshPixmap();
}

void Plotter2d::zoomSelectionStart()
{
    showRubberBand = true;
}

void Plotter2d::zoomSelectionCancel()
{
    showRubberBand = rubberBandIsShown = false;
}

QPixmap& Plotter2d::getPixmap()
{
    return pixmap;
}

PlotSettings2d Plotter2d::getPlotsettings() const
{
    return plotSettings;
}

PlotSettings2d::PlotSettings2d()
{
    minX = -10.0;
    maxX = 10.0;
    numXTicks = 10;
    minY = -10.0;
    maxY = 10.0;
    numYTicks = 10;
    
    showAxes = true;
    axesColor = Qt::black;
    showLabels = true;
    labelsColor = QColor(0x444444);
    showGrid = true;
    gridColor = QColor(0xE8E8E8);

    bgColor = Qt::white;

    showLegend = false;
    enableAntialiasing = false;
}

void PlotSettings2d::scroll(int dx, int dy)
{
    double stepX = spanX() / numXTicks;
    minX += dx * stepX;
    maxX += dx * stepX;
    double stepY = spanY() / numYTicks;
    minY += dy * stepY;
    maxY += dy * stepY;
}

void PlotSettings2d::adjust()
{
    adjustAxis(minX, maxX);
    adjustAxis(minY, maxY);
}

void PlotSettings2d::adjustAxis(double &min, double &max/*, int &numTicks*/)
{
    const int MinTicks = 4;
    double grossStep = (max - min) / MinTicks;
    double step = std::pow(10.0, std::floor(std::log10(grossStep)));
    if (5 * step < grossStep) {
        step *= 5;
    } else if (2 * step < grossStep) {
        step *= 2;
    }
//    numTicks = int(std::ceil(max / step) - std::floor(min / step));
//    if (numTicks < MinTicks)
//        numTicks = MinTicks;
    min = std::floor(min / step) * step;
    max = std::ceil(max / step) * step;
}

QDomElement PlotSettings2d::toXml(QDomDocument& doc)
{
    QDomElement plotSetting2dElem = doc.createElement("plotsettings2d");

    FVFormatHandler::elementXml(doc, plotSetting2dElem, QString("minX"), QString::number(minX));
    FVFormatHandler::elementXml(doc, plotSetting2dElem, QString("maxX"), QString::number(maxX));
    FVFormatHandler::elementXml(doc, plotSetting2dElem, QString("minY"), QString::number(minY));
    FVFormatHandler::elementXml(doc, plotSetting2dElem, QString("maxY"), QString::number(maxY));

    FVFormatHandler::elementXml(doc, plotSetting2dElem, QString("showAxes"), showAxes ? QString("yes") : QString("no"));
    if(showAxes) FVFormatHandler::elementXml(doc, plotSetting2dElem, QString("axesColor"), axesColor.name());
    FVFormatHandler::elementXml(doc, plotSetting2dElem, QString("showLabels"), showLabels ? QString("yes") : QString("no"));
    if(showLabels) FVFormatHandler::elementXml(doc, plotSetting2dElem, QString("labelsColor"), labelsColor.name());
    FVFormatHandler::elementXml(doc, plotSetting2dElem, QString("showGrid"), showGrid ? QString("yes") : QString("no"));
    if(showGrid) FVFormatHandler::elementXml(doc, plotSetting2dElem, QString("gridColor"), gridColor.name());

    FVFormatHandler::elementXml(doc, plotSetting2dElem, QString("bgColor"), bgColor.name());
    FVFormatHandler::elementXml(doc, plotSetting2dElem, QString("showLegend"), showLegend ? QString("yes") : QString("no"));
    FVFormatHandler::elementXml(doc, plotSetting2dElem, QString("enableAntialiasing"), enableAntialiasing ? QString("yes") : QString("no"));

    return plotSetting2dElem;
}
