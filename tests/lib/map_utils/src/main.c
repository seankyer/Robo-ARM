/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <lib/map_utils.h>

static void set_segment(struct segment *seg, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
        seg->x1 = (double)x1;
        seg->y1 = (double)y1;
        seg->x2 = (double)x2;
        seg->y2 = (double)y2;
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
