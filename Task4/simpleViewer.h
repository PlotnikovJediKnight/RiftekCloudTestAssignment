#ifndef SIMPLEVIEWER_H
#define SIMPLEVIEWER_H

#include <QGLViewer/qglviewer.h>
#include <vector>
#include <algorithm>
#include "BallPivotingAlgorithm.h"

class Viewer : public QGLViewer {
public:
    Viewer(QWidget* parent);
    void SetPointCloud(const std::vector<GeneratedPoint>& pointCloud);
    void SetDrawScale(bool drawScale);
    void SetDrawGrid(bool drawGrid);
    void SetDrawNormals(bool drawNormals);
    void SetDrawSurface(bool drawSurface);
protected:
  virtual void draw();
  virtual void init();
  virtual QString helpString() const;
private:
    void DrawScale();
    void DrawGrid();
    void DrawNormals();
    GeneratedColor GetColorByZ(float z);
    std::vector<GeneratedPoint> point_cloud_;

    float min_x_;
    float min_y_;
    float max_x_;
    float max_y_;

    float max_z_;
    float min_z_;

    bool draw_scale_;
    bool draw_grid_;
    bool draw_surface_;
    bool draw_normals_;
};

#endif // SIMPLEVIEWER_H
