#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QVector>
#include <qwt_plot_curve.h>
#include <QFile>
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <qwt_legend.h>


#include <QTime>
#include <QTimer>


#include <qwt_scale_draw.h>
#include <qwt_date_scale_draw.h>

#include <qwt_symbol.h>






MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // connect(ui->DryDc,SIGNAL(stateChanged(int)),this,SLOT(on_DryDc_clicked()));
    // connect(ui->Dry2FRef1Pdry,SIGNAL(stateChanged(int)),this,SLOT(on_DryDcRef1_clicked()));
    // connect(ui->RawDry2F,SIGNAL(stateChanged(int)),this,SLOT(on_RawDry2F_clicked()));
    // connect(ui->Dry2F,SIGNAL(stateChanged(int)),this,SLOT(on_Dry2F_clicked()));
    setWindowFlags(Qt::FramelessWindowHint);
    //ui->plot->setTitle("气体曲线分析");


   // QDateTime m_baseTime = QDateTime::currentDateTime();
   //  ui->plot->setAxisScaleDraw(QwtPlot::xBottom,new TimeScaleDrawR(m_baseTime));

 //设置画布
     QwtPlotCanvas *canvas = new QwtPlotCanvas();
     //画布的背景颜色
     canvas->setPalette( Qt::white);
     //四个角，半圆 半径为10
     canvas->setBorderRadius(10);

     ui->plot->setCanvas(canvas);




     QwtPlotGrid* grid = new QwtPlotGrid();
     grid->setPen(QColor(222,222,222),1);
     grid->attach(ui->plot);

    curve1=new QwtPlotCurve();
    curve1->setPen(Qt::blue,2);
    curve1->setSamples(time,val,10);
    curve1->setRenderHint(QwtPlotItem::RenderAntialiased,true);//使线条光滑
    curve1->attach(ui->plot);

    //启动定时器，每1s更新一次
    this->startTimer(1000);  //单位是毫秒


}


 TimeScaleDrawR::TimeScaleDrawR(const QDateTime &base):  m_scaleBaseTime(base){ }

 QwtText TimeScaleDrawR::label(double v) const

  {
     QDateTime upTime = m_scaleBaseTime.addSecs((int )v);  //addSecs增加秒数
     return upTime.toString("yyyy-MM-dd\nhh:mm:ss");
  }


void MainWindow::timerEvent(QTimerEvent *)
{
    for(int i=0;i<9;i++)
    {
        val[i]=val[i+1];
    }


    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));  //随机函数的初始化
    val[9]=qrand()%10;                                   //产生10以内的随机数


    curve1->setSamples(time,val,10);                           //重新加载数据
    curve1->setRenderHint(QwtPlotItem::RenderAntialiased,true);//使线条光滑




    QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
    QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 6, 6) );//设置样本点的颜色、大小
    curve1->setSymbol( symbol );//添加样本点形状


    ui->plot->replot();

}
MainWindow::~MainWindow()
{
    delete ui;
}

