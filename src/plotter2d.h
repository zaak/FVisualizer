#ifndef PLOTTER_H
#define PLOTTER_H

#include <QtCore/QMap>
#include <QPixmap>
#include <QWidget>
#include <QToolButton>

#include "function2d.h"

class PlotSettings2d
{
public:
        PlotSettings2d();
        QDomElement toXml(QDomDocument& doc);

        void scroll(int dx, int dy);
        void adjust();
        double spanX() const { return maxX - minX; };
        double spanY() const { return maxY - minY; };

        double minX;
        double maxX;
        int numXTicks;
        double minY;
        double maxY;
        int numYTicks;

        bool showAxes;
        QColor axesColor;
        bool showLabels;
        QColor labelsColor;
        bool showGrid;
        QColor gridColor;

        QColor bgColor;

        bool showLegend;
        bool enableAntialiasing;

private:
        static void adjustAxis(double &min, double &max/*, int &numTicks*/);
};

class Plotter2d : public QWidget
{
        Q_OBJECT

//        friend class GraphWindow2d;

public:
        Plotter2d(QVector<Function2d*>& functionsList, QWidget *parent = 0);

        void setPlotSettings(const PlotSettings2d &settings);
        void refreshPixmap(bool recalculate = true);
//	void setCurveData(int id, const QVector<QPointF> &data);
//	void clearCurve(int id);
	QSize minimumSizeHint() const;
	QSize sizeHint() const;

        PlotSettings2d getPlotsettings() const;
        QPixmap& getPixmap();

        // Lista funkcji
        QVector<Function2d*>& functionsList;

        //void addFunction(QString expression, QColor color);

public slots:
	void zoomIn();
        void zoomOut();
        void scrollUp();
        void scrollDown();
        void scrollLeft();
        void scrollRight();

        void zoomSelectionStart();
        void zoomSelectionCancel();

signals:
        void zoomSelectionFinished();

protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event);

private:
        void updateRubberBandRegion();
	void drawGrid(QPainter *painter);
        void drawCurves(QPainter *painter, bool recalculate);
        void drawLegend(QPainter *painter);

	enum { Margin = 50 };

//	QToolButton *zoomInButton;
//	QToolButton *zoomOutButton;
//	QMap<int, QVector<QPointF>> curveMap;
//	QVector<PlotSettings> zoomStack;
        PlotSettings2d plotSettings;
	int curZoom;
	bool rubberBandIsShown;
        bool showRubberBand;
	QRect rubberBandRect;
	QPixmap pixmap;
};


#endif // PLOTTER_H
