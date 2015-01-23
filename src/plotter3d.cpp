#include <QtGui>
#include "plotter3d.h"
#include <cmath>
#include <QTimer>
#include "fvformathandler.h"
#include <GL/glu.h>

PlotSettings3d::PlotSettings3d()
{
    minX = -10.0;
    maxX = 10.0;
    numXTicks = 4;
    minY = -10.0;
    maxY = 10.0;
    numYTicks = 4;
    minZ = -10.0;
    maxZ = 10.0;
    numZTicks = 4;

    showEdges = true;
    edgesColor = QColor(0x888888);
    showLabels = true;
    labelsColor = QColor(0x888888);
    bgColor = QColor(Qt::black);

    acwRotation = false;
    showCSMiniature = true;
    showDirectionsMarkers = true;
    showLegend = true;
}

void PlotSettings3d::scroll(int dx, int dy, int dz)
{
    double stepX = spanX() / numXTicks;
    minX += dx * stepX;
    maxX += dx * stepX;
    double stepY = spanY() / numYTicks;
    minY += dy * stepY;
    maxY += dy * stepY;
    double stepZ = spanZ() / numZTicks;
    minZ += dz * stepZ;
    maxZ += dz * stepZ;
}

QDomElement PlotSettings3d::toXml(QDomDocument& doc)
{
    QDomElement plotSetting3dElem = doc.createElement("plotsettings3d");

    FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("minX"), QString::number(minX));
    FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("maxX"), QString::number(maxX));
    FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("minY"), QString::number(minY));
    FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("maxY"), QString::number(maxY));
    FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("minZ"), QString::number(minZ));
    FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("maxZ"), QString::number(maxZ));

    FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("showEdges"), showEdges ? QString("yes") : QString("no"));
    if(showEdges) FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("edgesColor"), edgesColor.name());
    FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("showLabels"), showLabels ? QString("yes") : QString("no"));
    if(showLabels) FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("labelsColor"), labelsColor.name());

    FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("bgColor"), bgColor.name());

    FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("acwRotation"), acwRotation ? QString("yes") : QString("no"));
    FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("showCSMiniature"), showCSMiniature ? QString("yes") : QString("no"));
    FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("showDirectionsMarkers"), showDirectionsMarkers ? QString("yes") : QString("no"));
    FVFormatHandler::elementXml(doc, plotSetting3dElem, QString("showLegend"), showLegend ? QString("yes") : QString("no"));

    return plotSetting3dElem;
}

void Plotter3d::setPlotSettings(const PlotSettings3d &settings)
{
    plotSettings3d = settings;

    glClearColor (plotSettings3d.bgColor.redF(), plotSettings3d.bgColor.greenF(),
                  plotSettings3d.bgColor.blueF(), 0.0f);

    updateGL();
}

Plotter3d::Plotter3d(QVector<Function3d*>& functionsList, QVector<Ray>& raysList, QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent), ArcBall(640.0f, 480.0f),
	functionsList(functionsList), raysList(raysList), rotating(false), quadratic(NULL)
{

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);

    // Transform   = {  1.0f,  0.0f,  0.0f,  0.0f,
    //                  0.0f,  1.0f,  0.0f,  0.0f,
    //                  0.0f,  0.0f,  1.0f,  0.0f,
    //                  0.0f,  0.0f,  0.0f,  1.0f };

    // Inicjalizacja macierzy służących o obliczeń rotacji
    // (macierze jenostkowe, element neutralny mnożenia macierzy)

    Transform.M[0] = Transform.M[5] = Transform.M[10] = Transform.M[15] = 1.0f;
    Transform.M[1] = Transform.M[2] = Transform.M[3] =
    Transform.M[4] = Transform.M[6] = Transform.M[7] =
    Transform.M[8] = Transform.M[9] = Transform.M[11] =
    Transform.M[12] = Transform.M[13] = Transform.M[14] = 0.0f;

    LastRot.M[0] = LastRot.M[4] = LastRot.M[8] = 1.0;
    LastRot.M[1] = LastRot.M[2] = LastRot.M[3] =
    LastRot.M[5] = LastRot.M[6] = LastRot.M[7] = 0.0;

    ThisRot.M[0] = ThisRot.M[4] = ThisRot.M[8] = 1.0;
    ThisRot.M[1] = ThisRot.M[2] = ThisRot.M[3] =
    ThisRot.M[5] = ThisRot.M[6] = ThisRot.M[7] = 0.0;

    //Clip planes
    planeXmin[0] = 1.0;
    planeXmin[1] = 0.0;
    planeXmin[2] = 0.0;
    planeXmin[3] = 10.0;

    planeXmax[0] = -1.0;
    planeXmax[1] = 0.0;
    planeXmax[2] = 0.0;
    planeXmax[3] = 10.0;

    planeYmin[0] = 0.0;
    planeYmin[1] = 0.0;
    planeYmin[2] = 1.0;
    planeYmin[3] = 10.0;

    planeYmax[0] = 0.0;
    planeYmax[1] = 0.0;
    planeYmax[2] = -1.0;
    planeYmax[3] = 10.0;

    planeZmax[0] = 0.0;
    planeZmax[1] = -1.0;
    planeZmax[2] = 0.0;
    planeZmax[3] = 10.0;
    
    planeZmin[0] = 0.0;
    planeZmin[1] = 1.0;
    planeZmin[2] = 0.0;
    planeZmin[3] = 10.0;

    rotationTimer = new QTimer(this);
    timerDelay = 30;
    connect(rotationTimer, SIGNAL(timeout()), this, SLOT(rotateGraph()));

}

