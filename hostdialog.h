#ifndef HOSTDIALOG_H
#define HOSTDIALOG_H

#include "hinfo.h"
#include <QDialog>
#include <QMainWindow>

namespace Ui {
  class hostDialog;
}

class hostDialog : public QDialog
{
	Q_OBJECT

  public:
	explicit hostDialog(QWidget *parent = 0);
	~hostDialog();
	void setHosts(hInfo);

  private slots:
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();

  private:
	Ui::hostDialog *ui;
	hInfo h;

  signals:
	void hostsChanged(hInfo);
};

#endif // HOSTDIALOG_H
