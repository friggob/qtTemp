#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "hinfo.h"
#include "hostdialog.h"
#include <QMainWindow>
#include <QTcpSocket>
#include <QSettings>
#include <QMouseEvent>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

  public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

  public slots:
	void updateHosts(hInfo);

  private:
	Ui::MainWindow *ui;

	hostDialog *hd;

	void setTemp(void);
	//void get_temp();
	void readConfig();
	void createMenu();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

	int m_nMouseClick_X_Coordinate;
	int m_nMouseClick_Y_Coordinate;

	hInfo hi;
	QPalette dp;
	QPalette ep;
	QString last_temp;
	QString temp;
	QSettings *cSettings;
	QTcpSocket *socket;
	QString version = "0.14 BETA";

/*
  protected:
	void mouseReleaseEvent(QMouseEvent* event);
*/

  private slots:
	void on_action_Quit_triggered();
	void on_action_Update_triggered();
	void getTempNet();
	void on_action_Change_triggered();
	void testNet();
	void on_actionSet_host_triggered();
	void on_actionSavecfg_triggered();
	void on_actionPrint_triggered();
	void on_action_About_triggered();
};

#endif // MAINWINDOW_H