/*void MainWindow::on_DryDc_clicked()
{


      ui->plot->setAxisTitle(QwtPlot::xBottom,"t(ms)");
      ui->plot->setAxisTitle(QwtPlot::yLeft,"Dry Dc");
      ui->plot->setAxisScale(QwtPlot::xBottom,-0.0,50.0);
      ui->plot->setAxisScale(QwtPlot::yLeft,-0.05,1.05);



      QVector<double>x;
      QVector<double>y;
      for(double i=0;i<50.0;i++)
      {
          x.append(i);
      }

       QFile fpy("D:\\data1.txt");
       if(!fpy.open(QIODevice::ReadOnly))     //readonly只读，QFile文本文件
          {
              return;
          }

       int i=0,k=0;

       while(!fpy.atEnd())
          {

            QString lineString_y=QString(fpy.readLine()).trimmed();

            y<<(k++,lineString_y.toDouble());

       }

       for(i=0;i<50;i++)
           {

             qDebug()<<y[i];

          }


      //设置画布
       QwtPlotCanvas *canvas = new QwtPlotCanvas();
       canvas->setPalette( Qt::white);
       canvas->setBorderRadius( 10 );
       ui->plot->setCanvas(canvas);

       //设置栅格线（网格线）
      QwtPlotGrid*gird=new QwtPlotGrid;

      //两种网格线的方法（一种是以小方格为单位，一种是以大网格为单位）
      //gird->enableX(true);
      //gird->enableY(true);
      // gird->setMajorPen(Qt::black,0,Qt::DotLine);

      gird->enableXMin(true);
      gird->setMajorPen( Qt::gray, 0, Qt::DotLine );
      gird->setMinorPen( Qt::darkGray, 0, Qt::DotLine );

      gird->attach(ui->plot);
      //开始绘图
      QwtPlotCurve*curve1=new QwtPlotCurve();
      curve1->setStyle(QwtPlotCurve::Lines);//直线形式
      curve1->setCurveAttribute(QwtPlotCurve::Fitted,true);//使曲线更加光滑
      curve1->setPen(QPen(Qt::green));//设置画笔
      curve1->setSamples(x,y);
      curve1->attach(ui->plot);//把曲线附加在plot上
      ui->plot->replot();
     }

     else
     {


     }
}

void MainWindow::on_Dry2FRef1Pdry_clicked()
{
    if(ui->Dry2FRef1Pdry->isChecked())
      {

      ui->plot->setAxisTitle(QwtPlot::xBottom,"t(ms)");
      ui->plot->setAxisTitle(QwtPlot::yLeft,"Dry Dc Re f1");
      ui->plot->setAxisScale(QwtPlot::xBottom,-0.0,50.0);
      ui->plot->setAxisScale(QwtPlot::yLeft,-0.05,1.05);

      QVector<double>x;
      QVector<double>y;
      for(double i=0;i<50.0;i++)
      {
          x.append(i);
      }

       QFile fpy("D:\\data2.txt");
       if(!fpy.open(QIODevice::ReadOnly))     //readonly只读，QFile文本文件
          {
              return;
          }

       int i=0,k=0;

       while(!fpy.atEnd())
          {

            QString lineString_y=QString(fpy.readLine()).trimmed();

            y<<(k++,lineString_y.toDouble());

       }

       for(i=0;i<50;i++)
           {

             qDebug()<<y[i];

          }

       //设置栅格线
      QwtPlotGrid*gird=new QwtPlotGrid;
      gird->enableX(true);
      gird->enableY(true);
      gird->setMajorPen(Qt::black,0,Qt::DotLine);
      gird->attach(ui->plot);
      //开始绘图
      QwtPlotCurve*curve2=new QwtPlotCurve();
      curve2->setStyle(QwtPlotCurve::Lines);//直线形式
      curve2->setCurveAttribute(QwtPlotCurve::Fitted,true);//使曲线更加光滑
      curve2->setPen(QPen(Qt::black));//设置画笔
      curve2->setSamples(x,y);
      curve2->attach(ui->plot);//把曲线附加在plot上
      ui->plot->replot();
     }

     else
     {


     }
}


void MainWindow::on_RawDry2F_clicked()
{
    if(ui->RawDry2F->isChecked())
      {

       ui->plot->setAxisTitle(QwtPlot::xBottom,"t(ms)");
       ui->plot->setAxisTitle(QwtPlot::yLeft,"Raw Dry 2F");
       ui->plot->setAxisScale(QwtPlot::xBottom,-0.0,50.0);
       ui->plot->setAxisScale(QwtPlot::yLeft,-0.05,1.05);
       QVector<double>x;
       QVector<double>y;
       for(double i=0;i<50.0;i++)
       {
          x.append(i);
        }

        QFile fpy("D:\\data3.txt");
        if(!fpy.open(QIODevice::ReadOnly))     //readonly只读，QFile文本文件
          {
              return;
          }

       int i=0,k=0;

       while(!fpy.atEnd())
          {

            QString lineString_y=QString(fpy.readLine()).trimmed();

            y<<(k++,lineString_y.toDouble());

       }

       for(i=0;i<50;i++)
           {

             qDebug()<<y[i];

          }

       //设置栅格线
      QwtPlotGrid*gird=new QwtPlotGrid;
      gird->enableX(true);
      gird->enableY(true);
      gird->setMajorPen(Qt::black,0,Qt::DotLine);
      gird->attach(ui->plot);
      //开始绘图
      QwtPlotCurve*curve3=new QwtPlotCurve();
      curve3->setStyle(QwtPlotCurve::Lines);//直线形式
      curve3->setCurveAttribute(QwtPlotCurve::Fitted,true);//使曲线更加光滑
      curve3->setPen(QPen(Qt::red));//设置画笔
      curve3->setSamples(x,y);
      curve3->attach(ui->plot);//把曲线附加在plot上
      ui->plot->replot();
     }

     else
     {


     }

}

void MainWindow::on_Dry2F_clicked()
{
    if(ui->Dry2F->isChecked())
      {

      ui->plot->setAxisTitle(QwtPlot::xBottom,"t(ms)");
      ui->plot->setAxisTitle(QwtPlot::yLeft,"Dry 2F");
      ui->plot->setAxisScale(QwtPlot::xBottom,-0.0,50.0);
      ui->plot->setAxisScale(QwtPlot::yLeft,-0.05,1.05);
      QVector<double>x;
      QVector<double>y;
      for(double i=0;i<50.0;i++)
      {
          x.append(i);
      }

       QFile fpy("D:\\data4.txt");
       if(!fpy.open(QIODevice::ReadOnly))     //readonly只读，QFile文本文件
          {
              return;
          }

       int i=0,k=0;

       while(!fpy.atEnd())
          {

            QString lineString_y=QString(fpy.readLine()).trimmed();

            y<<(k++,lineString_y.toDouble());

       }

       for(i=0;i<50;i++)
           {

             qDebug()<<y[i];

          }


       //设置栅格线
      QwtPlotGrid*gird=new QwtPlotGrid;
      gird->enableX(true);
      gird->enableY(true);
      gird->setMajorPen(Qt::black,0,Qt::DotLine);
      gird->attach(ui->plot);
      //开始绘图
      QwtPlotCurve*curve4=new QwtPlotCurve();
      curve4->setStyle(QwtPlotCurve::Lines);//直线形式
      curve4->setCurveAttribute(QwtPlotCurve::Fitted,true);//使曲线更加光滑
      curve4->setPen(QPen(Qt::blue));//设置画笔
      curve4->setSamples(x,y);
      curve4->attach(ui->plot);//把曲线附加在plot上
      ui->plot->replot();

     }

     else
     {

      }
}

*/
/*void MainWindow::on_clean_btn_clicked()
{
   ui->plot->detachItems();
}
*/

