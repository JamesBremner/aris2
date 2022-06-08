#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"

class cPolygon
{
public:
    std::vector<std::pair<int, int>> myVertex;

    bool isInside(int x, int y) const;

    void clip(
        int &x1, int &y1,
        int &x2, int &y2);

private:
    bool get_line_intersection(
        float p0_x, float p0_y, float p1_x, float p1_y,
        float p2_x, float p2_y, float p3_x, float p3_y,
        float &i_x, float &i_y);
};

bool cPolygon::isInside(int x, int y) const
{
    typedef std::vector<std::pair<int, int>>::const_iterator it_t;
    int c = 0;
    it_t j = myVertex.end() - 1;
    for (it_t i = myVertex.begin();
         i != myVertex.end(); j = i++)
    {
        if (((i->second > y) != (j->second > y)) &&
            (x < (j->first - i->first) * (y - i->second) /
                         (j->second - i->second) +
                     i->first))
            c = !c;
    }
    return (c == 1);
}

void cPolygon::clip(
    int &x1, int &y1,
    int &x2, int &y2)
{
    //std::cout << "=>clip " << x1 << " " << y1 << " " << x2 << " " << y2 << "\n";

    bool fi1 = isInside(x1, y1);
    bool fi2 = isInside(x2, y2);
    if (fi1 && fi2)
    {
        // both ends inside, no clip needed
        return;
    }
    if ((!fi1) && (!fi2))
    {
        // both ends outside
        std::vector < std::pair<int, int>> vint;
        for (
            int edge = 0;
            edge < myVertex.size() - 1;
            edge++)
        {
            float ix, iy;
            if (get_line_intersection(
                    x1, y1, x2, y2,
                    myVertex[edge].first, myVertex[edge].second,
                    myVertex[edge + 1].first, myVertex[edge + 1].second,
                    ix, iy))
            {
                vint.push_back({ix, iy});
            }
        }
        if (!vint.size())
        {
            // entirely outside
            x1 = -1;
            x2 = -1;
            return;
        }
        if (vint.size() != 2)
            throw std::runtime_error("cPolygon::clip error ");
        if (y1 == y2)
        {
            // horizontal
            if (vint[0].first < vint[1].first)
            {
                x1 = vint[0].first;
                x2 = vint[1].first;
            }
            else
            {
                x1 = vint[1].first;
                x2 = vint[0].first;
            }
        }
        else
        {
            // vertical
            if (vint[0].second < vint[1].second)
            {
                y1 = vint[0].second;
                y2 = vint[1].second;
            }
            else
            {
                y1 = vint[1].second;
                y2 = vint[0].second;
            }
        }
    }
    else
    {
        // partially outside
        for (
            int edge = 0;
            edge < myVertex.size() - 1;
            edge++)
        {
            float ix, iy;
            if (get_line_intersection(
                    x1, y1, x2, y2,
                    myVertex[edge].first, myVertex[edge].second,
                    myVertex[edge + 1].first, myVertex[edge + 1].second,
                    ix, iy))
            {

                if (y1 == y2)
                {
                    if (!fi1)
                    {
                        x1 = ix;
                    }
                    else
                    {
                        x2 = ix;
                    }
                }
                else
                {
                    if (!fi1)
                    {
                        y1 = iy;
                    }
                    else
                    {
                        y2 = iy;
                    }
                }
            }
        }
    }

    //std::cout << "<=clip " << x1 << " " << y1 << "<<" << x2 << " " << y2 << "\n";
}

bool cPolygon::get_line_intersection(
    float p0_x, float p0_y, float p1_x, float p1_y,
    float p2_x, float p2_y, float p3_x, float p3_y,
    float &i_x, float &i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;
    s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;
    s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        i_x = p0_x + (t * s1_x);
        i_y = p0_y + (t * s1_y);
        return true;
    }

    return false; // No collision
}

class cStart
{
public:
    cStart(int locx, int locy, bool fred);

    int locAdjusted() const
    {
        if (red)
            return x + adjust;
        else
            return y + adjust;
    }

    /// Adjust line position if too close to other's line
    void tooClose(
        const cStart &other,
        int distance);

