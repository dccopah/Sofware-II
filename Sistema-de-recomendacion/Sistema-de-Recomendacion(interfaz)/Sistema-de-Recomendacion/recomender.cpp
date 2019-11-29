#include "recomender.h"
//Ordena Data de mayor a menor
bool sortData (Data d1, Data d2) { return (d1.score > d2.score); }

Recomender::~Recomender()
{
    //dtor
}

Recomender::Recomender()
{
    //dtor
}

vector<string> Recomender::split(const string &s, char delim)
{
    stringstream ss(s);
    string item;
    vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

void Recomender::loadFiles()
{
    loadMovies();
    loadRatings();
}

void Recomender::loadMovies()
{
    cout << "load movies" << endl;
    ifstream readMovies(fileMovies.c_str());
    string line;
    getline(readMovies, line);

    while(!readMovies.eof()){
        getline(readMovies, line);
        if(line.empty()) break;

        line = line.substr(0, line.size() -1);
        if(line == "") break;

        vector<string> words = split(line, ',');
        IDMOVIE idMovie = stoi(words[0]);

        vector<string> category = split(words[words.size() - 1], '|');
        for(int i = 0; i < category.size(); ++i)
            generos[category[i]] = 0;

        string name = "";
        for(int i = 1; i < words.size() - 1; ++i)
            name += words[i];

        Movie movie;
        movie.generos = category;
        movie.name = name;
        movie.idMovie = idMovie;

        movies[idMovie] = movie;
    }
}

void Recomender::loadRatings()
{
    cout << "load ratings" << endl;
    ifstream readRatings(fileRatings.c_str());
    string line;
    int cont = 0;
    getline(readRatings, line);

    while(!readRatings.eof()){
        getline(readRatings, line);
        if(line.empty()) break;

        line = line.substr(0, line.size() -1);
        if(line == "") break;

        vector<string> words = split(line, ',');
        IDUSER idUser = stoi(words[0]);
        IDMOVIE idMovie = stoi(words[1]);
        words[2][1] = ',';
        SCORE score = stod(words[2]);

        ratings[idUser][idMovie] = score;
        users[idUser] = 0;

        ++cont;
        if(cont % 2000000 == 0) cout << "rating: " << cont << endl;
    }

    cout << "Termino la carga de ratings." << endl;
    readRatings.close();

}

double Recomender::distEuclidian(unordered_map<IDMOVIE, SCORE> &movies1, unordered_map<IDMOVIE, SCORE> &movies2, IDUSER idUser)
{
    double sumSquares = 0;
    int nIntersections = 0;
    for(auto &movie1: movies1){
        auto itMovie2 = movies2.find(movie1.first);
        if(itMovie2 != movies2.end()){
            sumSquares += pow(movie1.second - itMovie2->second, 2.0);
            ++nIntersections;
        }
        else{
            sumSquares += pow(movie1.second, 2.0);
        }
    }

    if(nIntersections == 0) return 0;

    return 1.0 / (1.0 + sqrt(sumSquares));
}


void Recomender::recomenderMovieInt(vector<IDUSER> &usersVector, unordered_map<IDMOVIE, SCORE> &moviesRoot, int &posGlobal, int nBlocks)
{
    int posIni, posFinal;
    int posMax = usersVector.size();
    IDUSER idUser;

    while(true){
        mutexPosGlobal.lock();
        posIni = posGlobal;
        posFinal = posGlobal + nBlocks;
        posGlobal += nBlocks;
        mutexPosGlobal.unlock();

        if(posIni >= posMax) break;

        posFinal = posFinal > posMax ? posMax : posFinal;

        for(; posIni < posFinal; ++posIni)
        {
            idUser = usersVector[posIni];
            auto &movies2 = ratings[ idUser ];
            users[ idUser ] = distEuclidian(moviesRoot, movies2, idUser);
        }
    }
}

void Recomender::createPortion(unordered_map<IDMOVIE, SCORE> &moviesRoot, int nPortion)
{
    vector<Data> dataMovies;
    Data data;
    for(auto &movie: moviesRoot){
        data.idData = movie.first;
        data.score = movie.second;
        dataMovies.push_back(data);
    }

    sort(dataMovies.begin(), dataMovies.end(), sortData);
    moviesRoot.clear();

    for(int i = 0; i < nPortion / 2; ++i)
        moviesRoot[ dataMovies[i].idData ] = dataMovies[i].score;

    int nDataMovies = dataMovies.size();
    int nDataMid = nDataMovies - (nPortion / 2);
    for(int i = nDataMovies - 1; i >= nDataMid; --i)
        moviesRoot[ dataMovies[i].idData ] = dataMovies[i].score;

}

void Recomender::createUsersVector(vector<IDUSER> &usersVector, IDUSER idUserRoot)
{
    IDUSER idUser;
    for(auto &user: users){
        idUser = user.first;
        if(idUser != idUserRoot)
            usersVector.push_back(idUser);
    }
}

void Recomender::createUsersData(vector<Data> &usersData)
{
    Data data;
    for(auto &user: users){
        data.idData = user.first;
        data.score = user.second;
        if(data.score > 0)
            usersData.push_back(data);
    }
    sort(usersData.begin(), usersData.end(), sortData);
}

void Recomender::getMoviesOfUser(unordered_map<IDMOVIE, SCORE> &moviesUser, unordered_map<IDMOVIE, SCORE> &moviesRoot, unordered_map<IDMOVIE, SCORE> &movieOptions, SCORE scoreUser)
{
    IDMOVIE idMovie;
    SCORE scoreMovie;
    for(auto &movie: moviesUser){
        idMovie = movie.first;
        //Verificamos que el idMovie no se encuentre dentro de las peliculas del userRoot
        if(moviesRoot.find(idMovie) == moviesRoot.end()){
            scoreMovie = movie.second * scoreUser;
            auto itMovieOptions = movieOptions.find(idMovie);
            //Verificamos si idMovie encuentra dentro de las movieOptions;
            if(itMovieOptions != movieOptions.end()){
                SCORE scoreArchived = itMovieOptions->second;
                if(scoreMovie > scoreArchived)
                    movieOptions[idMovie] = scoreMovie;
            }
            else{
                movieOptions[idMovie] = scoreMovie;
            }
        }
    }
}

void Recomender::getMovieOptions(vector<Data> &usersData, unordered_map<IDMOVIE, SCORE> &moviesRoot, unordered_map<IDMOVIE, SCORE> &movieOptions)
{
    int userMax = 100;
    if(usersData.size() < userMax) userMax = usersData.size();

    cout << "Obteniendo movies de los " << userMax << " users mas influyentes." << endl;

    IDUSER idUser;
    SCORE scoreUser;
    for(int user = 0; user < userMax; ++user)
    {
        idUser = usersData[user].idData;
        scoreUser =  usersData[user].score;
        auto &moviesUser = ratings[idUser];
        getMoviesOfUser(moviesUser, moviesRoot, movieOptions, scoreUser);
    }
}

void Recomender::createMoviesData(vector<Data> &moviesData, unordered_map<IDMOVIE, SCORE> &movieOptions)
{
    Data data;
    for(auto &movie: movieOptions){
        data.idData = movie.first;
        data.score = movie.second;
        if(data.score > 0)
            moviesData.push_back(data);
    }
    sort(moviesData.begin(), moviesData.end(), sortData);
}

void Recomender::getMoviesRecomender(vector<Movie> &moviesRecomender, vector<Data> &moviesData, int nResult)
{
    IDMOVIE idMovie;
    SCORE scoreMovie;

    if(moviesData.size() < nResult) nResult = moviesData.size();
    cout << "Se retornara " << nResult << " resultados." << endl;

    for(int i = 0; i < nResult; ++i){
        idMovie = moviesData[i].idData;
        scoreMovie = moviesData[i].score;

        Movie movie = movies[idMovie];
        movie.weight = scoreMovie;
        moviesRecomender.push_back(movie);
    }

}

vector<Movie> Recomender::recomenderMovie(IDUSER idUserRoot, int nThreads, int nBlocks, int nPortion, int nResult)
{
    vector<Movie> moviesRecomender;
    if(users.find(idUserRoot) == users.end()){
        cout << "No existe dicho usuario" << endl;
        return moviesRecomender;
    }

    for(auto &user : users) user.second = 0;
    cout << "nUsers:" << users.size() << endl;

    vector<IDUSER> usersVector;
    createUsersVector(usersVector, idUserRoot);
    cout << "idUserRoot:" << idUserRoot << " usersVector:" << usersVector.size() << endl;

    auto moviesRoot = ratings[idUserRoot];
    cout << "Prev -> idUserRoot:" << idUserRoot << " moviesRoot:" << moviesRoot.size() << endl;
    if(moviesRoot.size() > nPortion) createPortion(moviesRoot, nPortion);
    cout << "Post -> idUserRoot:" << idUserRoot << " moviesRoot:" << moviesRoot.size() << endl;

    int posGlobal = 0;

    vector<thread> hilos;
    for(int i = 0; i  < nThreads; ++i)
        hilos.push_back(thread(&Recomender::recomenderMovieInt,this,ref(usersVector), ref(moviesRoot), ref(posGlobal), nBlocks));

    for(int i = 0; i  < nThreads; ++i)
        hilos[i].join();

    vector<Data> usersData;
    createUsersData(usersData);
    cout << "numero de usersData:" << usersData.size() << endl;

    unordered_map<IDMOVIE, SCORE> movieOptions;
    getMovieOptions(usersData, ratings[idUserRoot], movieOptions);
    cout << "numero de movieOptions:" << movieOptions.size() << endl;

    vector<Data> moviesData;
    createMoviesData(moviesData, movieOptions);
    cout << "numero de moviesData:" << moviesData.size() << endl;

    getMoviesRecomender(moviesRecomender, moviesData, nResult);

    return moviesRecomender;

}

string Recomender::getFileMovies() const
{
    return fileMovies;
}

void Recomender::setFileMovies(const string &value)
{
    fileMovies = value;
}

string Recomender::getFileRatings() const
{
    return fileRatings;
}

void Recomender::setFileRatings(const string &value)
{
    fileRatings = value;
}
