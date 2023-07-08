#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include "dialog.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->installing_lab->hide();
    ui->installingPB->hide();

    filePath = "/home/rakibul/Downloads/google-chrome-stable_current_amd64.deb";
    MainWindow::getFilePath();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getFilePath()
{
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists())
    {
        Dialog *d = new Dialog();
        d->show();
    }
    else
    {
        int dotIndex = filePath.lastIndexOf('.');
        if (dotIndex != -1)
        {
            filetype = filePath.mid(dotIndex + 1);
            if (filetype == "npi")
            {
                ui->app_icon->setStyleSheet("border-image: url(:/icons/npi.png);");
                MainWindow::NPIreadFile();
            }else if (filetype == "deb")
            {
                ui->app_icon->setStyleSheet("border-image: url(:/icons/deb.png);");
                MainWindow::DEBreadFile();
            }
            else if (filetype == "exe")
            {
                ui->app_icon->setStyleSheet("border-image: url(:/icons/exe.png);");
                Dialog *d = new Dialog();
                d->show();
            }
            else if (filetype == "apk")
            {
                ui->app_icon->setStyleSheet("border-image: url(:/icons/apk.png);");
                Dialog *d = new Dialog();
                d->show();
            }else
            {
                ui->app_icon->setStyleSheet("border-image: url(:/icons/default.png);");
                Dialog *d = new Dialog();
                d->show();
            }
        }
        else
        {
            ui->app_icon->setStyleSheet("border-image: url(:/icons/default.png);");
            Dialog *d = new Dialog();
            d->show();
        }
    }
}

void MainWindow::NPIreadFile()
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //qDebug() << "Failed to open the file.";
        //return a.exec();
    }else{
        QTextStream in(&file);

        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (line.contains("Name:"))
            {
                int startIndex = line.indexOf("\"") + 1;
                int endIndex = line.lastIndexOf("\"");
                name = line.mid(startIndex, endIndex - startIndex);
                break;
            }
        }

        in.seek(0);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (line.contains("Author:"))
            {
                int startIndex = line.indexOf("\"") + 1;
                int endIndex = line.lastIndexOf("\"");
                author = line.mid(startIndex, endIndex - startIndex);
                break;
            }
        }

        in.seek(0);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (line.contains("Version:"))
            {
                int startIndex = line.indexOf("\"") + 1;
                int endIndex = line.lastIndexOf("\"");
                vers = line.mid(startIndex, endIndex - startIndex);
                break;
            }
        }

        in.seek(0);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (line.contains("Link:"))
            {
                int startIndex = line.lastIndexOf("\"") + 1;
                int endIndex = line.lastIndexOf("\"");
                link = line.mid(startIndex, endIndex - startIndex);
                break;
            }
        }


        // Close the file
        file.close();


        ui->app_name->setText(QString(name));
        ui->app_author->setText(QString(author));
        ui->app_vers->setText(QString(vers));
        ui->app_link->setText(QString(link));
    }
}

void MainWindow::DEBreadFile()
{
    QProcess debinfo;
    debinfo.start("sudo", QStringList() << "dpkg" << "-I" << filePath);
    debinfo.waitForFinished(-1);
    QByteArray debfileraw = debinfo.readAllStandardOutput();
    QTextStream debmeta(&debfileraw);

    debmeta.seek(0);
    while (!debmeta.atEnd())
    {
        QString line = debmeta.readLine();
        if (line.contains("Package:"))
        {
            int startIndex = line.indexOf(":") + 2;
            name = line.mid(startIndex);
            break;
        }
    }

    debmeta.seek(0);
    while (!debmeta.atEnd())
    {
        QString line = debmeta.readLine();
        if (line.contains("Maintainer:"))
        {
            int startIndex = line.indexOf(":") + 2;
            author = line.mid(startIndex);
            break;
        }
    }

    debmeta.seek(0);
    while (!debmeta.atEnd())
    {
        QString line = debmeta.readLine();
        if (line.contains("Version:"))
        {
            int startIndex = line.indexOf(":") + 2;
            vers = line.mid(startIndex);
            break;
        }
    }

    debmeta.seek(0);
    while (!debmeta.atEnd())
    {
        QString line = debmeta.readLine();
        if (line.contains("Homepage:"))
        {
            int startIndex = line.indexOf(":") + 2;
            link = line.mid(startIndex);
            break;
        }
    }

    //name = QFileInfo(filePath).baseName();

    ui->app_name->setText(name);
    ui->app_author->setText(author);
    ui->app_vers->setText(vers);

    if(link == "Unknown"){
        ui->app_link->setText(link);
    }else
    {
        ui->app_link->setText("<a href=\"" + link + "\">" + link + "</a>");
    }

}

void MainWindow::on_cancel_clicked()
{
    QCoreApplication::quit();
}

int open_btn_status = 0;
void MainWindow::on_install_clicked()
{
    ui->install->setDisabled(true);
    ui->cancel->setDisabled(true);
    if(open_btn_status == 0)
    {
        installDEB();
    }else if(open_btn_status == 1)
    {
        openDEB();
    }
}

void MainWindow::installDEB()
{
    ui->installing_lab->show();
    ui->installingPB->show();

    if(filetype == "deb")
    {
        QProcess process;
        process.start("sudo", QStringList() << "dpkg" << "-i" << filePath);

        process.waitForFinished(-1);

        int exitcode = process.exitCode();
        if(exitcode == 0) {
            ui->installingPB->setValue(100);
            ui->installing_lab->setText("Installed successfully");

            ui->install->setText("Open");
            open_btn_status = 1;
        }else {
            ui->installingPB->setValue(95);
            ui->installing_lab->setText("Installation failed");

            ui->install->setVisible(false);
        }
    }

    ui->install->setDisabled(false);
    ui->cancel->setDisabled(false);
}

void MainWindow::openDEB()
{
    QProcess findDir;
    findDir.start("dpkg-query", QStringList() << "-L" << name);
    findDir.waitForFinished();

    QByteArray debDirsRaw = findDir.readAllStandardOutput();
    QTextStream debDirs(&debDirsRaw);

    debDirs.seek(0);
    while (!debDirs.atEnd())
    {
        QString line = debDirs.readLine();

        if (line.contains("/usr/bin/"))
        {
            int startIndex = line.lastIndexOf("/") + 1;
            executable = line.mid(startIndex);
            break;
        }
    }

    if(executable == "")
    {
        executable = name;
    }

    qDebug()  << executable;

    QProcess startExecutable;
    startExecutable.start(executable);
    startExecutable.waitForFinished(-1);

    if(startExecutable.exitStatus() == QProcess::NormalExit)
    {
        //QCoreApplication::quit();
    }else{
        Dialog *d = new Dialog();
        d->show();
    }


}

