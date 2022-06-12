namespace geo
{
    typedef std::pair<int, int> point_t;
    typedef std::pair<point_t, point_t> line_t;

    class cPolygon
    {
    public:
        std::vector<point_t> myVertex;

        bool isInside(point_t p) const;

        /** Clip line by polygon
         * @param[in,out] x1,y1 1st endpoint of line
         * @param[in,out] x2,y2 2nd endpoint of line
         *
         * Assumes the polygon is closed( first vertex == last vertex )
         * so the edges can be easily iterated over.
         *
         * Assumes line is vertical or horizontal
         * and first end point closest to top left
         */
        void clip(line_t &l);

    private:
        bool get_line_intersection(
            const line_t &l1,
            const line_t &l2,
            point_t &xy);
        std::vector<point_t> all_line_intersections(
            const line_t &l);
    };
}
