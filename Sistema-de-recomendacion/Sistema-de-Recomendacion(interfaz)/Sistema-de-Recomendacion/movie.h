#ifndef MOVIE_H
#define MOVIE_H

#include <string>
#include <string.h>
#include <vector>

using namespace std;

class Movie
{
    public:
        int idMovie;
        float weight;
        string name;
        vector<string> generos;

    public:
        Movie();
        ~Movie();

    protected:

    private:
};

#endif // MOVIE_H
