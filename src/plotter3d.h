#ifndef Plotter3d_H
#define Plotter3d_H

#include <QGLWidget>
#include "ArcBall.h"
#include "function3d.h"
#include <GL/glu.h>

class PlotSettings3d
{
public:
        PlotSettings3d();
        QDomElement toXml(QDomDocument& doc);

        double spanX() const { return maxX - minX; };
        double spanY() const { return maxY - minY; };
        double spanZ() const { return maxZ - minZ; };
        void scroll(int dx, int dy, int dz);

        double minX;
        double maxX;
        int numXTicks;
        double minY;
        double maxY;
        int numYTicks;
        double minZ;
        double maxZ;
        int numZTicks;

        bool showEdges;
        QColor edgesColor;
        bool showLabels;
        QColor labelsColor;
        QColor bgColor;

        bool acwRotation;

        bool showCSMiniature;

        bool showDirectionsMarkers;

        bool showLegend;
};

class Plotter3d : public QGLWidget
{
    Q_OBJECT

public:
	Plotter3d(QVector<Function3d*>& functionsList, QVector<Ray>& raysList, QWidget *parent = 0);
    ~Plotter3d();
    void setPlotSettings(const PlotSettings3d &settings);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    PlotSettings3d getPlotsettings() const;

public slots:
    void zoomIn();
    void zoomOut();
    void scrollUp();
    void scrollDown();
    void scrollN();
    void scrollS();
    void scrollLeft();
    void scrollRight();
    void recalculateAll();
    void startRotation();
    void stopRotation();

private slots:
    void rotateGraph();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void drawAxes();
    void drawSmallAxes();
    void drawLegend();
    void drawCurvatures(/*bool recalculate = true*/);

private:
    Matrix4fT   Transform;
    Matrix3fT   LastRot;
    Matrix3fT   ThisRot;
    ArcBallT    ArcBall;
    Point2fT    MousePt;

    PlotSettings3d plotSettings3d;

    QVector<Function3d*>& functionsList;
	QVector<Ray>& raysList;

    enum { Margin = 50 };
    
    // Płaszczyzny odcinające
    double planeXmin[4];
    double planeXmax[4];
    double planeYmin[4];
    double planeYmax[4];
    double planeZmax[4];// = {0.0, -1.0, 0.0, 10.0};
    double planeZmin[4];// = {0.0, 1.0, 0.0, 10.0};

    QTimer* rotationTimer;
    bool rotating;
    int timerDelay;

    GLUquadricObj *quadratic;
};

#endif // Plotter3d_H
