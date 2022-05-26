#include "simpleViewer.h"

using namespace std;

Viewer::Viewer(QWidget* parent) :
    QGLViewer(parent), draw_scale_(false),
    draw_grid_(false), draw_surface_(false),
    draw_normals_(false) {}


void Viewer::SetPointCloud(const std::vector<GeneratedPoint> &pointCloud)
{
    float currMin_x = pointCloud[0].x, currMax_x = currMin_x;
    float currMin_y = pointCloud[0].y, currMax_y = currMin_y;
    float currMin_z = pointCloud[0].z, currMax_z = currMin_z;

    for (auto& point : pointCloud){
        {
            if (point.x < currMin_x) currMin_x = point.x;
            if (point.x > currMax_x) currMax_x = point.x;
        }
        {
            if (point.y < currMin_y) currMin_y = point.y;
            if (point.y > currMax_y) currMax_y = point.y;
        }
        {
            if (point.z < currMin_z) currMin_z = point.z;
            if (point.z > currMax_z) currMax_z = point.z;
        }
    }
    point_cloud_.resize(pointCloud.size());
    copy(begin(pointCloud), end(pointCloud), begin(point_cloud_));

    min_x_ = currMin_x;
    max_x_ = currMax_x;

    min_y_ = currMin_y;
    max_y_ = currMax_y;

    min_z_ = currMin_z;
    max_z_ = currMax_z;

    this->setFocus();
}

void Viewer::SetDrawScale(bool drawScale)
{
    draw_scale_ = drawScale;
    this->setFocus();
}

void Viewer::SetDrawGrid(bool drawGrid)
{
    draw_grid_ = drawGrid;
    this->setFocus();
}

void Viewer::SetDrawNormals(bool drawNormals)
{
    draw_normals_ = drawNormals;
    this->setFocus();
}

void Viewer::SetDrawSurface(bool drawSurface)
{
    draw_surface_ = drawSurface;
    this->setFocus();
}

void Viewer::DrawScale(){
    const int viewerWidth  = this->width();
    const int viewerHeight = this->height();

    const int offset = 5;
    const int scaleWidth = 15;
    const int scaleHeight = viewerHeight - 2 * offset;

    const int scaleLeftTopX = viewerWidth - offset - scaleWidth;
    const int scaleLeftTopY = 5;

    const int scaleRightTopX = scaleLeftTopX + scaleWidth;
    const float regionHeight = scaleHeight / 4;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, viewerWidth, viewerHeight, 0.0, -1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_CULL_FACE);

    glClear(GL_DEPTH_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 0.0, 0.0, 0.5);
        glVertex2f(scaleLeftTopX, scaleLeftTopY);
        glVertex2f(scaleRightTopX, scaleLeftTopY);
    }
    {
        glColor4f(1.0, 1.0, 0.0, 0.5);
        glVertex2f(scaleRightTopX, scaleLeftTopY + regionHeight);
        glVertex2f(scaleLeftTopX, scaleLeftTopY + regionHeight);

        glVertex2f(scaleLeftTopX, scaleLeftTopY +  regionHeight);
        glVertex2f(scaleRightTopX, scaleLeftTopY + regionHeight);
    }
    {
        glColor4f(0.0, 1.0, 0.0, 0.5);
        glVertex2f(scaleRightTopX, scaleLeftTopY + 2 * regionHeight);
        glVertex2f(scaleLeftTopX, scaleLeftTopY +  2 * regionHeight);

        glVertex2f(scaleLeftTopX, scaleLeftTopY +  2 * regionHeight);
        glVertex2f(scaleRightTopX, scaleLeftTopY + 2 * regionHeight);
    }
    {
        glColor4f(0.0, 1.0, 1.0, 0.5);
        glVertex2f(scaleRightTopX, scaleLeftTopY + 3 * regionHeight);
        glVertex2f(scaleLeftTopX, scaleLeftTopY +  3 * regionHeight);

        glVertex2f(scaleLeftTopX, scaleLeftTopY +  3 * regionHeight);
        glVertex2f(scaleRightTopX, scaleLeftTopY + 3 * regionHeight);
    }
    {
        glColor4f(0.0, 0.0, 1.0, 0.5);
        glVertex2f(scaleRightTopX, scaleLeftTopY + 4 * regionHeight);
        glVertex2f(scaleLeftTopX, scaleLeftTopY +  4 * regionHeight);
    }

    glEnd();

    glDisable( GL_BLEND );

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    const int textOffset = 2 * offset;
    glColor3f(1.0, 1.0, 1.0);
    drawText(scaleLeftTopX, scaleLeftTopY + textOffset, "1.0", QFont("Helvetica", 5, 1));
    drawText(scaleLeftTopX, scaleLeftTopY + textOffset + regionHeight, "0.5", QFont("Helvetica", 5, 1));
    drawText(scaleLeftTopX, scaleLeftTopY + textOffset + 2 * regionHeight, "0", QFont("Helvetica", 5, 1));
    drawText(scaleLeftTopX, scaleLeftTopY + textOffset + 3 * regionHeight, "-0.5", QFont("Helvetica", 5, 1));
    drawText(scaleLeftTopX, scaleLeftTopY + 4 * regionHeight, "-1.0", QFont("Helvetica", 5, 1));
}

