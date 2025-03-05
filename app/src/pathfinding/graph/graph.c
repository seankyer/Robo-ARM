/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
#include <graph.h>

LOG_MODULE_REGISTER(graph, LOG_LEVEL_INF);

#ifdef CONFIG_GRAPH_BFS
static int bfs(uint8_t(*graph)[CSPACE_DIMENSION], int start_x, int start_y, int sol_mark, struct pathfinding_steps (*path)[MAX_NUM_STEPS], int *num_steps)
{
        return -1;
}
#endif

int graph_path(uint8_t(*graph)[CSPACE_DIMENSION], int start_x, int start_y, int sol_mark, struct pathfinding_steps (*path)[MAX_NUM_STEPS], int *num_steps)
{
        #ifdef CONFIG_GRAPH_BFS
        return bfs(graph, start_x, start_y, sol_mark, path, num_steps);
        #endif

        LOG_ERR("No graphing algorithm chosen in Kconfig!");
        return -1;
}