Plotter3d::~Plotter3d()
{
    if(quadratic != NULL)
        gluDeleteQuadric(quadratic);
}

void Plotter3d::initializeGL()
{
    glClearColor(plotSettings3d.bgColor.redF(),
                 plotSettings3d.bgColor.greenF(),
                 plotSettings3d.bgColor.blueF(), 0.0f);

    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);

    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    quadratic = gluNewQuadric();
    gluQuadricNormals(quadratic, GLU_SMOOTH);
}

void Plotter3d::resizeGL(int width, int height)
{
    ArcBall.setBounds(width, height);

    //int side = qMin(width, height);
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);

}

void Plotter3d::drawSmallAxes()
{
    glColor3d(0.0, 1.0, 0.0);
    glPushMatrix();
    glScalef(0.6f, 0.6f, 0.6f);
        gluDisk(quadratic, 0.0, 0.3, 10, 10);
        gluCylinder(quadratic, 0.3, 0.3, 3.0, 10, 10);
        glPushMatrix();
            glTranslatef(0.0, 0.0, 3.0);
            gluDisk(quadratic, 0.0, 0.5, 10, 10);
            gluCylinder(quadratic, 0.5, 0.0, 1.0, 10, 10);
        glPopMatrix();
    glPopMatrix();

    glColor3d(0.0, 0.0, 1.0);
    glPushMatrix();
        glRotatef(90.0, -1.0, 0.0, 0.0);
    glScalef(0.6f, 0.6f, 0.6f);
        gluDisk(quadratic, 0.0, 0.3, 10, 10);
        gluCylinder(quadratic, 0.3, 0.3, 3.0, 10, 10);
        glPushMatrix();
            glTranslatef(0.0, 0.0, 3.0);
            gluDisk(quadratic, 0.0, 0.5, 10, 10);
            gluCylinder(quadratic, 0.5, 0.0, 1.0, 10, 10);
        glPopMatrix();
    glPopMatrix();

    glColor3d(1.0, 0.0, 0.0);
    glPushMatrix();
        glRotatef(90.0, -1.0, 0.0, 0.0);
        glRotatef(90.0, 0.0, 1.0, 0.0);
    glScalef(0.6f, 0.6f, 0.6f);
        gluDisk(quadratic, 0.0, 0.3, 10, 10);
        gluCylinder(quadratic, 0.3, 0.3, 3.0, 10, 10);
        glPushMatrix();
            glTranslatef(0.0, 0.0, 3.0);
            gluDisk(quadratic, 0.0, 0.5, 10, 10);
            gluCylinder(quadratic, 0.5, 0.0, 1.0, 10, 10);
        glPopMatrix();
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glColor3d(0.0, 1.0, 0.0);
    renderText(0.0, 0.0, 2.8, "Y");
    glColor3d(0.0, 0.0, 1.0);
    renderText(0.0, 2.8, 0.0, "Z");
    glColor3d(1.0, 0.0, 0.0);
    renderText(2.8, 0.0, 0.0, "X");
    glEnable(GL_LIGHTING);
}