    /// remove line if other line too xlose
    void removeCollisions(
        const cStart &other,
        int distance);

    /// calculate line segments
    void generateLines(int distance);

    void clipLines(cPolygon &region);

    /// draw starts and lines
    void draw(wex::shapes &S);

    int x;
    int y;
    bool red;
    int adjust;
    std::vector<std::vector<int>> vline;
    int myID;
    static int lastID;
};

class cArisPath
{
public:
    cArisPath();
    void generateRandom();
    void generateTest1();
    void generateLines();
    void draw(wex::shapes &S);

private:
    cPolygon myRegion;
    std::vector<cStart> vStart;

    int minDistance;

    void adjustCalc();
    void removeCollisions();

    // bool isInside(int x, int y) const;
};

int cStart::lastID = 0;

cStart::cStart(int locx, int locy, bool fred)
    : x(locx), y(locy), red(fred),
      adjust(0)
{
    myID = ++lastID;
}

cArisPath::cArisPath() : minDistance(20)
{
    myRegion.myVertex.push_back({500, 100});
    myRegion.myVertex.push_back({900, 900});
    myRegion.myVertex.push_back({100, 900});
    myRegion.myVertex.push_back({500, 100});
}

void cArisPath::generateRandom()
{
    const int maxloc = 800;

    srand(time(NULL));
    int countRed = rand() % 15 + 10;
    int countBlue = rand() % 15 + 10;

    for (int k = 0; k < countBlue;)
    {
        cStart start(
            rand() % maxloc + 100,
            rand() % maxloc + 100,
            false);
        if (myRegion.isInside(start.x, start.y))
        {
            vStart.push_back(start);
            k++;
        }
    }
    for (int k = 0; k < countRed;)
    {
        cStart start(
            rand() % maxloc + 100,
            rand() % maxloc + 100,
            true);
        if (myRegion.isInside(start.x, start.y))
        {
            vStart.push_back(start);
            k++;
        }
    }
}

void cArisPath::generateTest1()
{
    const int maxloc = 800;

    srand(time(NULL));
    int countRed = 4;
    int countBlue = 3;

    vStart.push_back(cStart(rand() % maxloc + 100,
                            500, false));
    vStart.push_back(cStart(rand() % maxloc + 100,
                            505, false));
    vStart.push_back(cStart(rand() % maxloc + 100,
                            510, false));
    // vStart.push_back(cStart(rand() % maxloc + 100,
    //                            515, false));
    // vStart.push_back(cStart(rand() % maxloc + 100,
    //                            520, false));

    for (int k = 0; k < countRed;)
    {
        cStart start(rand() % maxloc + 100,
                     rand() % maxloc + 100, true);
        if (myRegion.isInside(start.x, start.y))
        {
            vStart.push_back(cStart(rand() % maxloc + 100,
                                    rand() % maxloc + 100, true));
            k++;
        }
    }
}

void cStart::clipLines(cPolygon &region)
{
    for (auto &l : vline)
    {
        region.clip(
            l[0], l[1], l[2], l[3]);
    }
}

void cStart::draw(wex::shapes &S)
{
    if (red)
        S.color(0x0000FF);
    else
        S.color(0xFF0000);
    S.circle(
        x,
        y,
        10);
    for (auto &l : vline)
    {
        S.line(l);
    }
}

void cStart::tooClose(const cStart &other, int minDistance)
{
    if (red != other.red)
        return;
    if (this == &other)
        return;
    if (fabs(locAdjusted() - other.locAdjusted()) >= minDistance)
        return;
    if (locAdjusted() < other.locAdjusted())
        adjust = -minDistance / 2;
    else
        adjust = minDistance / 2;
}

