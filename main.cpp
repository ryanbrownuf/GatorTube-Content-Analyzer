#include <iostream>
#include <utility>
#include <vector>
#include <stdlib.h>
#include <queue>
#include "randutils.hpp"
using namespace std;

// Graph class used from Stepik questions
class Graph {
public:
    int numVertices;
    vector<vector<pair<int,int>>> adjList;
    Graph(int userNum) {
        numVertices = userNum + 6;
        adjList.resize(userNum);
        cout << ".";
    }
};

void populateGraph(Graph& g) {

    // build zero weight connections between content
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            if (j != i) {
                g.adjList[i].push_back({j,0});
            }
        }
    }

    randutils::mt19937_rng rng;

    for (int i = 6; i < g.adjList.size(); i++) {
        if ((i-6) % 25000 == 0) {
            cout << ".";
        }

        // create content edges
        int c = rng.uniform(2,3);
        for (int j = 0; j < c; j++) {
            int cont = rand() % 6;
            bool r = false;
            for (int k = 0; k < g.adjList[i].size(); k++) {
                if (cont ==  g.adjList[i][k].first) {
                    j--;
                    r = true;
                    break;
                }
            }
            if (!r) {
                int watch = rng.uniform(100,1000);
                g.adjList[i].push_back({cont, watch});
                g.adjList[cont].push_back({i, watch});
            }
        }

        int f = rng.uniform(5,9);
        for (int j = 0; j < f; j++) {
            int fren = rng.uniform(6,100005);
            bool r = false;
            for (int k = 0; k < g.adjList[i].size(); k++) {
                if (fren ==  g.adjList[i][k].first) {
                    j--;
                    r = true;
                    break;
                }
            }
            if (!r) {
                int perc = rng.uniform(1,100);
                g.adjList[i].push_back({fren, perc});
                g.adjList[fren].push_back({i, perc});
            }
        }
    }
    return;
}

void printContent(int c) {
    switch(c) {
        case 0:
            cout << "Sports ";
            break;
        case 1:
            cout << "Cooking ";
            break;
        case 2:
            cout << "Meme ";
            break;
        case 3:
            cout << "Political ";
            break;
        case 4:
            cout << "TV and Film ";
            break;
        case 5:
            cout << "Educational ";
            break;
        default:
            break;
    }
}


vector<pair<int,int>> BFS(Graph& g)
{
    bool visited[100006] = {false};
    queue<int> q;
    visited[0] = true;
    q.push(0);
    int vert = 0;

    vector<pair<int,int>> res;
    res.resize(6);


    while(!q.empty()) {
        vert = q.front();
        q.pop();

        for (int i = 0; i < g.adjList[vert].size(); i++) {

            // If we are at a user vertex, we use their content connecting edge and add to
            // that content's total viewer count and watch hours
            if (vert > 5 && i < 3) {
                if (g.adjList[vert][i].first < 6) {
                    int cont = g.adjList[vert][i].first;
                    int w = g.adjList[vert][i].second;

                    res[cont].first += w;
                    res[cont].second++;
                }
            }

            int neigh = g.adjList[vert][i].first;

            if (!visited[neigh]) {

                visited[neigh] = true;
                q.push(neigh);
            }
        }
    }

    return res;
}


int shortestPathToFavContent(Graph& graph, int src) {

    // Simplified Dijkstra-like:
    // Finds the "shortest" path between a user to their greatest friend's most watched content
    // Returns that content vertex.

    // Because edges between users are better when they're smaller
    // we can still just find the shortest path leaving this vertex.

    int max = 100;
    int f = -1;
    for (int i = 0; i < graph.adjList[src].size(); i++) {
        if (graph.adjList[src][i].second < max) {
            max = graph.adjList[src][i].second;
            f = graph.adjList[src][i].first;
        }
    }

    // Once we get to the source user's best friend vertex, we have to find the largest
    // "best" path which is the friend's most watched content.

    int min = 100;
    int c = -1;
    for (int i = 0; i < graph.adjList[f].size(); i++) { //graph.adjList[f].size()
        if (graph.adjList[f][i].second > min) { //&& graph.adjList[f][i].second < max) {
            min = graph.adjList[f][i].second;
            c = graph.adjList[f][i].first;
        }
    }

    return c;
}