void Plotter3d::drawAxes()
{
    glDisable(GL_LIGHTING);

    if(plotSettings3d.showEdges)
    {
        glBegin(GL_LINES);

            glColor3f(plotSettings3d.edgesColor.redF(), plotSettings3d.edgesColor.greenF(),
                      plotSettings3d.edgesColor.blueF());

        //    glColor3d(1.0, 0.0, 0.0);
            glVertex3i(-10, 10, 10);
            glVertex3i(10, 10, 10);
            glVertex3i(-10, -10, 10);
            glVertex3i(10, -10, 10);
            glVertex3i(-10, 10, -10);
            glVertex3i(10, 10, -10);
            glVertex3i(-10, -10, -10);
            glVertex3i(10, -10, -10);

       //     glColor3d(0.0, 0.0, 1.0);
            glVertex3i(10, -10, 10);
            glVertex3i(10, 10, 10);
            glVertex3i(10, -10, -10);
            glVertex3i(10, 10, -10);
            glVertex3i(-10, -10, -10);
            glVertex3i(-10, 10, -10);
            glVertex3i(-10, -10, 10);
            glVertex3i(-10, 10, 10);

      //      glColor3d(0.0, 1.0, 0.0);
            glVertex3i(10, 10, -10);
            glVertex3i(10, 10, 10);
            glVertex3i(10, -10, -10);
            glVertex3i(10, -10, 10);
            glVertex3i(-10, 10, -10);
            glVertex3i(-10, 10, 10);
            glVertex3i(-10, -10, -10);
            glVertex3i(-10, -10, 10);
        glEnd();
    }


    if(plotSettings3d.showLabels)
    {
        glColor3f(plotSettings3d.labelsColor.redF(), plotSettings3d.labelsColor.greenF(),
                  plotSettings3d.labelsColor.blueF());

        double xLabelStep = plotSettings3d.spanX() / plotSettings3d.numXTicks;
        double yLabelStep = plotSettings3d.spanY() / plotSettings3d.numYTicks;
        double zLabelStep = plotSettings3d.spanZ() / plotSettings3d.numZTicks;
        double graphMinX = -10, graphMinY = -10, graphMinZ = -10;
        double graphMaxX = 10, graphMaxY = 10, graphMaxZ = 10;
        double graphSpanX = (graphMaxX - graphMinX);
        double graphSpanY = (graphMaxY - graphMinY);
        double graphSpanZ = (graphMaxZ - graphMinZ);
        double graphStepX = graphSpanX / plotSettings3d.numXTicks;
        double graphStepY = graphSpanY / plotSettings3d.numYTicks;
        double graphStepZ = graphSpanZ / plotSettings3d.numZTicks;

        //glColor3d(0.5, 0.0, 0.0);



        for(int i = 0; i <= plotSettings3d.numXTicks; ++i)
        {
            glBegin(GL_POINTS);
                glVertex3f(graphMinX + i * graphStepX, 10.3f, 10.3f);
                glVertex3f(graphMinX + i * graphStepX, 10.3f, -10.3f);
                glVertex3f(graphMinX + i * graphStepX, -10.3f, -10.3f);
                glVertex3f(graphMinX + i * graphStepX, -10.3f, 10.3f);
            glEnd();
            renderText(graphMinX + i * graphStepX, 10.5, 10.5, QString::number(plotSettings3d.minX + i * xLabelStep));
            renderText(graphMinX + i * graphStepX, 10.5, -10.5, QString::number(plotSettings3d.minX + i * xLabelStep));
            renderText(graphMinX + i * graphStepX, -10.5, -10.5, QString::number(plotSettings3d.minX + i * xLabelStep));
            renderText(graphMinX + i * graphStepX, -10.5, 10.5, QString::number(plotSettings3d.minX + i * xLabelStep));
        }

        //glColor3d(0.0, 0.0, 0.5);

        for(int i = 0; i <= plotSettings3d.numZTicks; ++i)
        {
            glBegin(GL_POINTS);
                glVertex3f(10.3f, graphMinZ + i * graphStepZ, 10.3f);
                glVertex3f(10.3f, graphMinZ + i * graphStepZ, -10.3f);
                glVertex3f(-10.3f, graphMinZ + i * graphStepZ, -10.3f);
                glVertex3f(-10.3f, graphMinZ + i * graphStepZ, 10.3f);
            glEnd();
            renderText(10.5, graphMinZ + i * graphStepZ, 10.5, QString::number(plotSettings3d.minZ + i * zLabelStep));
            renderText(10.5, graphMinZ + i * graphStepZ, -10.5, QString::number(plotSettings3d.minZ + i * zLabelStep));
            renderText(-10.5, graphMinZ + i * graphStepZ, -10.5, QString::number(plotSettings3d.minZ + i * zLabelStep));
            renderText(-10.5, graphMinZ + i * graphStepZ, 10.5, QString::number(plotSettings3d.minZ + i * zLabelStep));
        }

        //glColor3d(0.0, 0.5, 0.0);

        for(int i = 0; i <= plotSettings3d.numYTicks; ++i)
        {
            glBegin(GL_POINTS);
                glVertex3f(10.3f, 10.3f, graphMinY + i * graphStepY);
                glVertex3f(10.3f, -10.3f, graphMinY + i * graphStepY);
                glVertex3f(-10.3f, -10.3f, graphMinY + i * graphStepY);
                glVertex3f(-10.3f, 10.3f, graphMinY + i * graphStepY);
            glEnd();
            renderText(10.5, 10.5, graphMinY + i * graphStepY, QString::number(plotSettings3d.minY + i * yLabelStep));
            renderText(10.5, -10.5, graphMinY + i * graphStepY, QString::number(plotSettings3d.minY + i * yLabelStep));
            renderText(-10.5, -10.5, graphMinY + i * graphStepY, QString::number(plotSettings3d.minY + i * yLabelStep));
            renderText(-10.5, 10.5, graphMinY + i * graphStepY, QString::number(plotSettings3d.minY + i * yLabelStep));
        }
    }

    if(plotSettings3d.showDirectionsMarkers)
    {
        glEnable(GL_LIGHTING);
            glColor3d(0.0, 1.0, 0.0);
            glPushMatrix();
                glTranslatef(0.0, 0.0, 10.5);
                gluDisk(quadratic, 0.0, 0.3, 10, 10);
                gluCylinder(quadratic, 0.3, 0.3, 3.0, 10, 10);
                glPushMatrix();
                    glTranslatef(0.0, 0.0, 3.0);
                    gluDisk(quadratic, 0.0, 0.5, 10, 10);
                    gluCylinder(quadratic, 0.5, 0.0, 1.0, 10, 10);
                glPopMatrix();
            glPopMatrix();

            glColor3d(0.0, 0.0, 1.0);
            glPushMatrix();
                glRotatef(90.0, -1.0, 0.0, 0.0);
                glTranslatef(0.0, 0.0, 10.5);
                gluDisk(quadratic, 0.0, 0.3, 10, 10);
                gluCylinder(quadratic, 0.3, 0.3, 3.0, 10, 10);
                glPushMatrix();
                    glTranslatef(0.0, 0.0, 3.0);
                    gluDisk(quadratic, 0.0, 0.5, 10, 10);
                    gluCylinder(quadratic, 0.5, 0.0, 1.0, 10, 10);
                glPopMatrix();
            glPopMatrix();

            glColor3d(1.0, 0.0, 0.0);
            glPushMatrix();
                glRotatef(90.0, -1.0, 0.0, 0.0);
                glRotatef(90.0, 0.0, 1.0, 0.0);
                glTranslatef(0.0, 0.0, 10.5);
                gluDisk(quadratic, 0.0, 0.3, 10, 10);
                gluCylinder(quadratic, 0.3, 0.3, 3.0, 10, 10);
                glPushMatrix();
                    glTranslatef(0.0, 0.0, 3.0);
                    gluDisk(quadratic, 0.0, 0.5, 10, 10);
                    gluCylinder(quadratic, 0.5, 0.0, 1.0, 10, 10);
                glPopMatrix();
            glPopMatrix();

        glDisable(GL_LIGHTING);

        glColor3d(0.0, 1.0, 0.0);
        renderText(0.0, 0.0, 15.5, "Y");
        glColor3d(0.0, 0.0, 1.0);
        renderText(0.0, 15.5, 0.0, "Z");
        glColor3d(1.0, 0.0, 0.0);
        renderText(15.5, 0.0, 0.0, "X");
    }

    glEnable(GL_LIGHTING);
}

