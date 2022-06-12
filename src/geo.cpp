#include <vector>
#include "geo.h"

namespace geo
{
 
    bool cPolygon::isInside(point_t p) const
    {
        typedef std::vector<std::pair<int, int>>::const_iterator it_t;
        int c = 0;
        it_t j = myVertex.end() - 1;
        for (it_t i = myVertex.begin();
             i != myVertex.end(); j = i++)
        {
            if (((i->second > p.second) != (j->second > p.second)) &&
                (p.first < (j->first - i->first) * (p.second - i->second) /
                                   (j->second - i->second) +
                               i->first))
                c = !c;
        }
        return (c == 1);
    }

    void cPolygon::clip(line_t &l)
    {
        // std::cout << "=>clip "
        //           << l.first.first << " " << l.first.second << " "
        //           << l.second.first << " " << l.second.second << "\n";

        bool fi1 = isInside(l.first);
        bool fi2 = isInside(l.second);
        if (fi1 && fi2)
        {
            // both ends inside, no clip needed
            return;
        }

        auto vint = all_line_intersections(l);

        if (!vint.size())
        {
            // entirely outside
            l.first.first = -1;
            l.first.second = -1;
            l.second.first = -1;
            l.second.second = -1;
            return;
        }


        if ((!fi1) && (!fi2))
        {
            // both ends outside
            // std::cout << "both "
            //     << vint[0].first <<" " << vint[0].second 
            //     <<" "<< vint[1].first <<" " << vint[1].second << "\n";
            if (l.first.second == l.second.second)
            {
                // horizontal
                if (vint[0].first < vint[1].first)
                {
                    l.first.first = vint[0].first;
                    l.second.first = vint[1].first;
                }
                else
                {
                    l.first.first = vint[1].first;
                    l.second.first = vint[0].first;
                }
            }
            else
            {
                // vertical
                if (vint[0].second < vint[1].second)
                {
                    l.first.second = vint[0].second;
                    l.second.second = vint[1].second;
                }
                else
                {
                    l.first.second = vint[1].second;
                    l.second.second = vint[0].second;
                }
            }
        }
        else
        {
            // partially outside

            // std::cout << "partial "
            //     << vint.size() <<" "<< vint[0].first
            //     <<" " << vint[0].second << "\n";

            if (l.first.second == l.second.second)
            {
                // horizontal
                if( ! fi1 ) 
                    l.first.first = vint[0].first;
                else
                    l.second.first = vint[0].first;
            }
            else
            {
                if( ! fi1 )
                    l.first.second = vint[0].second;
                else
                    l.second.second = vint[0].second;
            }
        }

        // std::cout << "<=clip "
        //           << l.first.first << " " << l.first.second << " "
        //           << l.second.first << " " << l.second.second << "\n\n";
    }

    bool cPolygon::get_line_intersection(
        const line_t &l1,
        const line_t &l2,
        point_t &x)
    // float p0_x, float p0_y, float p1_x, float p1_y,
    // float p2_x, float p2_y, float p3_x, float p3_y,
    // float &i_x, float &i_y)
    {
        float s1_x, s1_y, s2_x, s2_y;
        // s1_x = p1_x - p0_x;
        // s1_y = p1_y - p0_y;
        // s2_x = p3_x - p2_x;
        // s2_y = p3_y - p2_y;

        s1_x = l1.second.first - l1.first.first;
        s1_y = l1.second.second - l1.first.second;
        s2_x = l2.second.first - l2.first.first;
        s2_y = l2.second.second - l2.first.second;

        float s, t;
        // s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
        // t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

        s = (-s1_y * (l1.first.first - l2.first.first) + s1_x * (l1.first.second - l2.first.second)) /
            (-s2_x * s1_y + s1_x * s2_y);
        t = (s2_x * (l1.first.second - l2.first.second) - s2_y * (l1.first.first - l2.first.first)) /
            (-s2_x * s1_y + s1_x * s2_y);

        if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
        {
            // Collision detected
            x.first = l1.first.first + (t * s1_x);
            x.second = l1.first.second + (t * s1_y);
            return true;
        }

        return false; // No collision
    }
    std::vector<point_t> cPolygon::all_line_intersections(
        const line_t &l)
    {
        std::vector<point_t> vint;
        for (
            int edge = 0;
            edge < myVertex.size() - 1;
            edge++)
        {
            point_t x;
            if (get_line_intersection(
                    l,
                    line_t(myVertex[edge], myVertex[edge + 1]),
                    x))
            {
                vint.push_back(x);
            }
        }
        return vint;
    }
}