void Viewer::DrawGrid() {
    float centerX = (min_x_ + max_x_) / 2;
    float centerY = (min_y_ + max_y_) / 2;
    float centerZ = (min_z_ + max_z_) / 2;

    const float squareSide = 0.1;

    const float xLeftWidth = abs(centerX - min_x_);
    const float xRightWidth = abs(centerX - max_x_);

    const float yLeftWidth = abs(centerY - min_y_);
    const float yRightWidth = abs(centerY - max_y_);

    const float zLeftWidth = abs(centerZ - min_z_);
    const float zRightWidth = abs(centerZ - max_z_);

    const int xLeftRows =  static_cast<int>(ceil(xLeftWidth / squareSide));
    const int xRightRows = static_cast<int>(ceil(xRightWidth / squareSide));

    const int yLeftRows =  static_cast<int>(ceil(yLeftWidth / squareSide));
    const int yRightRows = static_cast<int>(ceil(yRightWidth / squareSide));

    const int zLeftRows =  static_cast<int>(ceil(zLeftWidth / squareSide));
    const int zRightRows = static_cast<int>(ceil(zRightWidth / squareSide));

    float minGridX = centerX - xLeftRows * squareSide;
    float maxGridX = centerX + xRightRows * squareSide;

    float minGridY = centerY - yLeftRows * squareSide;
    float maxGridY = centerY + yRightRows * squareSide;

    float minGridZ = centerZ - zLeftRows * squareSide;
    float maxGridZ = centerZ + zRightRows * squareSide;

    float currY = centerY - yLeftRows * squareSide;
    float currX = centerX - xLeftRows * squareSide;
    float currZ = centerZ - zLeftRows * squareSide;

    const int CELLS_MAX = 100;
    int xSum = xLeftRows + xRightRows;
    int ySum = yLeftRows + yRightRows;
    int zSum = zLeftRows + zRightRows;

    const int yTotal = (ySum <= CELLS_MAX) ? ySum : CELLS_MAX;
    const int xTotal = (xSum <= CELLS_MAX) ? xSum : CELLS_MAX;
    const int zTotal = (zSum <= CELLS_MAX) ? zSum : CELLS_MAX;
    const float epsilon = 0.00001;

    glColor3f(0.45, 0.45, 0.45);
    glBegin(GL_LINES);
    {
        for (int i = 0; i <= yTotal; ++i){
            if (abs(currY - centerY) <= epsilon) { currY += squareSide; continue; }
            glVertex3f(minGridX, currY, min_z_);
            glVertex3f(maxGridX, currY, min_z_);
            currY += squareSide;
        }

        for (int i = 0; i <= xTotal; ++i){
            if (abs(currX - centerX) <= epsilon) { currX += squareSide; continue; }
            glVertex3f(currX, minGridY, min_z_);
            glVertex3f(currX, maxGridY, min_z_);
            currX += squareSide;
        }

        currX = centerX - xLeftRows * squareSide;
        for (int i = 0; i <= xTotal; ++i){
            glVertex3f(currX, min_y_, minGridZ);
            glVertex3f(currX, min_y_, maxGridZ);
            currX += squareSide;
        }

        for (int i = 0; i <= zTotal; ++i){
            glVertex3f(minGridX, min_y_, currZ);
            glVertex3f(maxGridX, min_y_, currZ);
            currZ += squareSide;
        }

        currY = centerY - yLeftRows * squareSide;
        for (int i = 0; i <= yTotal; ++i){
            glVertex3f(min_x_, currY, minGridZ);
            glVertex3f(min_x_, currY, maxGridZ);
            currY += squareSide;
        }

        currZ = centerZ - zLeftRows * squareSide;
        for (int i = 0; i <= zTotal; ++i){
            glVertex3f(min_x_, minGridY, currZ);
            glVertex3f(min_x_, maxGridY, currZ);
            currZ += squareSide;
        }

        glColor3f(1, 1, 1);
        {
            glVertex3f(centerX, minGridY, min_z_);
            glVertex3f(centerX, maxGridY, min_z_);
        }
        {
            glVertex3f(minGridX, centerY, min_z_);
            glVertex3f(maxGridX, centerY, min_z_);
        }
    }

    glEnd();

    {
        currX = maxGridX;
        for (int i = 0; i < xTotal; ++i){
            renderText(currX, minGridY, min_z_, QString::number(round(currX * 100) / 100.0), QFont("Helvetica", 5));
            currX -= squareSide;
        }
    }

    {
        currY = maxGridY;
        for (int i = 0; i < yTotal; ++i){
            renderText(minGridX, currY, min_z_, QString::number(round(currY * 100) / 100.0), QFont("Helvetica", 5));
            currY -= squareSide;
        }
    }

    {
        currZ = minGridZ;
        for (int i = 0; i <= zTotal; ++i){
            renderText(minGridX, minGridY, currZ, QString::number(round(currZ * 100) / 100.0), QFont("Helvetica", 5));
            currZ += squareSide;
        }
    }
}

