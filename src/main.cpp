#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"
#include "geo.h"

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

    void clipLines(geo::cPolygon &region);

    /// draw starts and lines
    void draw(wex::shapes &S);

    int x;
    int y;
    bool red;
    int adjust;
    std::vector<geo::line_t> vline;
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
    geo::cPolygon myRegion;
    std::vector<cStart> vStart;

    int minDistance;

    void adjustCalc();
    void removeCollisions();
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
    myRegion.myVertex.push_back({900, 800});
    myRegion.myVertex.push_back({100, 800});
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
        if (myRegion.isInside(geo::point_t(start.x, start.y)))
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
        if (myRegion.isInside(geo::point_t(start.x, start.y)))
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
        if (myRegion.isInside(geo::point_t(start.x, start.y)))
        {
            vStart.push_back(cStart(rand() % maxloc + 100,
                                    rand() % maxloc + 100, true));
            k++;
        }
    }
}

void cStart::clipLines(geo::cPolygon &region)
{
    for (auto &l : vline)
    {
        region.clip(l);
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
        S.line({l.first.first,
                l.first.second,
                l.second.first,
                l.second.second});
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
                geo::line_t(
                    geo::point_t(x, screentop),
                    geo::point_t(x, screenbottom)));
        }
        else if (adjust == MAXINT)
        {
            return;
        }
        else
        {
            x = locAdjusted();
            vline.push_back(
                geo::line_t(
                    geo::point_t(locAdjusted(), screentop),
                    geo::point_t(locAdjusted(), y - distance)));
            vline.push_back(
                geo::line_t(
                    geo::point_t(locAdjusted(), y - distance),
                    geo::point_t(x, y)));
            vline.push_back(
                geo::line_t(
                    geo::point_t(x, y),
                    geo::point_t(locAdjusted(), y + distance)));
            vline.push_back(
                geo::line_t(
                    geo::point_t(locAdjusted(), y + distance),
                    geo::point_t(locAdjusted(), screenbottom)));
        }
    }
    else
    {
        if (adjust == 0)
        {
            vline.push_back(
                geo::line_t(
                    geo::point_t(screenleft, y),
                    geo::point_t(screenright, y)));
        }
        else if (adjust == MAXINT)
        {
            return;
        }
        else
        {
            vline.push_back(
                geo::line_t(
                    geo::point_t(screenleft, locAdjusted()),
                    geo::point_t(x - distance, locAdjusted())));
            vline.push_back(
                geo::line_t(
                    geo::point_t(x - distance, locAdjusted()),
                    geo::point_t(x, y)));
            vline.push_back(
                geo::line_t(
                    geo::point_t(x, y),
                    geo::point_t(x + distance, locAdjusted())));
            vline.push_back(
                geo::line_t(
                    geo::point_t(x + distance, locAdjusted()),
                    geo::point_t(screenright, locAdjusted())));
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

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Aris Path",
              {50, 50, 1000, 1000})
    {

        myPath.generateRandom();
        //myPath.generateTest1();
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
