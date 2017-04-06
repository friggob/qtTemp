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
	void setRrdFont(QString);
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
	void on_fontComboBox_activated(const QString &arg1);
	void on_pushButton_clicked();
	void on_dateTimeEditTo_dateTimeChanged(const QDateTime &dateTime);
	void on_dateTimeEditFrom_dateTimeChanged(const QDateTime &dateTime);

  private:
	Ui::graphDialog *ui;
	QPixmap *qpm;
	QString rrdPath;
	QString rrdCmd;
	QString rrdFont;
	const int hour = 3600;
	void showEvent(QShowEvent *);
	void setDateTimeEdit(qint64,qint64);

  signals:
	void rrdpathChanged();
	void pixmapChanged();
	void offsetChanged();
	void rrdFontChanged(QString);
};

#endif // GRAPHDIALOG_H