void Plotter3d::recalculateAll()
{
    QVectorIterator<Function3d*> it(functionsList);
    while (it.hasNext())
        it.next()->recalculate(plotSettings3d);
}

void Plotter3d::drawCurvatures(/*bool recalculate*/)
{

    //Function3d f3d(QString("x^ln(x)"));
    //f3d.recalculate(plotSettings3d);
//    glBegin(GL_QUADS);
//    glColor4f(1.0, 1.0, 1.0, 0.3);
//    for(int x = 0; x < 99; ++x)
//    {
//        for(int y = 0; y < 99; ++y)
//        {
//            glVertex3f(c[x][y].x(), c[x][y].z(), c[x][y].y());
//            glVertex3f(c[x][y+1].x(),c[x][y+1].z(),c[x][y+1].y());
//            glVertex3f(c[x+1][y+1].x(), c[x+1][y+1].z(),c[x+1][y+1].y());
//            glVertex3f(c[x+1][y].x(), c[x+1][y].z(), c[x+1][y].y());
//        }
//    }
//    glEnd();

    double graphMinX = -10, graphMinY = -10, graphMinZ = -10;
    double graphMaxX = 10, graphMaxY = 10, graphMaxZ = 10;
    double graphSpanX = (graphMaxX - graphMinX);
    double graphSpanY = (graphMaxY - graphMinY);
    double graphSpanZ = (graphMaxZ - graphMinZ);

    double fitX = graphSpanX / plotSettings3d.spanX();
    double fitY = graphSpanY / plotSettings3d.spanY();
    double fitZ = graphSpanZ / plotSettings3d.spanZ();

    QVectorIterator<Function3d*> it(functionsList);
    while (it.hasNext()) {
        Function3d *f = it.next();
        if(!f->active) continue;
//
//        if(recalculate)
//            f->recalculate(plotSettings3d);

        QVector3D** fc = f->curvatureData;

        QVector3D** c = new QVector3D*[f->pointsXCount];
        for(int i = 0; i < f->pointsXCount; ++i)
            c[i] = new QVector3D[f->pointsYCount];

        for(int x = 0; x < f->pointsXCount; ++x)
        {
            for(int y = 0; y < f->pointsYCount; ++y)
            {
                double dx = fc[x][y].x() - plotSettings3d.minX;
                double dy = fc[x][y].y() - plotSettings3d.minY;
                double dz = fc[x][y].z() - plotSettings3d.minZ;
                double xx = graphMinX + dx * fitX;
                double yy = graphMinY + dy * fitY;
                double zz = graphMinZ + dz * fitZ;
                c[x][y] = QVector3D(xx, yy, zz);
            }
        }

        double r, g, b, a;
        f->color.getRgbF(&r, &g, &b, &a);
        glColor4f(r, g, b, a);

        QVector3D n;
        QVectorIterator<QVector3D> nit(f->normals);

        if(f->drawingMethod == GL_QUADS)
        {
            glBegin(GL_QUADS);
            for(int x = 0; x < f->pointsXCount-1; ++x)
            {
                for(int y = 0; y < f->pointsYCount-1; ++y)
                {

                    if(nit.hasNext())
                    {
                        n = nit.next();
                        glNormal3f(n.x(), n.z(), n.y());
                    }

                    glVertex3f(c[x][y].x(), c[x][y].z(), c[x][y].y());
                    glVertex3f(c[x][y+1].x(),c[x][y+1].z(),c[x][y+1].y());
                    glVertex3f(c[x+1][y+1].x(), c[x+1][y+1].z(),c[x+1][y+1].y());
                    glVertex3f(c[x+1][y].x(), c[x+1][y].z(), c[x+1][y].y());
                }
            }
            glEnd();
        }

        else if(f->drawingMethod == GL_TRIANGLES)
        {
            glBegin(GL_TRIANGLES);
            for(int x = 0; x < f->pointsXCount-1; ++x)
            {
                for(int y = 0; y < f->pointsYCount-1; ++y)
                {

                    if(nit.hasNext())
                    {
                        n = nit.next();
                        glNormal3f(n.x(), n.z(), n.y());
                    }

                    glVertex3f(c[x+1][y+1].x(), c[x+1][y+1].z(),c[x+1][y+1].y());
                    glVertex3f(c[x][y+1].x(),c[x][y+1].z(),c[x][y+1].y());
                    glVertex3f(c[x][y].x(), c[x][y].z(), c[x][y].y());

                    if(nit.hasNext())
                    {
                        n = nit.next();
                        glNormal3f(n.x(), n.z(), n.y());
                    }

                    glVertex3f(c[x+1][y+1].x(), c[x+1][y+1].z(),c[x+1][y+1].y());
                    glVertex3f(c[x][y].x(), c[x][y].z(), c[x][y].y());
                    glVertex3f(c[x+1][y].x(), c[x+1][y].z(), c[x+1][y].y());
                }
            }
            glEnd();
        }

        else if(f->drawingMethod == GL_LINES)
        {
            glDisable(GL_LIGHTING);
            glBegin(GL_LINES);

                for(int x = 1; x < f->pointsXCount; ++x)
                {
                    for(int y = 1; y < f->pointsYCount; ++y)
                    {
                        glVertex3f(c[x][y].x(), c[x][y].z(), c[x][y].y());
                        glVertex3f(c[x][y-1].x(),c[x][y-1].z(),c[x][y-1].y());
                        glVertex3f(c[x-1][y-1].x(), c[x-1][y-1].z(),c[x-1][y-1].y());
                        glVertex3f(c[x-1][y].x(), c[x-1][y].z(), c[x-1][y].y());
                    }
                }

                for(int x = 0; x < f->pointsXCount-1; ++x)
                {
                    for(int y = 0; y < f->pointsYCount-1; ++y)
                    {
                        glVertex3f(c[x][y].x(), c[x][y].z(), c[x][y].y());
                        glVertex3f(c[x+1][y].x(), c[x+1][y].z(), c[x+1][y].y());
                        glVertex3f(c[x][y+1].x(),c[x][y+1].z(),c[x][y+1].y());
                        glVertex3f(c[x+1][y+1].x(), c[x+1][y+1].z(),c[x+1][y+1].y());
                    }
                }
            glEnd();
            glEnable(GL_LIGHTING);
        }

        else if(f->drawingMethod == GL_POINTS)
        {
            glDisable(GL_LIGHTING);
            glBegin(GL_POINTS);

                for(int x = 0; x < f->pointsXCount; ++x)
                {
                    for(int y = 0; y < f->pointsYCount; ++y)
                    {
                        glVertex3f(c[x][y].x(), c[x][y].z(), c[x][y].y());
                    }
                }
            glEnd();
            glEnable(GL_LIGHTING);
        }


        for(int i = 0; i < f->pointsXCount; ++i)
            delete[] c[i];
        delete[] c;
    }

	QVectorIterator<Ray> rit(raysList);
	QVector3D vbf, vef; // vector begin point fitted, vector end point fitted;
    while (rit.hasNext()) {
		Ray r = rit.next();

		// fit vectors

		double dx = r.beginPoint.x() - plotSettings3d.minX;
        double dy = r.beginPoint.y() - plotSettings3d.minY;
		double dz = r.beginPoint.z() - plotSettings3d.minZ;
        double xx = graphMinX + dx * fitX;
        double yy = graphMinY + dy * fitY;
        double zz = graphMinZ + dz * fitZ;
        vbf = QVector3D(xx, yy, zz);

		dx = r.endPoint.x() - plotSettings3d.minX;
        dy = r.endPoint.y() - plotSettings3d.minY;
		dz = r.endPoint.z() - plotSettings3d.minZ;
        xx = graphMinX + dx * fitX;
        yy = graphMinY + dy * fitY;
        zz = graphMinZ + dz * fitZ;
        vef = QVector3D(xx, yy, zz); // -0.5 for ending cone length

		// end fit calculations

		glColor3f(r.color.redF(), r.color.greenF(), r.color.blueF());

		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
			glVertex3d(vbf.x(), vbf.z(), vbf.y());
			glVertex3d(vef.x(), vef.z(), vef.y());
		glEnd();
				
		glEnable(GL_LIGHTING);

		QVector3D v = vef - vbf;

		double ax = acos(v.y()/v.length())*180/3.14159265; // in ax we have now angle between OZ
		if(v.y() < 0.0)
			ax = -ax;	// change sign if Z is negative (angle stays the same, we need to change it's sign)

		if(v.y() == 0.0)
			v.setY(0.0000001); // don't divide by 0

		qDebug() << "angle ax is: " << ax << " degrees";

		glPushMatrix();
			vef = vbf + v.normalized() * (v.length() - 0.5); // - length for cone

			glTranslated(vef.x(), vef.z(), vef.y()); // move cone to proper point

			glRotatef(ax, -v.z()*v.y(), v.x()*v.y(), 0); // apply rotation

			gluCylinder(quadratic, 0.2, 0, 0.5, 5, 5);
			
		glPopMatrix();
	}

}

