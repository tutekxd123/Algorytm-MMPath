#include <format>
#include <vector>
#include <glaze/glaze.hpp>
#include <unordered_set>
#include <random>
#pragma once
static int GenerateNumber(int min, int max) {
    return rand() % (max - min + 1) + min;
}
static int GenerateNumberWithRNG(int min, int max, std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}
static bool ChanceGenerate(int percentage) {
    int number = GenerateNumber(0, 100);
    return number < percentage;
}
struct Point {
public:
    uint8_t x;
    uint8_t y;
    bool collision;
    Point(int x, int y, bool collision = false) : x(x), y(y), collision(collision) {};
    std::string toString() {
        return std::format("{},{}", this->x, this->y);
    }
    Point() : x(0), y(0), collision(false) {};
    bool operator==(const Point& otherpoint) const {
        return this->x == otherpoint.x && this->y == otherpoint.y;
    }
};


struct Edge {
public:
    int idMapConnect = 0;
    Point Grid1Point;
    Point Grid2Point;
    Edge(int idMapConnect, Point& Grid1Point, Point& Grid2Point) : idMapConnect(idMapConnect), Grid1Point(Grid1Point), Grid2Point(Grid2Point) {};
	Edge() : idMapConnect(0), Grid1Point(), Grid2Point() {};
    Edge(int idMapConnect, Point* Grid1Point, Point* Grid2Point) : idMapConnect(idMapConnect), Grid1Point(*Grid1Point), Grid2Point(*Grid2Point) {};
};

class Grid {
public:
    std::vector<Point> points; //1D or 2D? x*ymax + y 
    std::vector<Edge> Edges;
    unsigned short  id = 0;
    uint8_t width = 0;
    uint8_t height = 0;
    Grid(int id, int width, int height) : id(id), width(width), height(height) {}
	Grid() : id(0), width(1), height(1) {}
    Point& getPoint(int x, int y) {
        return points[(x * height) + y];
    }
    const Point& getPoint(int x, int y)const  {
        return points[(x * height) + y];
    }
    Point* getNotRandomPoint(std::unordered_set<std::string>& PointsonGridTaken) {
        std::vector<Point*> edgePoints;
        //Metoda która dodaje punkty na edge
        for (int x = 0; x < width; x++) {
            edgePoints.push_back(&getPoint(x, 0));
            edgePoints.push_back(&getPoint(x, height - 1));
        }
        //0,0 1,0 2,0 3,0   4,0 .... + w gore
        for (int y = 1; y < height - 1; y++) {
            edgePoints.push_back(&getPoint(0, y));
            edgePoints.push_back(&getPoint(width - 1, y));
        }
        // lewa i prawa scianka
        // 0,1 0,2 0,3 + width,y
        //pick nearnest point ? from what? from everyEdge?
		std::vector < std::pair<int, Point*>> distances;
        for (auto& point : edgePoints) {
			int sumdistance = 0;
            for (auto& edge : Edges) {
                Point current = edge.Grid1Point;
				int dist = abs(point->x - current.x) + abs(point->y - current.y);
				sumdistance += dist;
            }
			distances.emplace_back(sumdistance, point);
        }
        if (distances.empty()) {
            int idx = GenerateNumber(0, edgePoints.size() - 1);
            return edgePoints[idx];
        }
        else {
			//iterujemy i bierzemy ten z najwieksza odlegloscia
            int maxdistance = 0;
			Point* selectedpoint = nullptr;
            for(auto& [dist, point] : distances) {
                if (PointsonGridTaken.contains(point->toString()) || point->collision) {
                    continue;
                }
                if (dist >= maxdistance) {
                    maxdistance = dist;
					selectedpoint = point;
                }
			}
			PointsonGridTaken.insert(selectedpoint->toString());
            return selectedpoint;

        }

    }
    Point* getRandomPoint(bool collision, std::unordered_set<std::string>& PointsonGridTaken, std::mt19937& rng) {
        size_t maxretry = this->points.size();
        while (maxretry > 0) {
            int x = GenerateNumberWithRNG(0, this->width - 1,rng);
            int y = GenerateNumberWithRNG(0, this->height - 1,rng);
            Point& point = this->getPoint(x, y);
            if (point.collision == false) {
                std::string pointtostring = point.toString();
                if (PointsonGridTaken.contains(pointtostring)) {
                    continue;
                }
                //PointsonGridTaken.insert(pointtostring);
                bool ok = true;
                for (auto& taken : PointsonGridTaken) {
					int tx, ty;
                    sscanf_s(taken.c_str(), "%d,%d", &tx, &ty); //?? wczytanie ze string do dwoch intow
                    if ( (abs(tx - point.x) +  abs(ty - point.y) )<5) {
                        ok = false;
                        break;
					}
                }
                if (ok) {
					PointsonGridTaken.insert(pointtostring);
					return &point;
                }
                else {
                    maxretry--;
                    continue;
                }
                return &point;
            }
            maxretry--;
        }
        return nullptr;
    }
    Edge getEdge(int mapidconnect, int& lengthoperations) {
        for (auto& edge : Edges) {
            lengthoperations += 2;
            if (edge.idMapConnect == mapidconnect) {
                return edge;
            }
        }
        return Edge();
    }

    Edge getEdge(int mapidconnect, int& lengthoperations) const {
        for (const auto& edge : Edges) {
            lengthoperations += 2;
            if (edge.idMapConnect == mapidconnect) {
                return edge;
            }
        }
        return Edge();
    }

    void GenerateNewGrid(int minx, int maxx, int miny, int maxy, int chanceforcol) {
        width = GenerateNumber(minx, maxx);
        height = GenerateNumber(miny, maxy);
        points.clear();
        points.reserve(width * height);
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                points.emplace_back(Point(x, y, ChanceGenerate(chanceforcol)));
            }
        }
    }
};


class Graph {
public:
    //Vector Siatek i generowanie miedzy nimi przejsc?
    std::vector<Grid>Grids;
    Graph() {};
};

template <>
struct glz::meta<Point> {
    using T = Point;
    static constexpr auto value = object(
        "x",& T::x,
        "y",& T::y,
        "collision",& T::collision
    );

};
template <>
struct glz::meta<Edge> {
    using T = Edge;
    static constexpr auto value = object(
        "idMapConnect",& T::idMapConnect,
        "Grid1Point",& T::Grid1Point,
        "Grid2Point",& T::Grid2Point
    );
};
template <>
struct glz::meta<Grid> {
    using T = Grid;
    static constexpr auto value = object(
        "points",& T::points,
        "Edges",& T::Edges,
        "id",& T::id,
        "width",& T::width,
        "height",& T::height
    );
};
template <>
struct glz::meta<Graph> {
    using T = Graph;
    static constexpr auto value = object(
        "Grids", &T::Grids
    );
};