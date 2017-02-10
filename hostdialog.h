#ifndef HOSTDIALOG_H
#define HOSTDIALOG_H

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
	void sethPi(const QString);
	void setpPi(const int);
	void sethTemp(const QString);
	void setpTemp(const int);

  private slots:
	void on_buttonBox_accepted();

	void on_buttonBox_rejected();

	void on_hPiEdit_returnPressed();

  private:
	Ui::hostDialog *ui;

  signals:
	void pPiChanged(int);
	void pTempChanged(int);
	void hTempChanged(QString);
	void hPiChanged(QString);
};

#endif // HOSTDIALOG_H
