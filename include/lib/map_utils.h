/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MAP_UTILS_H_
#define MAP_UTILS_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Struct defining a line segment
 */
struct segment {
        double x1; /** origin x coordinate */
        double y1; /** origin y coordinate */
        double x2; /** ending x coordinate */
        double y2; /** ending y coordinate */
};

/**
 * @brief Struct defining an axis-aligned rectangle
 */
struct rectangle {
        struct segment bottom; /** bottom segment of rectangle */
        struct segment top; /** top segment of rectangle */
        struct segment left; /** left segment of rectangle */
        struct segment right; /** right segment of rectangle */
};

/**
 * @brief Determine if two segments intersect
 *
 * Define the following vectors:
 *   Segment1: A = (x1, y1), B = (x2, y2), AB (x2 - x1, y2 - y1)
 *   Segment2: C = (x3, y3), D = (x4, y4), CD (x4 - x3, y4 - y3)
 *
 * Any point on AB: A + xAB  where x is Real Number [0, 1]
 * Any point on CD: C + zCD  where z is Real Number [0, 1]
 *
 * We must find the value of x and z that match the same point on
 * AB and CD.
 *
 * Use system of equations for `A + xAB = C + zCD` solving for x and z
 *
 *     (x4 - x3)(y3 - y1) - (y4 - y3)(x3 - x1)   a
 * x = --------------------------------------- = -
 *     (x4 - x3)(y2 - y1) - (y4 - y3)(x2 - x1)   b
 *
 *     (x2 - x1)(y3 - y1) - (y2 - y1)(x3 - x1)   c
 * z = --------------------------------------- = -
 *     (x4 - x3)(y2 - y1) - (y4 - y3)(x2 - x1)   b
 *
 * Evaluate the cases:
 *   if b == 0 then AB || CD are parallel
 *   if a == b == 0 then AB and CD are collinear
 *   if 0 < x && z < 1 two line segments intersect
 *   otherwise, two segments do not intersect
 *
 * Reference: https://www.youtube.com/watch?v=bvlIYX9cgls&ab_channel=EdgarProgrammator
 *
 * @param[in] segment1 the first segment
 * @param[in] segment2 the second segment
 *
 * @return true if collision, false otherwise
 */
bool check_segment_segment_intersect(struct segment segment1, struct segment segment2);

/**
 * @brief Returns if segment collides with a rectangle
 *
 * @param[in] segment the subject line segment
 * @param[in] rectangle the rectangle we are concerned of collisions with
 *
 * @return true if collides, false if no collision
 */
bool check_segment_rectangle_collisions(struct segment segment, struct rectangle rectangle);

#endif /* MAP_UTILS_H_ */