void correlate(Graph& g, int src) {
    // check all viewers watching src content
    // check their best friend and that friends most watched content

    vector<int> cont;
    cont.resize(6);

    for (int i = 5; i < g.adjList[src].size(); i++) {
        int watcher = g.adjList[src][i].first;
        int friendsFav = shortestPathToFavContent(g, watcher);
        cont[friendsFav]++;
    }

    int min = 0;
    int most = -1;
    int prev = -1;
    for (int i = 0; i < 6; i++) {
        if (cont[i] > min) {
            most = i;
            min = cont[i];
        }
    }
    int max = min;
    min = 0;
    for (int i = 0; i < 6; i++) {
        if (cont[i] > min && cont[i] < max) {
            prev = i;
            min = cont[i];
        }
    }


    // People that watch source content share the best connections with fans of that content
    if (most == src) {
        cout << "Fans of ";
        printContent(src);
        cout << "content share the best connection with other ";
        printContent(src);
        cout << "content fans.\n";
        cout << "Their next best connections are with fans of ";
        printContent(prev);
        cout << "content.\n\n";

        cout << "Continue recommending ";
        printContent(src);
        cout << "content. \nAdditionally reccommend ";
        printContent(prev);
        cout << "content.\n";
    }

    // People that watch source content share the best connections with fans of different content
    else {
        cout << "Fans of ";
        printContent(src);
        cout << "content share the best connection with ";
        printContent(most);
        cout << "content fans.\n";
        cout << "Best to recommend ";
        printContent(most);
        cout << "content to them.\n";
    }

    return;
}



int main() {
    vector<pair<int,int>> res;
    int max = 0;
    int c = -1;

    cout << "GatorTube Backend\nRetreiving Site Data";
    Graph graph = Graph(100000);
    populateGraph(graph);
    cout << "\nDone!";

    while (1) {
        max = 0;
        c = -1;

        cout << "\n1: Content with most viewers\n";
        cout << "2: Content with most watch hours\n";
        cout << "3: Recommendations through audience correlation\n";
        cout << "4: Randomize Data\n";
        cout << "5: Exit\n";
        int option;
        int cOption;
        cin >> option;
        switch(option) {
            case 1:
                res = BFS(graph);
                for (int i = 0; i < res.size(); i++) {
                    if (res[i].second > max) {
                        max = res[i].second;
                        c = i;
                    }
                }
                printContent(c);
                cout << "content has the most viewers.\n";
                break;

            case 2:
                res = BFS(graph);
                for (int i = 0; i < res.size(); i++) {
                    if (res[i].first > max) {
                        max = res[i].first;
                        c = i;
                    }
                }
                printContent(c);
                cout << "content has the most watch hours.\n";
                break;

            case 3:
                cout << "Which audience do you want to examine?\n";
                cout << "1: Sports\n2: Cooking\n3: Memes\n4: Politics\n5: TV and Film\n6: Education\n";
                cin >> cOption;

                switch (cOption) {
                    case 1:
                        correlate(graph, 0);
                        break;
                    case 2:
                        correlate(graph, 1);
                        break;
                    case 3:
                        correlate(graph, 2);
                        break;
                    case 4:
                        correlate(graph, 3);
                        break;
                    case 5:
                        correlate(graph, 4);
                        break;
                    case 6:
                        correlate(graph, 5);
                        break;
                    default:
                        break;
                }
                break;

            case 4:
                graph.adjList.clear();
                graph.adjList.resize(graph.numVertices + 6);
                populateGraph(graph);
                cout << "user data randomized.\n";
                break;

            case 5:
                return 0;

            default:
                break;
        }
    }
}