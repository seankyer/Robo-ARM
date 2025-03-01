/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <lib/map_utils.h>
#include <errno.h>
#include <math.h>

#define EPSILON 0.001

static void set_segment(struct segment *seg, double x1, double y1, double x2, double y2)
{
        seg->x1 = x1;
        seg->y1 = y1;
        seg->x2 = x2;
        seg->y2 = y2;
}

static bool float_equal(double a, double b)
{
        return fabs(a - b) < EPSILON;
}

static bool compare_segments(struct segment s1, struct segment s2)
{
        if (!float_equal(s1.x1, s2.x1)) {
                return false;
        } else if(!float_equal(s1.y1, s2.y1)) {
                return false;
        } else if(!float_equal(s1.x2, s2.x2)) {
                return false;
        } else if(!float_equal(s1.y2, s2.y2)) {
                return false;
        }

        return true;
}

ZTEST(map_utils, test_segment_translation)
{
        struct segment start;
        struct segment end;
        double mag;

        /* Horizontal line shift */
        set_segment(&start, 0, 0, 4, 0);
        set_segment(&end, 0, 2, 4, 2);
        mag = 2;
        zassert_equal(compare_segments(translate_segment(start, mag), end), true);

        /* Vertical line shift */
        set_segment(&start, 0, 0, 0, 4);
        set_segment(&end, -2, 0, -2, 4);
        mag = 2;
        zassert_equal(compare_segments(translate_segment(start, mag), end), true);

        /* Diagonal line shift */
        set_segment(&start, 0, 0, 3, 3);
        set_segment(&end, -1.414, 1.414, 1.586, 4.414);
        mag = 2;
        zassert_equal(compare_segments(translate_segment(start, mag), end), true);

        /* Negative shift */
        set_segment(&start, 0, 0, 4, 3);
        set_segment(&end, 1.2, -1.6, 5.2, 1.4);
        mag = -2;
        zassert_equal(compare_segments(translate_segment(start, mag), end), true);

        /* Very small shift */
        set_segment(&start, 1, 1, 4, 5);
        set_segment(&end, 0.9997, 1.0006, 3.9997, 5.0006);
        mag = 0.001;
        zassert_equal(compare_segments(translate_segment(start, mag), end), true);

        /* Very large shift */
        set_segment(&start, 0, 0, 1, 1);
        set_segment(&end, -707.107, 707.107, -706.107, 708.107);
        mag = 1000;
        zassert_equal(compare_segments(translate_segment(start, mag), end), true);
}

