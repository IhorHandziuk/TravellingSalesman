#pragma once

#include <iostream>
#include <ctime>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <GL/freeglut.h>

using namespace std;

const int gl_width = 30;
const int gl_height = 30;

struct Vertex {
    float x, y;
    vector<Vertex *> pts;
};

class Graph {
public:
    Graph(int);

    ~Graph(void);

    void create(int, int);

    void read(FILE *, FILE *);

    void display_start();

    void display_loop();

    void draw();

    void make_first_gen(int);

    void next_gen();

    void mutate(map<float, vector<int>> &, map<float, vector<int>>::reverse_iterator);

private:
    void setupDrawCallback();

    void setupKeypressCallback();

    void setupNextGen();

    float cycle_length(vector<int>);

    float length(vector<int>);

    vector<Vertex *> graph;
    map<float, vector<int>> routes;
};