void MainWindow::on_clean_btn_clicked()
{
   ui->plot->detachItems();

}


void MainWindow::on_showdata_activated(const QString &arg1)
{
     int index=ui->showdata->currentIndex();
     if(index==0)
     {
         ui->plot1->clear();
         ui->plot2->clear();
         ui->plot3->clear();
         ui->plot4->clear();
         ui->plot1->addItem("DryDc");
         ui->plot1->addItem("DryDCRef1");
         ui->plot1->addItem("RawDry2F");
         ui->plot1->addItem("Dry2F");
         ui->plot1->addItem("Dry2FRef1Pdry");
         ui->plot1->addItem("DryDCRef1");
         ui->plot1->addItem("WetDC");
         ui->plot1->addItem("WetDCRef1");
         ui->plot1->addItem("RawWet2F");
         ui->plot1->addItem("Wet2F");
         ui->plot1->addItem("Wet2FRef1PwetFR");
         ui->plot1->addItem("2F");
         ui->plot1->addItem("Ref1");
         ui->plot1->addItem("Ref2");
         ui->plot1->addItem("Ref3");
         ui->plot1->addItem("Dry2FRef1Pwet");
         ui->plot1->addItem("Ref4");
         ui->plot1->addItem("Ref5");
         ui->plot1->addItem("Ref0");
         ui->plot1->addItem("ValRef");
         ui->plot1->addItem("ValRefDryPdry");
         ui->plot1->addItem("ValRefWetPwetFR");
         ui->plot1->addItem("CompositeRef");


         ui->plot2->addItem("DryDc");
         ui->plot2->addItem("DryDCRef1");
         ui->plot2->addItem("RawDry2F");
         ui->plot2->addItem("Dry2F");
         ui->plot2->addItem("Dry2FRef1Pdry");
         ui->plot2->addItem("DryDCRef1");
         ui->plot2->addItem("WetDC");
         ui->plot2->addItem("WetDCRef1");
         ui->plot2->addItem("RawWet2F");
         ui->plot2->addItem("Wet2F");
         ui->plot2->addItem("Wet2FRef1PwetFR");
         ui->plot2->addItem("2F");
         ui->plot2->addItem("Ref1");
         ui->plot2->addItem("Ref2");
         ui->plot2->addItem("Ref3");
         ui->plot2->addItem("Dry2FRef1Pwet");
         ui->plot2->addItem("Ref4");
         ui->plot2->addItem("Ref5");
         ui->plot2->addItem("Ref0");
         ui->plot2->addItem("ValRef");
         ui->plot2->addItem("ValRefDryPdry");
         ui->plot2->addItem("ValRefWetPwetFR");
         ui->plot2->addItem("CompositeRef");

         ui->plot3->addItem("DryDc");
         ui->plot3->addItem("DryDCRef1");
         ui->plot3->addItem("RawDry2F");
         ui->plot3->addItem("Dry2F");
         ui->plot3->addItem("Dry2FRef1Pdry");
         ui->plot3->addItem("DryDCRef1");
         ui->plot3->addItem("WetDC");
         ui->plot3->addItem("WetDCRef1");
         ui->plot3->addItem("RawWet2F");
         ui->plot3->addItem("Wet2F");
         ui->plot3->addItem("Wet2FRef1PwetFR");
         ui->plot3->addItem("2F");
         ui->plot3->addItem("Ref1");
         ui->plot3->addItem("Ref2");
         ui->plot3->addItem("Ref3");
         ui->plot3->addItem("Dry2FRef1Pwet");
         ui->plot3->addItem("Ref4");
         ui->plot3->addItem("Ref5");
         ui->plot3->addItem("Ref0");
         ui->plot3->addItem("ValRef");
         ui->plot3->addItem("ValRefDryPdry");
         ui->plot3->addItem("ValRefWetPwetFR");
         ui->plot3->addItem("CompositeRef");

         ui->plot4->addItem("DryDc");
         ui->plot4->addItem("DryDCRef1");
         ui->plot4->addItem("RawDry2F");
         ui->plot4->addItem("Dry2F");
         ui->plot4->addItem("Dry2FRef1Pdry");
         ui->plot4->addItem("DryDCRef1");
         ui->plot4->addItem("WetDC");
         ui->plot4->addItem("WetDCRef1");
         ui->plot4->addItem("RawWet2F");
         ui->plot4->addItem("Wet2F");
         ui->plot4->addItem("Wet2FRef1PwetFR");
         ui->plot4->addItem("2F");
         ui->plot4->addItem("Ref1");
         ui->plot4->addItem("Ref2");
         ui->plot4->addItem("Ref3");
         ui->plot4->addItem("Dry2FRef1Pwet");
         ui->plot4->addItem("Ref4");
         ui->plot4->addItem("Ref5");
         ui->plot4->addItem("Ref0");
         ui->plot4->addItem("ValRef");
         ui->plot4->addItem("ValRefDryPdry");
         ui->plot4->addItem("ValRefWetPwetFR");
         ui->plot4->addItem("CompositeRef");
 }

     else
     {
         ui->plot1->clear();
         ui->plot2->clear();
         ui->plot3->clear();
         ui->plot4->clear();
         ui->plot1->addItem("Concentration");
         ui->plot1->addItem("DryTemp");
         ui->plot1->addItem("DryPressure");
         ui->plot1->addItem("WetTemp");
         ui->plot1->addItem("WetPressure");
         ui->plot1->addItem("DryDC");
         ui->plot1->addItem("DryPKHeight");
         ui->plot1->addItem("WetDC");
         ui->plot1->addItem("WetPKHeight");
         ui->plot1->addItem("FitRatio");
         ui->plot1->addItem("FitRatio2");
         ui->plot1->addItem("FitRatio3");
         ui->plot1->addItem("FitRatio4");
         ui->plot1->addItem("FitRatio5");
         ui->plot1->addItem("FitRatioDry");
         ui->plot1->addItem("FitRatioDry-1");
         ui->plot1->addItem("FitRatioStrech");


         ui->plot2->clear();
         ui->plot2->addItem("Concentration");
         ui->plot2->addItem("DryTemp");
         ui->plot2->addItem("DryPressure");
         ui->plot2->addItem("WetTemp");
         ui->plot2->addItem("WetPressure");
         ui->plot2->addItem("DryDC");
         ui->plot2->addItem("DryPKHeight");
         ui->plot2->addItem("WetDC");
         ui->plot2->addItem("WetPKHeight");
         ui->plot2->addItem("FitRatio");
         ui->plot2->addItem("FitRatio2");
         ui->plot2->addItem("FitRatio3");
         ui->plot2->addItem("FitRatio4");
         ui->plot2->addItem("FitRatio5");
         ui->plot2->addItem("FitRatioDry");
         ui->plot2->addItem("FitRatioDry-1");
         ui->plot2->addItem("FitRatioStrech");


         ui->plot3->clear();
         ui->plot3->addItem("Concentration");
         ui->plot3->addItem("DryTemp");
         ui->plot3->addItem("DryPressure");
         ui->plot3->addItem("WetTemp");
         ui->plot3->addItem("WetPressure");
         ui->plot3->addItem("DryDC");
         ui->plot3->addItem("DryPKHeight");
         ui->plot3->addItem("WetDC");
         ui->plot3->addItem("WetPKHeight");
         ui->plot3->addItem("FitRatio");
         ui->plot3->addItem("FitRatio2");
         ui->plot3->addItem("FitRatio3");
         ui->plot3->addItem("FitRatio4");
         ui->plot3->addItem("FitRatio5");
         ui->plot3->addItem("FitRatioDry");
         ui->plot3->addItem("FitRatioDry-1");
         ui->plot3->addItem("FitRatioStrech");


         ui->plot4->clear();
         ui->plot4->addItem("Concentration");
         ui->plot4->addItem("DryTemp");
         ui->plot4->addItem("DryPressure");
         ui->plot4->addItem("WetTemp");
         ui->plot4->addItem("WetPressure");
         ui->plot4->addItem("DryDC");
         ui->plot4->addItem("DryPKHeight");
         ui->plot4->addItem("WetDC");
         ui->plot4->addItem("WetPKHeight");
         ui->plot4->addItem("FitRatio");
         ui->plot4->addItem("FitRatio2");
         ui->plot4->addItem("FitRatio3");
         ui->plot4->addItem("FitRatio4");
         ui->plot4->addItem("FitRatio5");
         ui->plot4->addItem("FitRatioDry");
         ui->plot4->addItem("FitRatioDry-1");
         ui->plot4->addItem("FitRatioStrech");
     }
}