ZTEST(map_utils, test_segment_end_trig)
{
        double x;
        double y;
        int ret;

        ret = get_segment_endpoint_trig(5, 0, NULL, &y);
        zassert_equal(ret, -EINVAL);

        ret = get_segment_endpoint_trig(5, 0, &x, NULL);
        zassert_equal(ret, -EINVAL);

        ret = get_segment_endpoint_trig(5, 0, &x, &y);
        zassert_equal(ret, 0);
        zassert_equal(float_equal(x, 5), true);
        zassert_equal(float_equal(y, 0), true);

        ret = get_segment_endpoint_trig(5, 90, &x, &y);
        zassert_equal(ret, 0);
        zassert_equal(float_equal(x, 0), true);
        zassert_equal(float_equal(y, 5), true);

        ret = get_segment_endpoint_trig(5, 180, &x, &y);
        zassert_equal(ret, 0);
        zassert_equal(float_equal(x, -5), true);
        zassert_equal(float_equal(y, 0), true);

        ret = get_segment_endpoint_trig(5, 270, &x, &y);
        zassert_equal(ret, 0);
        zassert_equal(float_equal(x, 0), true);
        zassert_equal(float_equal(y, -5), true);

        ret = get_segment_endpoint_trig(5, 360, &x, &y);
        zassert_equal(ret, 0);
        zassert_equal(float_equal(x, 5), true);
        zassert_equal(float_equal(y, 0), true);

        ret = get_segment_endpoint_trig(10, 45, &x, &y);
        zassert_equal(ret, 0);
        zassert_equal(float_equal(x, 10 * cos(M_PI / 4)), true);
        zassert_equal(float_equal(y, 10 * sin(M_PI / 4)), true);

        ret = get_segment_endpoint_trig(-5, 30, &x, &y);
        zassert_equal(ret, 0);
        zassert_equal(float_equal(x, -5 * cos(M_PI / 6)), true);
        zassert_equal(float_equal(y, -5 * sin(M_PI / 6)), true);

        ret = get_segment_endpoint_trig(7.5, 200, &x, &y);
        zassert_equal(ret, 0);
        zassert_equal(float_equal(x, 7.5 * cos(10 * M_PI / 9)), true);
        zassert_equal(float_equal(y, 7.5 * sin(10 * M_PI / 9)), true);

        ret = get_segment_endpoint_trig(7, 175.5, &x, &y);
        zassert_equal(ret, 0);
        zassert_equal(float_equal(x, 7 * cos(175.5 * M_PI / 180)), true);
        zassert_equal(float_equal(y, 7 * sin(175.5 * M_PI / 180)), true);


        ret = get_segment_endpoint_trig(5, -180, &x, &y);
        zassert_equal(ret, 0);
        zassert_equal(float_equal(x, -5), true);
        zassert_equal(float_equal(y, 0), true);

        ret = get_segment_endpoint_trig(5, -0.5, &x, &y);
        zassert_equal(ret, 0);
        zassert_equal(float_equal(x, 5 * cos(359.5 * M_PI / 180)), true);
        zassert_equal(float_equal(y, 5 * sin(359.5 * M_PI / 180)), true);

        ret = get_segment_endpoint_trig(5, -361, &x, &y);
        zassert_equal(ret, 0);
        zassert_equal(float_equal(x, 5 * cos(359 * M_PI / 180)), true);
        zassert_equal(float_equal(y, 5 * sin(359 * M_PI / 180)), true);
}

ZTEST(map_utils, test_rectangle_intersect)
{
        struct rectangle rect = {
                .bottom = {
                        .x1 = 0,
                        .y1 = 0,
                        .x2 = 4,
                        .y2 = 0
                },
                .top = {
                        .x1 = 0,
                        .y1 = 4,
                        .x2 = 4,
                        .y2 = 4
                },
                .left = {
                        .x1 = 0,
                        .y1 = 0,
                        .x2 = 0,
                        .y2 = 4
                },
                .right = {
                        .x1 = 4,
                        .y1 = 0,
                        .x2 = 4,
                        .y2 = 4
                }
        };
        struct segment s;

        /* Segment exactly aligns with edge */
        set_segment(&s, 0, 0, 4, 0);
        zassert_equal(check_segment_rectangle_collisions(s, rect), true);

        /* Segment intersects one edge */
        set_segment(&s, -1, 2, 2, 2);
        zassert_equal(check_segment_rectangle_collisions(s, rect), true);

        /* Segment misses rectangle */
        set_segment(&s, -5, -5, -1, -1);
        zassert_equal(check_segment_rectangle_collisions(s, rect), false);

        /* Segment intersects two edges */
        set_segment(&s, -1, 2, 5, 2);
        zassert_equal(check_segment_rectangle_collisions(s, rect), true);
}

