#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <lib/map_utils.h>

int get_segment_endpoint_trig(double hypotenuse, double angle_d, double *x, double *y)
{
        if (x == NULL || y == NULL) {
                return -EINVAL;
        }

        while (angle_d < 0) {
                angle_d += 360;
        }

        double angle_radians = fmod(angle_d, 360) * M_PI / 180.0;

        *y = hypotenuse * sin(angle_radians);
        *x = hypotenuse * cos(angle_radians);

        return 0;
}

bool check_segment_segment_intersect(struct segment s1, struct segment s2)
{
        double b = (s2.x2 - s2.x1) * (s1.y2 - s1.y1) - (s2.y2 - s2.y1) * (s1.x2 - s1.x1);
        double a = (s2.x2 - s2.x1) * (s2.y1 - s1.y1) - (s2.y2 - s2.y1) * (s2.x1 - s1.x1);
        double c = (s1.x2 - s1.x1) * (s2.y1 - s1.y1) - (s1.y2 - s1.y1) * (s2.x1 - s1.x1);

        /* Lines are parallel or collinear */
        if (b == 0) {
                /* Collinear check */
                if (a == 0) {
                        /* Do they overlap, even if they dont 'intersect'? */
                        if (fmax(s1.x1, s1.x2) >= fmin(s2.x1, s2.x2) && fmax(s2.x1, s2.x2) >= fmin(s1.x1, s1.x2) &&
                            fmax(s1.y1, s1.y2) >= fmin(s2.y1, s2.y2) && fmax(s2.y1, s2.y2) >= fmin(s1.y1, s1.y2)) {
                                return true;
                        }
                        /* Collinear with no overlap */
                        return false;
                }
                /* Parallel */
                return false;
        }

        /* Check if they properly intersect or not */
        double x = a / b;
        double z = c / b;

        if (x >= 0 && x <= 1 && z >= 0 && z <= 1) {
                /* Intersect at a point */
                return true;
        } else {
                /* No relation to one another */
                return false;
        }
}

bool check_segment_rectangle_collisions(struct segment segment, struct rectangle rectangle)
{
        bool collision = false;

        /* Check if any sides of the rectangle intersect with our segment */
        collision |= check_segment_segment_intersect(segment, rectangle.bottom);
        collision |= check_segment_segment_intersect(segment, rectangle.top);
        collision |= check_segment_segment_intersect(segment, rectangle.left);
        collision |= check_segment_segment_intersect(segment, rectangle.right);

        return collision;
}
