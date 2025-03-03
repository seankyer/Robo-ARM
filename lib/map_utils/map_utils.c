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

struct segment translate_segment(struct segment segment, double magnitude)
{
	/* Compute the direction vector */
	double dx = segment.x2 - segment.x1;
	double dy = segment.y2 - segment.y1;

	/* Compute the length of the vector */
	double length = sqrt((dx * dx) + (dy * dy));

	/* Compute the shift vector */
	double x_shift = magnitude * (-dy / length);
	double y_shift = magnitude * (dx / length);

	/* Perform shift */
	struct segment translatedSeg = {.x1 = segment.x1 + x_shift,
					.y1 = segment.y1 + y_shift,
					.x2 = segment.x2 + x_shift,
					.y2 = segment.y2 + y_shift};

	return translatedSeg;
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
			if (fmax(s1.x1, s1.x2) >= fmin(s2.x1, s2.x2) &&
			    fmax(s2.x1, s2.x2) >= fmin(s1.x1, s1.x2) &&
			    fmax(s1.y1, s1.y2) >= fmin(s2.y1, s2.y2) &&
			    fmax(s2.y1, s2.y2) >= fmin(s1.y1, s1.y2)) {
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

int get_arm_endpoint(double theta0, double theta1, double len, double range, double origin_x,
		     double origin_y, double *end_x, double *end_y)
{
	int ret;

	double x0_delta;
	double y0_delta;
	double x1_delta;
	double y1_delta;

	/*
	 * This gets the endpoint assuming origin of 0
	 */
	ret = get_segment_endpoint_trig(len, (double)theta0, &x0_delta, &y0_delta);
	if (ret) {
		return ret;
	}

	/*
	 * Assuming theta1 is perpendicular to theta0,
	 * we calculate on an axis aligned basis. Thus,
	 * we minus 90 degrees from theta0 and add to
	 * theta1.
	 */
	ret = get_segment_endpoint_trig(len, (double)theta1 + (theta0 - 90), &x1_delta, &y1_delta);
	if (ret) {
		return ret;
	}

	*end_x = origin_x + x0_delta + x1_delta;
	*end_y = origin_y + y0_delta + y1_delta;

	return 0;
}
