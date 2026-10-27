#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <fstream>
#include <iostream>
#include <string>


std::string file = "C:\\Users\\WildJ\\Documents\\CoordinateManager\\coord2.txt";
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    std::ifstream f(file);
    qDebug() << "file: " << file;
    std::string text;


    QLineSeries *series = new QLineSeries;

    //series -> append(0,0);
    //*series << QPointF(0,1)<< QPointF(0,11)<< QPointF(10,1)<< QPointF(13,13);
    while (getline (f, text)) {
        // Output the text from the file
        *series << QPointF(text[0],text[1]);
    }

    QChart *chart = new QChart;
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Line Chart");

    QChartView *chartVeiw = new QChartView(chart);
    chartVeiw->setRenderHint(QPainter::Antialiasing);
    chartVeiw->setParent(ui->horizontalFrame);
}

MainWindow::~MainWindow()
{

    delete ui;

}
void MainWindow::on_pushButton_clicked(){
    qDebug() << "clicked";
    file = QFileDialog::getOpenFileName(this, tr("Select File"), QCoreApplication::applicationDirPath()).toStdString();

    MainWindow();


    //copy pasted from MainWindow
    ui->setupUi(this);

    std::ifstream f(file);
    std::string text;


    QLineSeries *series = new QLineSeries;

    while (getline (f, text)) {
        // Output the text from the file
        qDebug() << text;
        qDebug() << text[0];
        float x=0;
        float y=0;
        std::string xs="";
        std::string ys="";
        int i = 0;
        //getting strings of numerals
        //This doesn't work for some reason
        /*for(int u = 0;text[i]==0 || text[i]==1 || text[i]==2 ||text[i]==3 || text[i]==4 || text[i]==5 ||text[i]==6 || text[i]==7 || text[i]==8 ||text[i]==9; i++){
            xs+=text[i];
            qDebug() << text[i];
        }

        while(!(text[i]==0 || text[i]==1 || text[i]==2 ||text[i]==3 || text[i]==4 || text[i]==5 ||text[i]==6 || text[i]==7 || text[i]==8 ||text[i]==9)){
            i++;
        }

        for(int u=0;text[i]==0 || text[i]==1 || text[i]==2 ||text[i]==3 || text[i]==4 || text[i]==5 ||text[i]==6 || text[i]==7 || text[i]==8 ||text[i]==9; i++){
            ys+=text[i];
        }*/
        bool second = false;
        for(int i = 0; i<text.length(); i++){
            if(text[i]==','){
                second = true;
            }
            else{
                if(second){

                    ys.append(1,text[i]);
                }
                else{
                    xs.append(1,text[i]);
                }
            }
        }
        //reading strings into ints
        //I don't know why this doesn't work
        for (int i = xs.length(); i>= 0; i--){
            x+=xs[i]*(10^(xs.length()-i));
        }
        for (int i = ys.length(); i>= 0; i--){
            y+=ys[i]*(10^(ys.length()-i));
        }

        qDebug() << "ys="+ys;

        //qDebug() << QPointF(x,y);
        *series << QPointF(x,y);

    }

/*  Reads the first 2 characters on each line and assumes they are numerals
    //series -> append(0,0);
    //*series << QPointF(0,1)<< QPointF(0,11)<< QPointF(10,1)<< QPointF(13,13);
    while (getline (f, text)) {
        // Output the text from the file
        *series << QPointF(text[0],text[1]);
    }
*/
    QChart *chart = new QChart;
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Line Chart");

    QChartView *chartVeiw = new QChartView(chart);
    chartVeiw->setRenderHint(QPainter::Antialiasing);
    chartVeiw->setParent(ui->horizontalFrame);

}
