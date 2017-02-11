#ifndef DEBUGFORM_H
#define DEBUGFORM_H

#include "hinfo.h"
#include <QDialog>

namespace Ui {
  class debugForm;
}

class debugForm : public QDialog
{
	Q_OBJECT

  public:
	explicit debugForm(QWidget *parent = 0);
	~debugForm();

  public slots:
	void setData(hInfo);

  private slots:
	void on_actionQuit_triggered();

  private:
	Ui::debugForm *ui;
};

#endif // DEBUGFORM_H
