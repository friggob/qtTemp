#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "hinfo.h"
#include "hostdialog.h"
#include "debugform.h"
#include "graphdialog.h"
#include <QMainWindow>
#include <QTcpSocket>
#include <QSettings>
#include <QMouseEvent>
#include <QVersionNumber>

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

  signals:
	void hInfoChanged(hInfo);


  private:
	Ui::MainWindow *ui;

	hostDialog *hd;
	debugForm *df;
	graphDialog *gd;

	void setTemp(void);
	void readConfig();
	void createMenu();
	void setupRrd();
	void updateRrd(QString);
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
	QVersionNumber version;
	QPoint oldPos;
	QString rrdCmdPath;
	QString rrdPath;
	bool mon;

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
	void on_actionOnTop_triggered();
	void on_actionGraph_triggered();
	void on_actionMonitor_triggered(bool checked);
};

#endif // MAINWINDOW_H
