#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    r.setFileMovies("data/movies.csv");
    r.setFileRatings("data/ratings.csv");

    tableMovies = ui->tableWidget_movies;
    tableMovies->verticalHeader()->setVisible(false);

    tableHeader << "IDMovie" << "Titulo" << "Peso";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_recomendar_clicked()
{
    int nHilos = ui->lineEdit_nhilos->text().toInt();
    int nBloques = ui->lineEdit_nbloques->text().toInt();
    int nMuestras = ui->lineEdit_nmuestras->text().toInt();
    int idUsuario = ui->lineEdit_idusuario->text().toInt();
    int nResultados = ui->lineEdit_nresultados->text().toInt();

    vector<Movie> movies = r.recomenderMovie(idUsuario, nHilos, nBloques, nMuestras, nResultados);

    nResultados = movies.size();
    cout << "#Peliculas a recomendar: " << nResultados << endl;
    cout << endl;

    setTableMovies(movies);
}

void MainWindow::on_pushButton_cargardata_clicked()
{
    r.loadFiles();
}

void MainWindow::setTableMovies(vector<Movie> &movies)
{
    tableMovies->setRowCount(0);
    tableMovies->setColumnCount(0);
    int nMovies = movies.size();

    tableMovies->setRowCount(nMovies);
    tableMovies->setColumnCount(3);
    tableMovies->setHorizontalHeaderLabels(tableHeader);

    tableMovies->setColumnWidth(1, 360);

    for(int i = 0; i< nMovies; ++i){
        tableMovies->setItem(i, 0, new QTableWidgetItem(QString::number(movies[i].idMovie)));
        tableMovies->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(movies[i].name)));
        tableMovies->setItem(i, 2, new QTableWidgetItem(QString::number(movies[i].weight)));
    }
}