void Viewer::DrawNormals()
{
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    for (auto& point : point_cloud_){
        glVertex3f(point.x, point.y, point.z);
        glVertex3f(point.n_x + point.x, point.n_y + point.y, point.n_z + point.z);
    }
    glEnd();
}

void Viewer::draw() {

    if (draw_surface_){

        //HardCoded Parallelepiped Surface Drawing
        vector<GeneratedPoint> lowerPlane = { {0.2, 0.2, -0.99}, {0.2, 0.6, -0.99}, {0.6, 0.6, -0.99}, {0.6, 0.2, -0.99} };
        vector<GeneratedPoint> lowerMiddlePlane = { {0.2, 0.2, -0.5}, {0.2, 0.6, -0.5}, {0.6, 0.6, -0.5}, {0.6, 0.2, -0.5} };
        vector<GeneratedPoint> middlePlane = { {0.2, 0.2, 0.0}, {0.2, 0.6, 0.0}, {0.6, 0.6, 0.0}, {0.6, 0.2, 0.0} };
        vector<GeneratedPoint> upperMiddlePlane = { {0.2, 0.2, 0.5}, {0.2, 0.6, 0.5}, {0.6, 0.6, 0.5}, {0.6, 0.2, 0.5} };
        vector<GeneratedPoint> upperPlane = { {0.2, 0.2, 0.99}, {0.2, 0.6, 0.99}, {0.6, 0.6, 0.99}, {0.6, 0.2, 0.99} };
        vector<GeneratedPoint> inputPoints;

        array<vector<GeneratedPoint>, 5> planes = { lowerPlane, lowerMiddlePlane, middlePlane, upperMiddlePlane, upperPlane};

        for (size_t i = 0; i < planes.size() - 1; ++i){
            inputPoints.clear();

            auto& low = planes[i];
            auto& up = planes[i + 1];
            for_each(begin(low), end(low), [](GeneratedPoint& point)->void{
                point.n_x = 0.0;
                point.n_y = 0.0;
                point.n_z = -1.0;
            });

            for_each(begin(up), end(up), [](GeneratedPoint& point)->void{
                point.n_x = 0.0;
                point.n_y = 0.0;
                point.n_z = 1.0;
            });

            copy(begin(low), end(low), back_inserter(inputPoints));
            copy(begin(up), end(up), back_inserter(inputPoints));

            cout << "Starting algorithm" << endl;
            auto triangles = DoBallPivotingAlgorithm(inputPoints, 0.6);
            cout << "Ending algorithm" << endl;

            glBegin(GL_TRIANGLES);
            for (auto& triangle : triangles){
                for (int i = 0; i < 3; ++i){
                    GeneratedColor color = GetColorByZ(triangle[i].z);
                    glColor3f(color.r, color.g, color.b);
                    glVertex3f(triangle[i].x, triangle[i].y, triangle[i].z);
                }
            }
            glEnd();
        }

    } else {
        glBegin(GL_POINTS);
        for (auto& point : point_cloud_){
            GeneratedColor color = GetColorByZ(point.z);
            glColor3f(color.r, color.g, color.b);
            glVertex3f(point.x, point.y, point.z);
        }
        glEnd();
    }

    if (draw_grid_){
        DrawGrid();
    }

    if (draw_normals_){
        DrawNormals();
    }

    if (draw_scale_){
        DrawScale();
    }
}

