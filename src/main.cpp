#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"

class cStart
{
public:
    cStart(int locx, int locy, bool fred)
        : x(locx), y(locy), red(fred),
        adjust(0)
    {
    }
    int locAdjusted()
    {
        if (red)
            return x + adjust;
        else
            return y + adjust;
    }

    int x;
    int y;
    bool red;
    int adjust;
    std::vector<std::vector<int>> vline;
};

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
    std::vector<cStart> redStart;
    std::vector<cStart> blueStart;
    // std::vector<std::vector<int>> redLine;
    // std::vector<std::vector<int>> blueLine;

    // /* adjustments required to line location to prevent them being too close
    //     0 : no adjustment
    //     MAXINT : line not required
    //     otherwise adjustment amount
    // */
    // std::vector<int> redAdjust;
    // std::vector<int> blueAdjust;
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
        blueStart.push_back(cStart(
            rand() % maxloc + 100,
            rand() % maxloc + 100,
            false));
    }
    for (int k = 0; k < countRed; k++)
    {
        redStart.push_back(cStart(
            rand() % maxloc + 100,
            rand() % maxloc + 100,
            true));
    }
}

void cArisPath::generateTest1()
{
    const int maxloc = 800;

    srand(time(NULL));
    countRed = 4;
    countBlue = 3;

    blueStart.push_back(cStart(rand() % maxloc + 100,
                         500, false));
    blueStart.push_back(cStart(rand() % maxloc + 100,
                         505, false));
    blueStart.push_back(cStart(rand() % maxloc + 100,
                         510, false));
    blueStart.push_back(cStart(rand() % maxloc + 100,
                         515, false));
    blueStart.push_back(cStart(rand() % maxloc + 100,
                         520, false));

    for (int k = 0; k < countRed; k++)
    {
        redStart.push_back(cStart(rand() % maxloc + 100,
                            rand() % maxloc + 100, true));
    }
}

void cArisPath::draw(wex::shapes &S)
{

    S.fill();
    S.color(0x0000FF);
    for (auto &loc : redStart)
    {
        S.circle(
            loc.x,
            loc.y,
            10);
    }
    S.color(0xFF0000);
    for (auto &loc : blueStart)
    {
        S.circle(
            loc.x,
            loc.y,
            10);
    }

    S.penThick(2);
    S.color(0x0000FF);
    for (auto &start : redStart)
    {
        for (auto &l : start.vline)
        {
            S.line(l);
        }
    }
    S.color(0xFF0000);
    for (auto &start : blueStart)
    {
        for (auto &l : start.vline)
        {
            S.line(l);
        }
    }
}

void cArisPath::adjustCalc()
{
    int i1 = 0;
    for (auto &start1 : redStart)
    {
        int i2 = 0;
        for (auto &start2 : redStart)
        {
            if (fabs(start1.locAdjusted() - start2.locAdjusted()) < minDistance)
            {
                if (i1 != i2)
                {
                    // std::cout << "adjust " << i1 << " too close to " << i2
                    //           << " " << loc1.first << " " << loc2.first << "\n";
                    if (start1.x <= start2.y)
                    {
                        start1.adjust = -minDistance / 2;
                    }
                    else
                    {
                        start1.adjust = minDistance / 2;
                    }
                    break;
                }
            }
            i2++;
        }
        i1++;

        std::cout << start1.x <<" "<< start1.adjust <<" ";
    }

    i1 = 0;
    for (auto &start1 : blueStart)
    {
        int i2 = 0;
        for (auto &start2 : blueStart)
        {
            if (fabs(fabs(start1.locAdjusted() - start2.locAdjusted()) < minDistance))
            {
                if (i1 != i2)
                {
                    if (start1.y <= start2.y)
                    {
                        start1.adjust = -minDistance / 2;
                    }
                    else
                    {
                        start1.adjust = minDistance / 2;
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
    int i1 = -1;
    for (auto &start1 : redStart)
    {
        i1++;
        int i2 = -1;
        for (auto &start2 : redStart)
        {
            i2++;
            if (i1 == i2)
                continue;

            if (start1.adjust == MAXINT || start2.adjust == MAXINT)
                continue;

            if (fabs(start1.locAdjusted() - start2.locAdjusted()) < minDistance)
            {
                // std::cout << "collision " << start1.locAdjusted() 
                //     <<" "<< start2.locAdjusted() 
                //     <<" "<< i1 <<" "<< i2 << "\n";
                start1.adjust = MAXINT;
            }
        }
    }
    i1 = -1;
    for (auto &start1 : blueStart)
    {
        i1++;
        int i2 = -1;
        for (auto &start2 : blueStart)
        {
            i2++;
            if (i1 == i2)
                continue;

            if (start1.adjust == MAXINT || start2.adjust == MAXINT)
                continue;

            if (fabs(start1.locAdjusted() - start2.locAdjusted()) < minDistance)
            {
                start1.adjust = MAXINT;
            }
        }
    }
}

void cArisPath::generateLines()
{
    const int screenleft = 100;
    const int screentop = 100;
    const int screenright = 900;
    const int screenbottom = 900;

    adjustCalc();

    for (auto &start : redStart)
    {
        int x = start.x;

        if (start.adjust == 0)
        {
            start.vline.push_back(
                {x,
                 screentop,
                 x,
                 screenbottom});
        }
        else if (start.adjust == MAXINT)
        {
        }
        else
        {
            x = start.locAdjusted();
            start.vline.push_back(
                {x,
                 screentop,
                 x,
                 start.y - minDistance});
            start.vline.push_back(
                {x,
                 start.y - minDistance,
                 start.x,
                 start.y});
            start.vline.push_back(
                {start.x,
                 start.y,
                 x,
                 start.y + minDistance});
            start.vline.push_back(
                {x,
                 start.y + minDistance,
                 x,
                 screenbottom});
        }
    }

    for (auto &start : blueStart)
    {
        int y = start.y;

        if (start.adjust == 0)
        {
            start.vline.push_back(
                {screenleft,
                 y,
                 screenright,
                 y});
        }
        else if (start.adjust == MAXINT)
        {
        }
        else
        {
            y = start.locAdjusted();
            start.vline.push_back(
                {screenleft,
                 y,
                 start.x - minDistance,
                 y});
            start.vline.push_back(
                {start.x - minDistance,
                 y,
                 start.x,
                 start.y});
            start.vline.push_back(
                {start.x,
                 start.y,
                 start.x + minDistance,
                 y});
            start.vline.push_back(
                {start.x + minDistance,
                 y,
                 screenright,
                 y});
        }
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
