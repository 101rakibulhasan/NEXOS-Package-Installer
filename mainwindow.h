#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_cancel_clicked();
    void on_install_clicked();

private:
    Ui::MainWindow *ui;

    QString name = "Unknown";
    QString filetype = "Unknown";
    QString filePath = "Unknown";
    QString author = "Unknown";
    QString vers = "Unknown";
    QString link = "Unknown";

    QString executable = "";

    void NPIreadFile();
    void DEBreadFile();
    void getFilePath();
    void installDEB();
    void openDEB();



};
#endif // MAINWINDOW_H
