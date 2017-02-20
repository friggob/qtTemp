#ifndef GRAPHDIALOG_H
#define GRAPHDIALOG_H

#include <QDialog>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>

namespace Ui {
  class graphDialog;
}

class graphDialog : public QDialog
{
	Q_OBJECT

  public:
	explicit graphDialog(QWidget *parent = 0);
	void setPixmap(QPixmap *);
	void setRrdPath(QString);
	void setRrdCmd(QString);
	~graphDialog();

  private slots:
	void on_actionClose_triggered();
	void updateView();
	void resizeEvent(QResizeEvent *);
	void createPixmap();
	void on_dayButton_clicked(bool checked);
	void on_hourButton_clicked(bool checked);
	void on_weekButton_clicked(bool checked);
	void on_monthButton_clicked(bool checked);
	void on_yearButton_clicked(bool checked);

	void on_spinBox_valueChanged(int arg1);

private:
	Ui::graphDialog *ui;
	QPixmap *qpm;
	QString rrdPath;
	QString rrdCmd;
	const int hour = 3600;
	const int day = 24*hour;
	const int week = 7*day;
	const int month = 30*day;
	const int year = 365*day;
	int offset;
	void showEvent(QShowEvent *);

  signals:
	void rrdpathChanged();
	void pixmapChanged();
	void offsetChanged();
};

#endif // GRAPHDIALOG_H
