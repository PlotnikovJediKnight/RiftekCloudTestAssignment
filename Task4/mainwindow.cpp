#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "BallPivotingAlgorithm.h"
#include <array>
#include <tuple>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Open File
void MainWindow::on_pushButton_clicked()
{
    QString textFilter = tr("Text Files (*.txt)");
    QString fileName = QFileDialog::getOpenFileName(
                this,
                "Read Point Cloud Data",
                QString(),
                textFilter,
                &textFilter);

    QFile inputFile(fileName);
    inputFile.open(QIODevice::ReadOnly);
    QTextStream in(&inputFile);

    vector<GeneratedPoint> points;
    while (!in.atEnd()){
        QString line = in.readLine();
        QStringList list = line.split("/", Qt::SkipEmptyParts);
        QStringList pointList = list[0].split(";", Qt::SkipEmptyParts);
        QStringList normalList = list[1].split(";", Qt::SkipEmptyParts);

        points.push_back({pointList[0].toFloat(), pointList[1].toFloat(), pointList[2].toFloat(),
                          normalList[0].toFloat(), normalList[1].toFloat(), normalList[2].toFloat()});
    }

    inputFile.close();

    static_cast<Viewer*>(ui->openGLWidget)->SetPointCloud(points);
}

//Generate Parallelepiped Data
void MainWindow::on_pushButton_3_clicked()
{
    const float squareSideIncrement = 0.002666667;
    const size_t oneSidePointsCount = 149;
    const float zMinValue = -1.0;
    const float zMaxValue =  1.0;
    const float zIncrement = 0.001 * 1.0000000161290;

    const size_t POINTS_COUNT = 1'000'000;

    vector<GeneratedPoint> points;
    points.reserve(POINTS_COUNT);

    std::array<tuple<float, float, float, float>, 4> startingPoints
            = { tuple<float, float, float, float>{0.2, 0.6, squareSideIncrement, 0.0},
                tuple<float, float, float, float>{0.6, 0.6, 0.0, -squareSideIncrement},
                tuple<float, float, float, float>{0.6, 0.2, -squareSideIncrement, 0.0},
                tuple<float, float, float, float>{0.2, 0.2, 0.0, squareSideIncrement} };

    std::array<pair<float,float>, 4> normals
            = { pair<float, float>{0.0, 1.0},
                pair<float, float>{1.0, 0.0},
                pair<float, float>{0.0, -1.0},
                pair<float, float>{-1.0, 0.0} };

    float currZ = zMinValue;
    while (currZ <= zMaxValue){

        for(size_t i = 0; i < startingPoints.size(); ++i){
            float x = get<0>(startingPoints[i]);
            float y = get<1>(startingPoints[i]);
            float xIncrement = get<2>(startingPoints[i]);
            float yIncrement = get<3>(startingPoints[i]);

            for (size_t j = 0; j < oneSidePointsCount; ++j){
                float n_x = normals[i].first;
                float n_y = normals[i].second;
                float n_z = 0;

                float vectorLength = sqrt(n_x * n_x + n_y * n_y);
                n_x /= vectorLength; n_y /= vectorLength;


                points.push_back({
                                     x,
                                     y,
                                     currZ,
                                     n_x,
                                     n_y,
                                     n_z
                                 });

                x += xIncrement;
                y += yIncrement;
            }
        }
        currZ += zIncrement;
    }

    QString textFilter = tr("Text Files (*.txt)");
    QString fileName = QFileDialog::getSaveFileName(
                this,
                "Save Ellipse Like Cylinder Data",
                QString(),
                textFilter,
                &textFilter);

    QFile outputFile(fileName);
    outputFile.open(QIODevice::WriteOnly);
    QTextStream out(&outputFile);

    for (auto& point : points){
        out << point.x << ";" << point.y << ";" << point.z << ";/" << point.n_x << ";" << point.n_y << ";" << point.n_z << ";\n";
    }

    outputFile.close();
}

//Scale
void MainWindow::on_checkBox_stateChanged(int state)
{
    Viewer* viewer = static_cast<Viewer*>(ui->openGLWidget);
    if (state == Qt::Unchecked){
        viewer->SetDrawScale(false);
    } else if (state == Qt::Checked){
        viewer->SetDrawScale(true);
    }
}

//Grid
void MainWindow::on_checkBox_2_stateChanged(int state)
{
    Viewer* viewer = static_cast<Viewer*>(ui->openGLWidget);
    if (state == Qt::Unchecked){
        viewer->SetDrawGrid(false);
    } else if (state == Qt::Checked){
        viewer->SetDrawGrid(true);
    }
}

//Normals
void MainWindow::on_checkBox_4_stateChanged(int state)
{
    Viewer* viewer = static_cast<Viewer*>(ui->openGLWidget);
    if (state == Qt::Unchecked){
        viewer->SetDrawNormals(false);
    } else if (state == Qt::Checked){
        viewer->SetDrawNormals(true);
    }
}

//Surface
void MainWindow::on_checkBox_3_stateChanged(int state)
{
    Viewer* viewer = static_cast<Viewer*>(ui->openGLWidget);
    if (state == Qt::Unchecked){
        viewer->SetDrawSurface(false);
    } else if (state == Qt::Checked){
        viewer->SetDrawSurface(true);
    }
}

