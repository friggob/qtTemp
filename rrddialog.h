#ifndef RRDDIALOG_H
#define RRDDIALOG_H

#include <QDialog>

namespace Ui {
  class rrdDialog;
}

class rrdDialog : public QDialog
{
	Q_OBJECT

  public:
	explicit rrdDialog(QWidget *parent = 0);
	~rrdDialog();
	void setRrdPath(QString rp);
	void setRrdCmdPath(QString rcp);

  private slots:
	void on_rrdPathButton_clicked();
	void on_rrdCmdPathButton_clicked();
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();

  private:
	Ui::rrdDialog *ui;
	QString rrdPath;
	QString rrdCmdPath;

  signals:
	void rrdConfigChanged(QString rp,QString rcp);
};

#endif // RRDDIALOG_H