void Plotter3d::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();								// Reset The Current Modelview Matrix
    glTranslatef(0.0f,0.0f,-40.0f);

    // Eye xyz, center xyz, upvector xyz

    //gluLookAt(10, 0, 0, 0, 0, 0, 0, 0, 1);



    glPushMatrix();
        glMultMatrixf(Transform.M);
        glRotatef(40.0, 0.0, -1.0, 0.0);
        drawAxes();
        glClipPlane(GL_CLIP_PLANE0, planeZmax);
        glClipPlane(GL_CLIP_PLANE1, planeZmin);
        glClipPlane(GL_CLIP_PLANE2, planeXmin);
        glClipPlane(GL_CLIP_PLANE3, planeXmax);
        glClipPlane(GL_CLIP_PLANE4, planeYmin);
        glClipPlane(GL_CLIP_PLANE5, planeYmax);
        glEnable(GL_CLIP_PLANE0);
        glEnable(GL_CLIP_PLANE1);
        glEnable(GL_CLIP_PLANE2);
        glEnable(GL_CLIP_PLANE3);
        glEnable(GL_CLIP_PLANE4);
        glEnable(GL_CLIP_PLANE5);
        drawCurvatures();
        glDisable(GL_CLIP_PLANE0);
        glDisable(GL_CLIP_PLANE1);
        glDisable(GL_CLIP_PLANE2);
        glDisable(GL_CLIP_PLANE3);
        glDisable(GL_CLIP_PLANE4);
        glDisable(GL_CLIP_PLANE5);
        glEnable(GL_DEPTH_TEST);
    glPopMatrix();


    //glRotatef(315.0, 0.0, 1.0, 0.0);
    //glRotatef(30.0, 1.0, 0.0, -1.0);

    if(plotSettings3d.showCSMiniature)
    {
        glLoadIdentity();
        glTranslatef(-22.0f, -13.5f, -40.0f);

        glPushMatrix();
            glMultMatrixf(Transform.M);
            glRotatef(40.0, 0.0, -1.0, 0.0);
            drawSmallAxes();
        glPopMatrix();
    }

    if(plotSettings3d.showLegend)
    {
        drawLegend();
    }

    glFlush();
}

