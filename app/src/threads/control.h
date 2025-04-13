/*
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief Control job
 */
typedef struct {
	int x_coord; /**< Requested X coordinate */
	int y_coord; /**< Requested Y coordinate */
        bool demo; /**< If demo is requested */
} control_job_t;

/**
 * @brief Submit coordinates request to controller
 *
 * This function blocks until the job is accepted by the control
 * thread.
 *
 * @param[in] job Job struct containing the requested coordinates
 */
int control_submit_job(control_job_t job);