ZTEST(map_utils, test_segment_intersect)
{
        struct segment s1;
        struct segment s2;

        /* intersect */
        set_segment(&s1, 50, 50, 200, 200);
        set_segment(&s2, 200, 50, 50, 200);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* Parallel */
        set_segment(&s1, 50, 50, 200, 50);
        set_segment(&s2, 50, 100, 200, 100);
        zassert_equal(check_segment_segment_intersect(s1, s2), false);

        /* Collinear, overlapping */
        set_segment(&s1, 50, 50, 200, 50);
        set_segment(&s2, 200, 50, 350, 50);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* Collinear, no overlap, with negative coordinates */
        set_segment(&s1, 0, 0, 200, 0);
        set_segment(&s2, 250, 0, 300, 0);
        zassert_equal(check_segment_segment_intersect(s1, s2), false);

        /* Collinear, no overlap, with negative coordinates */
        set_segment(&s1, 250, 0, 300, 0);
        set_segment(&s2, 10, 0, 240, 0);
        zassert_equal(check_segment_segment_intersect(s1, s2), false);

        /* Collinear, not overlapping */
        set_segment(&s1, 50, 50, 100, 100);
        set_segment(&s2, 200, 200, 250, 250);
        zassert_equal(check_segment_segment_intersect(s1, s2), false);

        /* No Relation */
        set_segment(&s1, 50, 50, 100, 100);
        set_segment(&s2, 220, 200, 225, 220);
        zassert_equal(check_segment_segment_intersect(s1, s2), false);

        /* Intersection at tip */
        set_segment(&s1, 50, 50, 100, 100);
        set_segment(&s2, 100, 100, 150, 150);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* Intersection at tip in different direction */
        set_segment(&s1, 50, 50, 100, 100);
        set_segment(&s2, 100, 100, 150, 50);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* One segment within and collinear to the other */
        set_segment(&s1, 50, 50, 200, 50);
        set_segment(&s2, 100, 50, 150, 50);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* Coincidental segment (touching but not intersecting) */
        set_segment(&s1, 50, 50, 200, 50);
        set_segment(&s2, 200, 50, 350, 50);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* Intersecting as line, but not segment */
        set_segment(&s1, 50, 50, 100, 100);
        set_segment(&s2, 200, 200, 250, 250);
        zassert_equal(check_segment_segment_intersect(s1, s2), false);

        /* Segment with Zero Length (point) */
        set_segment(&s1, 50, 50, 50, 50);
        set_segment(&s2, 50, 50, 200, 50);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* Larger coordinates */
        set_segment(&s1, 0, 0, 500, 500);
        set_segment(&s2, 500, 0, 0, 500);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* Intersection with negative coordinates */
        set_segment(&s1, -50, -50, 200, 200);
        set_segment(&s2, 200, -50, -50, 200);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* Parallel with negative coordinates */
        set_segment(&s1, -50, -50, 200, -50);
        set_segment(&s2, -50, -100, 200, -100);
        zassert_equal(check_segment_segment_intersect(s1, s2), false);

        /* Collinear, no overlap, with negative coordinates */
        set_segment(&s1, -25, -50, 200, -50);
        set_segment(&s2, -50, -50, -75, -50);
        zassert_equal(check_segment_segment_intersect(s1, s2), false);

        /* Collinear, reverse overlap, with negative coordinates */
        set_segment(&s1, -100, -50, 200, -50);
        set_segment(&s2, -50, -50, -75, -50);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* No relation, with negative coordinates */
        set_segment(&s1, -50, -50, 100, 100);
        set_segment(&s2, 200, 200, 250, 250);
        zassert_equal(check_segment_segment_intersect(s1, s2), false);

        /* Intersection at tip, collinear, with negative coordinates */
        set_segment(&s1, -50, -50, -100, -100);
        set_segment(&s2, -100, -100, -150, -150);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* Intersection at tip, not collinear, with negative coordinates */
        set_segment(&s1, -50, -50, -100, -100);
        set_segment(&s2, -100, -100, 150, -50);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* Segment completely inside the other, with negative coordinates */
        set_segment(&s1, -150, -50, 200, -50);
        set_segment(&s2, -100, -50, 150, -50);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* Coincidental segment touching but not intersecting, with negative coordinates */
        set_segment(&s1, -50, -50, 200, -50);
        set_segment(&s2, 200, -50, 350, -50);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* Intersection outside of segment range, with negative coordinates */
        set_segment(&s1, -50, -50, 100, 100);
        set_segment(&s2, 200, 200, 250, 250);
        zassert_equal(check_segment_segment_intersect(s1, s2), false);

        /* Segment with zero length, with negative coordinates */
        set_segment(&s1, -50, -50, -50, -50);
        set_segment(&s2, -50, -50, 100, -50);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);

        /* Large coordinates, with negative coordinates */
        set_segment(&s1, -1000, -1000, -500, -500);
        set_segment(&s2, -500, -500, 500, -500);
        zassert_equal(check_segment_segment_intersect(s1, s2), true);
}

ZTEST_SUITE(map_utils, NULL, NULL, NULL, NULL, NULL);
