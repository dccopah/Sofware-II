#ifndef RECOMENDER_H
#define RECOMENDER_H

#include <string>
#include <string.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <mutex>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include "movie.h"
#include "data.h"

using namespace std;

typedef string GENERO;
typedef int IDMOVIE;
typedef int IDUSER;
typedef double SCORE;
typedef string TITLEMOVIE;


class Recomender
{
    private:
        unordered_map<GENERO, int> generos;
        unordered_map<IDMOVIE, Movie> movies;
        unordered_map<IDUSER, double> users;
        unordered_map<IDUSER, unordered_map<IDMOVIE, SCORE> > ratings;

        string fileRatings;
        string fileMovies;
        mutex mutexPosGlobal;

    public:
        Recomender();
        ~Recomender();
        void loadFiles();
        vector<Movie> recomenderMovie(IDUSER idUserRoot, int nThreads, int nBlocks, int nPortion, int nResult);
        string getFileMovies() const;
        string getFileRatings() const;
        void setFileMovies(const string &value);
        void setFileRatings(const string &value);

private:
        void loadMovies();
        void loadRatings();
        vector<string> split(const string &s, char delim);
        double distEuclidian(unordered_map<IDMOVIE, SCORE> &user1, unordered_map<IDMOVIE, SCORE> &user2, IDUSER idUser);
        void recomenderMovieInt(vector<IDUSER> &usersVector, unordered_map<IDMOVIE, SCORE> &moviesRoot, int &posGlobal, int nBlocks);
        void createPortion(unordered_map<IDMOVIE, SCORE> &movies, int nPortion);

        void createUsersVector(vector<IDUSER> &usersVector, IDUSER idUserRoot);
        void createUsersData(vector<Data> &usersData);
        void createMoviesData(vector<Data> &moviesData, unordered_map<IDMOVIE, SCORE> &movieOptions);

        void getMoviesOfUser(unordered_map<IDMOVIE, SCORE> &moviesUser, unordered_map<IDMOVIE, SCORE> &moviesRoot, unordered_map<IDMOVIE, SCORE> &movieOptions, SCORE scoreUser);
        void getMovieOptions(vector<Data> &usersData, unordered_map<IDMOVIE, SCORE> &moviesRoot, unordered_map<IDMOVIE, SCORE> &movieOptions);
        void getMoviesRecomender(vector<Movie> &moviesRecomender, vector<Data> &moviesData, int nResult);
};

#endif // RECOMENDER_H