void Viewer::init() {
    restoreStateFromFile();
    //help();
}

GeneratedColor Viewer::GetColorByZ(float z)
{
    const int range = 1'000;
    const float zIncrement = 0.001;
    if (z < 0){

        const float startBlue = 0.0f;
        const float endBlue = 1.0f;
        const float startGreen = 1.0f;
        const float endGreen = 0.0f;

        z = abs(z);

        float newBlue  = startBlue  + (endBlue  - startBlue ) * z / zIncrement / (range / 2);

        if (z <= 0.5){
            return {0.0, 1.0, newBlue};
        }

        float newGreen = startGreen + (endGreen - startGreen) * (z / zIncrement - 500) / (range / 2);

        return {0.0, newGreen, 1.0};

    } else{
        const float startRed = 0.0f;
        const float endRed = 1.0f;
        const float startGreen = 1.0f;
        const float endGreen = 0.0f;

        float newRed  = startRed  + (endRed  - startRed ) * z / zIncrement / (range / 2);

        if (z <= 0.5){
            return {newRed, 1.0, 0.0};
        }

        float newGreen = startGreen + (endGreen - startGreen) * (z / zIncrement - 500) / (range / 2);

        return {1.0, newGreen, 0.0};
    }

}


QString Viewer::helpString() const {
    QString text("<h2>S i m p l e V i e w e r</h2>");
    text += "Use the mouse to move the camera around the object. ";
    text += "You can respectively revolve around, zoom and translate with the "
            "three mouse buttons. ";
    text += "Left and middle buttons pressed together rotate around the camera "
            "view direction axis<br><br>";
    text += "Pressing <b>Alt</b> and one of the function keys "
            "(<b>F1</b>..<b>F12</b>) defines a camera keyFrame. ";
    text += "Simply press the function key again to restore it. Several "
            "keyFrames define a ";
    text += "camera path. Paths are saved when you quit the application and "
            "restored at next start.<br><br>";
    text +=
            "Press <b>F</b> to display the frame rate, <b>A</b> for the world axis, ";
    text += "<b>Alt+Return</b> for full screen mode and <b>Control+S</b> to save "
            "a snapshot. ";
    text += "See the <b>Keyboard</b> tab in this window for a complete shortcut "
            "list.<br><br>";
    text += "Double clicks automates single click actions: A left button double "
            "click aligns the closer axis with the camera (if close enough). ";
    text += "A middle button double click fits the zoom of the camera and the "
            "right button re-centers the scene.<br><br>";
    text += "A left button double click while holding right button pressed "
            "defines the camera <i>Revolve Around Point</i>. ";
    text += "See the <b>Mouse</b> tab and the documentation web pages for "
            "details.<br><br>";
    text += "Press <b>Escape</b> to exit the viewer.";
    return text;
}
