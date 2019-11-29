#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "recomender.h"
#include <QMainWindow>
#include <QDebug>
#include <QTableWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_pushButton_recomendar_clicked();

    void on_pushButton_cargardata_clicked();

private:
    Ui::MainWindow *ui;

private:
    Recomender r;
    QTableWidget *tableMovies;
    QStringList tableHeader;

private:
    void setTableMovies(vector<Movie> &movies);


};

#endif // MAINWINDOW_H


