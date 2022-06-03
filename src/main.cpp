#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"

class cArisPath
{
public:
    cArisPath() : minDistance(20) {}
    void generateRandom();
    void generateTest1();
    void generateLines();
    void draw(wex::shapes &S);

private:
    int countRed;
    int countBlue;
    std::vector<std::pair<int, int>> redStart;
    std::vector<std::pair<int, int>> blueStart;
    std::vector<std::vector<int>> redLine;
    std::vector<std::vector<int>> blueLine;

    /* adjustments required to line location to prevent them being too close
        0 : no adjustment
        MAXINT : line not required
        otherwise adjustment amount
    */
    std::vector<int> redAdjust;
    std::vector<int> blueAdjust;
    int minDistance;

    void adjustCalc();
    void removeCollisions();
};

void cArisPath::generateRandom()
{
    const int maxloc = 800;

    srand(time(NULL));
    countRed = rand() % 15 + 10;
    countBlue = rand() % 15 + 10;

    for (int k = 0; k < countBlue; k++)
    {
        blueStart.push_back({rand() % maxloc + 100,
                             rand() % maxloc + 100});
    }
    for (int k = 0; k < countRed; k++)
    {
        redStart.push_back({rand() % maxloc + 100,
                            rand() % maxloc + 100});
    }
}

void cArisPath::generateTest1()
{
    const int maxloc = 800;

    srand(time(NULL));
    countRed = 4;
    countBlue = 3;

    blueStart.push_back({rand() % maxloc + 100,
                         500});
    blueStart.push_back({rand() % maxloc + 100,
                         505});
    blueStart.push_back({rand() % maxloc + 100,
                         510});
    blueStart.push_back({rand() % maxloc + 100,
                         515});
    blueStart.push_back({rand() % maxloc + 100,
                         520});

    for (int k = 0; k < countRed; k++)
    {
        redStart.push_back({rand() % maxloc + 100,
                            rand() % maxloc + 100});
    }
}

void cArisPath::draw(wex::shapes &S)
{

    S.fill();
    S.color(0x0000FF);
    for (auto &loc : redStart)
    {
        S.circle(
            loc.first,
            loc.second,
            10);
    }
    S.color(0xFF0000);
    for (auto &loc : blueStart)
    {
        S.circle(
            loc.first,
            loc.second,
            10);
    }

    S.penThick(2);
    S.color(0x0000FF);
    for (auto &line : redLine)
    {
        S.line(line);
    }
    S.color(0xFF0000);
    for (auto &line : blueLine)
    {
        S.line(line);
    }
}

void cArisPath::adjustCalc()
{
    redAdjust.resize(redStart.size());
    int i1 = 0;
    for (auto &loc1 : redStart)
    {
        int i2 = 0;
        for (auto &loc2 : redStart)
        {
            if (fabs(
                (loc1.first+redAdjust[i1]) - 
                (loc2.first+redAdjust[i2]))
                 < minDistance)
            {
                if (i1 != i2)
                {
                    std::cout << "adjust " << i1 << " too close to " << i2
                              << " " << loc1.first << " " << loc2.first << "\n";
                    if (loc1.first <= loc2.first)
                    {
                        redAdjust[i1] = -minDistance / 2;
                        std::cout << "left\n";
                    }
                    else
                    {
                        redAdjust[i1] = minDistance / 2;
                        std::cout << "right\n";
                    }
                    break;
                }
            }
            i2++;
        }
        i1++;
    }
    blueAdjust.resize(blueStart.size());
    i1 = 0;
    for (auto &loc1 : blueStart)
    {
        int i2 = 0;
        for (auto &loc2 : blueStart)
        {
            if (fabs(
                (loc1.second+blueAdjust[i1]) - 
                (loc2.second+blueAdjust[i2])) 
                < minDistance)
            {
                if (i1 != i2)
                {
                    if (loc1.second <= loc2.second)
                    {
                        blueAdjust[i1] = -minDistance / 2;
                    }
                    else
                    {
                        blueAdjust[i1] = minDistance / 2;
                    }
                    break;
                }
            }
            i2++;
        }
        i1++;
    }
    removeCollisions();
}
void cArisPath::removeCollisions()
{
    int i1 = 0;
    for (auto &loc1 : redStart)
    {
        int i2 = 0;
        for (auto &loc2 : redStart)
        {
            if (i1 == i2)
                continue;

            if (redAdjust[i1] == MAXINT || redAdjust[i2] == MAXINT)
                continue;

            if (fabs(
                    (loc1.first + redAdjust[i1]) -
                    (loc2.first + redAdjust[i2])) < minDistance)
            {
                redAdjust[i1] = MAXINT;
            }
            i2++;
        }
        i1++;
    }
    i1 = 0;
    for (auto &loc1 : blueStart)
    {
        int i2 = 0;
        for (auto &loc2 : blueStart)
        {
            if (i1 == i2)
                continue;

            if (blueAdjust[i1] == MAXINT || blueAdjust[i2] == MAXINT)
                continue;

            if (fabs(
                    (loc1.second + blueAdjust[i1]) -
                    (loc2.second + blueAdjust[i2])) < minDistance)
            {
                blueAdjust[i1] = MAXINT;
            }
            i2++;
        }
        i1++;
    }
}

void cArisPath::generateLines()
{
    const int screenleft = 100;
    const int screentop = 100;
    const int screenright = 900;
    const int screenbottom = 900;

    adjustCalc();

    int i1 = 0;
    for (auto &loc : redStart)
    {
        int x = loc.first;

        if (redAdjust[i1] == 0)
        {
            redLine.push_back(
                {x,
                 screentop,
                 x,
                 screenbottom});
        }
        else if (redAdjust[i1] == MAXINT)
        {
        }
        else
        {
            x += redAdjust[i1];
            redLine.push_back(
                {x,
                 screentop,
                 x,
                 loc.second - minDistance});
            redLine.push_back(
                {x,
                 loc.second - minDistance,
                 loc.first,
                 loc.second});
            redLine.push_back(
                {loc.first,
                 loc.second,
                 x,
                 loc.second + minDistance});
            redLine.push_back(
                {x,
                 loc.second + minDistance,
                 x,
                 screenbottom});
        }
        i1++;
    }

    i1 = 0;
    for (auto &loc : blueStart)
    {
        int y = loc.second;

        if (blueAdjust[i1] == 0)
        {
            blueLine.push_back(
                {screenleft,
                 y,
                 screenright,
                 y});
        }
        else if (blueAdjust[i1] == MAXINT)
        {
        }
        else
        {
            y += blueAdjust[i1];
            blueLine.push_back(
                {screenleft,
                 y,
                 loc.first - minDistance,
                 y});
            blueLine.push_back(
                {loc.first - minDistance,
                 y,
                 loc.first,
                 loc.second});
            blueLine.push_back(
                {loc.first,
                 loc.second,
                 loc.first + minDistance,
                 y});
            blueLine.push_back(
                {loc.first + minDistance,
                 y,
                 screenright,
                 y});
        }
        i1++;
    }
}

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Aris Path",
              {50, 50, 1000, 1000}),
          lb(wex::maker::make<wex::label>(fm))
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
    wex::label &lb;
    cArisPath myPath;
};

main()
{
    cGUI theGUI;
    return 0;
}