void cStart::removeCollisions(
    const cStart &other,
    int distance)
{
    if (red != other.red)
        return;
    if (this == &other)
        return;
    if (adjust == MAXINT || other.adjust == MAXINT)
        return;
    if (fabs(locAdjusted() - other.locAdjusted()) < distance)
    {
        if (!red)
            std::cout << "removing " << x << " " << y
                      << " " << myID << " " << other.myID
                      << " " << locAdjusted() << " " << other.locAdjusted() << "\n";
        adjust = MAXINT;
    }
}
void cArisPath::removeCollisions()
{
    for (auto &start1 : vStart)
        for (auto &start2 : vStart)
            start1.removeCollisions(start2, minDistance);
}
void cArisPath::adjustCalc()
{
    for (auto &start1 : vStart)
        for (auto &start2 : vStart)
            start1.tooClose(start2, minDistance);

    removeCollisions();
}
void cArisPath::draw(wex::shapes &S)
{
    S.fill();
    S.penThick(2);
    // S.color(0x00AAAA);
    // for (auto &p : myRegion.myVertex)
    //     S.circle(p.first, p.second, 20);
    for (auto &start : vStart)
        start.draw(S);
}

void cStart::generateLines(int distance)
{
    const int screenleft = 100;
    const int screentop = 100;
    const int screenright = 900;
    const int screenbottom = 900;

    if (red)
    {

        if (adjust == 0)
        {
            vline.push_back(
                {x,
                 screentop,
                 x,
                 screenbottom});
        }
        else if (adjust == MAXINT)
        {
            return;
        }
        else
        {
            x = locAdjusted();
            vline.push_back(
                {locAdjusted(),
                 screentop,
                 locAdjusted(),
                 y - distance});
            vline.push_back(
                {locAdjusted(),
                 y - distance,
                 x,
                 y});
            vline.push_back(
                {x,
                 y,
                 locAdjusted(),
                 y + distance});
            vline.push_back(
                {locAdjusted(),
                 y + distance,
                 locAdjusted(),
                 screenbottom});
        }
    }
    else
    {
        if (adjust == 0)
        {
            vline.push_back(
                {screenleft,
                 y,
                 screenright,
                 y});
        }
        else if (adjust == MAXINT)
        {
            return;
        }
        else
        {
            vline.push_back(
                {screenleft,
                 locAdjusted(),
                 x - distance,
                 locAdjusted()});
            vline.push_back(
                {x - distance,
                 locAdjusted(),
                 x,
                 y});
            vline.push_back(
                {x,
                 y,
                 x + distance,
                 locAdjusted()});
            vline.push_back(
                {x + distance,
                 locAdjusted(),
                 screenright,
                 locAdjusted()});
        }
    }
}

void cArisPath::generateLines()
{
    adjustCalc();

    for (auto &start : vStart)
        start.generateLines(minDistance);

    for (auto &start : vStart)
        start.clipLines(myRegion);
}

// bool cArisPath::isInside(int x, int y) const
// {
//     typedef std::vector<std::pair<int, int>>::const_iterator it_t;
//     int c = 0;
//     it_t j = myRegion.end() - 1;
//     for (it_t i = myRegion.begin();
//          i != myRegion.end(); j = i++)
//     {
//         if (((i->second > y) != (j->second > y)) &&
//             (x < (j->first - i->first) * (y - i->second) /
//                          (j->second - i->second) +
//                      i->first))
//             c = !c;
//     }
//     return (c == 1);
// }

// bool get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,
//                            float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
// {
//     float s1_x, s1_y, s2_x, s2_y;
//     s1_x = p1_x - p0_x;
//     s1_y = p1_y - p0_y;
//     s2_x = p3_x - p2_x;
//     s2_y = p3_y - p2_y;

//     float s, t;
//     s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
//     t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

//     if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
//     {
//         // Collision detected
//         if (i_x != NULL)
//             *i_x = p0_x + (t * s1_x);
//         if (i_y != NULL)
//             *i_y = p0_y + (t * s1_y);
//         return true;
//     }

//     return false; // No collision
// }

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Aris Path",
              {50, 50, 1000, 1000})
    {

        myPath.generateRandom();
        // myPath.generateTest1();
        myPath.generateLines();

        fm.events().draw(
            [&](PAINTSTRUCT &ps)
            {
                wex::shapes S(ps);
                myPath.draw(S);
            });

        show();
        run();
    }

private:
    cArisPath myPath;
};

main()
{
    cGUI theGUI;
    return 0;
}
