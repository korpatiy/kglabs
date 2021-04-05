//
// Created by korpatiy on 02.04.2021.
//

#ifndef KGLAB2_BEZIER_H
#define KGLAB2_BEZIER_H

/**
 * tbezier.cpp is an implementation of finite discrete point set smooth interpolation algorithms
 * based on cubic Bezier curves with control points calculated according to the tangents to the
 * angles of polygonal line that is built by the linear interpolation of the input points set.
 *
 * Two functions are provided: tbezierSO1 that builds the curve with smoothness order 1 and
 * tbezierSO0 that builds curve with smoothness order 0 and uses special heuristics to
 * reduce lengths of tangents and therefore reduce the difference with linear interpolation
 * making result curve look nicer.
 *
 * tbezierSO1 is recommended for scientific visualization as it uses strict math to balance
 * between smoothness and interpolation accuracy.
 * tbezierSO0 is recommended for advertising purposes as it produces nicer looking curves while
 * the accuracy is in common case lower.
 *
 * Read this for algorithm details: http://sv-journal.org/2017-1/04.php?lang=en
 *
 * Written by Konstantin Ryabinin under terms of MIT license.
 *
 * The MIT License (MIT)
 * Copyright (c) 2016 Konstantin Ryabinin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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
class Point2D {
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
    Point2D(double _x, double _y) {
        x = _x;
        y = _y;
    };

    /**
     * Add other point to the current one.
     *
     * @param p - point to add.
     * @return summ of the current point and the given one.
     */
    Point2D operator+(const Point2D &p) const { return Point2D(x + p.x, y + p.y); };

    /**
     * Subtract other point from the current one.
     *
     * @param p - point to subtract.
     * @return difference of the current point and the given one.
     */
    Point2D operator-(const Point2D &p) const { return Point2D(x - p.x, y - p.y); };

    /**
     * Multiply current point by the real value.
     *
     * @param v - value to multiply by.
     * @return current point multiplied by the given value.
     */
    Point2D operator*(double v) const { return Point2D(x * v, y * v); };

    /**
     * Safely normalize current point.
     */
    void normalize() {
        double l = sqrt(x * x + y * y);
        if (IS_ZERO(l))
            x = y = 0.0;
        else {
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
    static Point2D absMin(const Point2D &p1, const Point2D &p2) {
        return Point2D(abs(p1.x) < abs(p2.x) ? p1.x : p2.x, abs(p1.y) < abs(p2.y) ? p1.y : p2.y);
    };
};

/**
 * The Segment class provides methods to store and calculate Bezier-based cubic curve segment.
 */
class Segment {
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
    Point2D calc(double t) const {
        double t2 = t * t;
        double t3 = t2 * t;
        double nt = 1.0 - t;
        double nt2 = nt * nt;
        double nt3 = nt2 * nt;
        return Point2D(nt3 * points[0].x + 3.0 * t * nt2 * points[1].x + 3.0 * t2 * nt * points[2].x + t3 * points[3].x,
                       nt3 * points[0].y + 3.0 * t * nt2 * points[1].y + 3.0 * t2 * nt * points[2].y +
                       t3 * points[3].y);
    };
};

/**
 * Build an interpolation curve with smoothness order 1 based on cubic Bezier according to given point set.
 *
 * @param values - input array of points to interpolate.
 * @param curve - output array of curve segments.
 * @return true if interpolation successful, false if not.
 */
bool tbezierSO1(const vector<Point2D> &values, vector<Segment> &curve);

/**
 * Build an interpolation curve with smoothness order 0 based on cubic Bezier according to given point set.
 *
 * @param values - input array of points to interpolate.
 * @param curve - output array of curve segments.
 * @return true if interpolation successful, false if not.
 */
bool tbezierSO0(const vector<Point2D> &values, vector<Segment> &curve);

#endif //KGLAB2_BEZIER_H
