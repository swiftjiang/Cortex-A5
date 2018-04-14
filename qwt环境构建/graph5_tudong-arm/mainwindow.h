#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

#include <qwt_plot_curve.h>
#include<QTimer>

#include <qwt_scale_draw.h>
#include <qwt_date_scale_draw.h>




namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int n;



private slots:

   /* void on_DryDc_clicked();

    void on_RawDry2F_clicked();

    void on_Dry2F_clicked();

    void on_clean_btn_clicked();

    void on_Dry2FRef1Pdry_clicked();
*/

    void on_clean_btn_clicked();

    void on_showdata_activated(const QString &arg1);

    void timerEvent(QTimerEvent *);

private:
    Ui::MainWindow *ui;

     QwtPlotCurve*curve1;

     double time[10]={1,2,3,4,5,6,7,8,9,10};
     //y轴的数据
     double val[10]={3,5,8,7,2,0,7,9,1,6};
};



class TimeScaleDrawR : public QwtScaleDraw
{


 public:

    TimeScaleDrawR(const QDateTime &base);

    virtual QwtText label(double v) const ;

 private:

    QDateTime m_scaleBaseTime;

};



#endif // MAINWINDOW_H
