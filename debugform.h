#ifndef DEBUGFORM_H
#define DEBUGFORM_H

#include <QDialog>
#include <QSettings>

namespace Ui {
  class debugForm;
}

class debugForm : public QDialog
{
	Q_OBJECT

  public:
	explicit debugForm(QWidget *parent = 0);
	~debugForm();
	void setData(QString org, QString app);

  private slots:
	void on_actionQuit_triggered();

  private:
	Ui::debugForm *ui;
	void showEvent(QShowEvent *);
	QSettings *settings;
};

#endif // DEBUGFORM_H