void Plotter3d::drawLegend()
{
    for(int i = 0; i < functionsList.size(); ++i)
    {
        glColor3f(functionsList[i]->color.redF(),
                  functionsList[i]->color.greenF(),
                  functionsList[i]->color.blueF());
        renderText(10, 20 + 15 * i, functionsList[i]->expression);
    }

	for(int i = 0; i < raysList.size(); ++i)
    {
        glColor3f(raysList[i].color.redF(),
                  raysList[i].color.greenF(),
                  raysList[i].color.blueF());
		renderText(width()-200, 20 + 15 * i, QString("[%1, %2, %3] » [%4, %5, %6]")
												.arg(QString::number(raysList[i].beginPoint.x()))
												.arg(QString::number(raysList[i].beginPoint.y()))
												.arg(QString::number(raysList[i].beginPoint.z()))
												.arg(QString::number(raysList[i].endPoint.x()))
												.arg(QString::number(raysList[i].endPoint.y()))
												.arg(QString::number(raysList[i].endPoint.z())));
    }
}

void Plotter3d::mousePressEvent(QMouseEvent *event)
{
    //lastPos = event->pos();
    LastRot = ThisRot;
    MousePt.T[0] = event->x();
    MousePt.T[1] = event->y();
    ArcBall.click(&MousePt);

    if(rotating)
        rotationTimer->stop();
}

