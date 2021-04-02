//
// Created by korpatiy on 02.04.2021.
//

#ifndef KGLAB2_BEZIER_H
#define KGLAB2_BEZIER_H

#include <vector>
#include <iostream>
#include <cmath>

using namespace std;

/**
 * Threshold for zero.
 */
#define EPSILON 1.0e-5
/**
 * Test if real value is zero.
 */
#define IS_ZERO(v) (abs(v) < EPSILON)
/**
 * Signum function.
 */
#define SIGN(v) (int)(((v) > EPSILON) - ((v) < -EPSILON))
/**
 * Amount of lines representing each Bezier segment.
 */
#define RESOLUTION 32
/**
 * Paramenet affecting curvature, should be in [2; +inf).
 */
#define C 2.0

/**
 * The Point2D class provides methods to store and handle 2d points.
 */
class Point2D
{
public:
    /**
     * Point coordinates.
     */
    double x, y;

    /**
     * Point2D constructor.
     */
    Point2D() { x = y = 0.0; };
    /**
     * Point2D constructor.
     *
     * @param x - x coordinate of the point.
     * @param y - y coordinate of the point.
     */
    Point2D(double _x, double _y) { x = _x; y = _y; };

    /**
     * Add other point to the current one.
     *
     * @param p - point to add.
     * @return summ of the current point and the given one.
     */
    Point2D operator +(const Point2D &p) const { return Point2D(x + p.x, y + p.y); };
    /**
     * Subtract other point from the current one.
     *
     * @param p - point to subtract.
     * @return difference of the current point and the given one.
     */
    Point2D operator -(const Point2D &p) const { return Point2D(x - p.x, y - p.y); };
    /**
     * Multiply current point by the real value.
     *
     * @param v - value to multiply by.
     * @return current point multiplied by the given value.
     */
    Point2D operator *(double v) const { return Point2D(x * v, y * v); };

    /**
     * Safely normalize current point.
     */
    void normalize()
    {
        double l = sqrt(x * x + y * y);
        if (IS_ZERO(l))
            x = y = 0.0;
        else
        {
            x /= l;
            y /= l;
        }
    };

    /**
     * Get the absolute minimum of two given points.
     *
     * @param p1 - first point.
     * @param p2 - second point.
     * @return absolute minimum of the given points' coordinates.
     */
    static Point2D absMin(const Point2D &p1, const Point2D &p2)
    {
        return Point2D(abs(p1.x) < abs(p2.x) ? p1.x : p2.x, abs(p1.y) < abs(p2.y) ? p1.y : p2.y);
    };
};

/**
 * The Segment class provides methods to store and calculate Bezier-based cubic curve segment.
 */
class Segment
{
public:
    /**
     * Bezier control points.
     */
    Point2D points[4];

    /**
     * Calculate the intermediate curve points.
     *
     * @param t - parameter of the curve, should be in [0; 1].
     * @return intermediate Bezier curve point that corresponds the given parameter.
     */
    Point2D calc(double t) const
    {
        double t2 = t * t;
        double t3 = t2 * t;
        double nt = 1.0 - t;
        double nt2 = nt * nt;
        double nt3 = nt2 * nt;
        return Point2D(nt3 * points[0].x + 3.0 * t * nt2 * points[1].x + 3.0 * t2 * nt * points[2].x + t3 * points[3].x,
                       nt3 * points[0].y + 3.0 * t * nt2 * points[1].y + 3.0 * t2 * nt * points[2].y + t3 * points[3].y);
    };
};

bool tbezierSO0(const vector<Point2D> &values, vector<Segment> &curve)
{
    int n = values.size() - 1;

    if (n < 2)
        return false;

    curve.resize(n);

    Point2D cur, next, tgL, tgR, deltaC;
    double l1, l2, tmp, x;
    bool zL, zR;

    next = values[1] - values[0];
    next.normalize();

    for (int i = 0; i < n; ++i)
    {
        tgL = tgR;
        cur = next;

        deltaC = values[i + 1] - values[i];

        if (i < n - 1)
        {
            next = values[i + 2] - values[i + 1];
            next.normalize();
            if (IS_ZERO(cur.x) || IS_ZERO(cur.y))
                tgR = cur;
            else if (IS_ZERO(next.x) || IS_ZERO(next.y))
                tgR = next;
            else
                tgR = cur + next;
            tgR.normalize();
        }
        else
        {
            tgR = Point2D();
        }

        // There is actually a little mistake in the white paper (http://sv-journal.org/2017-1/04.php?lang=en):
        // algorithm described after figure 14 implicitly assumes that tangent vectors point inside the
        // A_i and B_i areas (see fig. 14). However in practice they can point outside as well. If so, tangents’
        // coordinates should be clamped to the border of A_i or B_i respectively to keep control points inside
        // the described area and thereby to avoid false extremes and loops on the curve.
        // The clamping is implemented by the next 4 if-statements.
        if (SIGN(tgL.x) != SIGN(deltaC.x))
            tgL.x = 0.0;
        if (SIGN(tgL.y) != SIGN(deltaC.y))
            tgL.y = 0.0;
        if (SIGN(tgR.x) != SIGN(deltaC.x))
            tgR.x = 0.0;
        if (SIGN(tgR.y) != SIGN(deltaC.y))
            tgR.y = 0.0;

        zL = IS_ZERO(tgL.x);
        zR = IS_ZERO(tgR.x);

        l1 = zL ? 0.0 : deltaC.x / (C * tgL.x);
        l2 = zR ? 0.0 : deltaC.x / (C * tgR.x);

        if (abs(l1 * tgL.y) > abs(deltaC.y))
            l1 = IS_ZERO(tgL.y) ? 0.0 : deltaC.y / tgL.y;
        if (abs(l2 * tgR.y) > abs(deltaC.y))
            l2 = IS_ZERO(tgR.y) ? 0.0 : deltaC.y / tgR.y;

        if (!zL && !zR)
        {
            tmp = tgL.y / tgL.x - tgR.y / tgR.x;
            if (!IS_ZERO(tmp))
            {
                x = (values[i + 1].y - tgR.y / tgR.x * values[i + 1].x - values[i].y + tgL.y / tgL.x * values[i].x) / tmp;
                if (x > values[i].x && x < values[i + 1].x)
                {
                    if (abs(l1) > abs(l2))
                        l1 = 0.0;
                    else
                        l2 = 0.0;
                }
            }
        }

        curve[i].points[0] = values[i];
        curve[i].points[1] = curve[i].points[0] + tgL * l1;
        curve[i].points[3] = values[i + 1];
        curve[i].points[2] = curve[i].points[3] - tgR * l2;
    }

    return true;
}

#endif //KGLAB2_BEZIER_H