void Plotter3d::mouseReleaseEvent(QMouseEvent *event)
{
    if (rotating && !rotationTimer->isActive())
    {
        rotationTimer->start(timerDelay);
        qDebug() << " restarting timer after dragging";
    }
}

void Plotter3d::mouseMoveEvent(QMouseEvent *event)
{

    if (event->buttons() & Qt::LeftButton) {

        Quat4fT     ThisQuat;
        MousePt.T[0] = event->x();
        MousePt.T[1] = event->y();
        ArcBall.drag(&MousePt, &ThisQuat);						// Update End Vector And Get Rotation As Quaternion

        Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);		// Convert Quaternion Into Matrix3fT
        Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
        Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One
        updateGL();
    }

}


QSize Plotter3d::minimumSizeHint() const
{
    return QSize(6 * Margin, 4 * Margin);
}

QSize Plotter3d::sizeHint() const
{
    return QSize(12 * Margin, 8 * Margin);
}

PlotSettings3d Plotter3d::getPlotsettings() const
{
    return plotSettings3d;
}

void Plotter3d::zoomOut()
{
    double mx = fabs(plotSettings3d.minX - plotSettings3d.maxX) / 2;
    double my = fabs(plotSettings3d.minY - plotSettings3d.maxY) / 2;
    double mz = fabs(plotSettings3d.minZ - plotSettings3d.maxZ) / 2;

    plotSettings3d.minX -= mx;
    plotSettings3d.maxX += mx;
    plotSettings3d.minY -= my;
    plotSettings3d.maxY += my;
    plotSettings3d.minZ -= mz;
    plotSettings3d.maxZ += mz;

    recalculateAll();
    updateGL();
}

void Plotter3d::zoomIn()
{
    double mx = fabs(plotSettings3d.minX - plotSettings3d.maxX) / 4;
    double my = fabs(plotSettings3d.minY - plotSettings3d.maxY) / 4;
    double mz = fabs(plotSettings3d.minZ - plotSettings3d.maxZ) / 4;

    plotSettings3d.minX += mx;
    plotSettings3d.maxX -= mx;
    plotSettings3d.minY += my;
    plotSettings3d.maxY -= my;
    plotSettings3d.minZ += mz;
    plotSettings3d.maxZ -= mz;

    recalculateAll();
    updateGL();
}

void Plotter3d::scrollLeft()
{
    plotSettings3d.scroll(-1, 0, 0);
    recalculateAll();
    updateGL();
}

void Plotter3d::scrollRight()
{
    plotSettings3d.scroll(1, 0, 0);
    recalculateAll();
    updateGL();
}

void Plotter3d::scrollN()
{
    plotSettings3d.scroll(0, -1, 0);
    recalculateAll();
    updateGL();
}

void Plotter3d::scrollS()
{
    plotSettings3d.scroll(0, 1, 0);
    recalculateAll();
    updateGL();
}


void Plotter3d::scrollUp()
{
    plotSettings3d.scroll(0, 0, 1);
    recalculateAll();
    updateGL();
}

void Plotter3d::scrollDown()
{
    plotSettings3d.scroll(0, 0, -1);
    recalculateAll();
    updateGL();
}

void Plotter3d::rotateGraph()
{
    LastRot = ThisRot;
    MousePt.T[0] = plotSettings3d.acwRotation ? 0.01 : 0.0;
    MousePt.T[1] = height() / 2;
    ArcBall.click(&MousePt);

    Quat4fT     ThisQuat;

    MousePt.T[0] = plotSettings3d.acwRotation ? 0.0 : 0.01;
    MousePt.T[1] = height() / 2;
    ArcBall.drag(&MousePt, &ThisQuat);
    Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);
    Matrix3fMulMatrix3f(&ThisRot, &LastRot);
    Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);


    updateGL();
}

void Plotter3d::startRotation()
{
    rotationTimer->start(timerDelay);
    rotating = true;
}

void Plotter3d::stopRotation()
{
    rotationTimer->stop();
    rotating = false;
}
