/**
 * @file graphctl.c
 * @brief GraphOS Native Command-Line Control Interface
 *
 * graphctl is the official user-space control binary for GraphOS. Rather than 
 * relying on legacy POSIX hierarchical file manipulation binaries (like mkdir, 
 * ln, or rm), graphctl provides a vocabulary explicitly designed for graph operations.
 *
 * It communicates with the GraphFS engine via a virtual control channel (`/.graph_cmd`).
 * This channel allows user-space programs to trigger kernel-level graph cascades,
 * such as the SEVER_ALL signal, which cannot be represented via standard VFS syscalls.
 * 
 * Features:
 * - Direct Node Instantiation (spawn-dir, spawn-file)
 * - Raw Edge Creation (draw-edge) for zero-copy file linking
 * - Granular Relationship Destruction (sever-edge)
 * - Global Node Annihilation (sever-all-edges)
 * - Live State Introspection (inspect-graph)
 *
 * @author GraphOS Development Team
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define MOUNT_POINT "/tmp/graphfs_mount"

void print_usage() {
    printf("GraphOS Control Interface (graphctl)\n");
    printf("Usage:\n");
    printf("  graphctl spawn-dir <path>             : Create a DIRECTORY node\n");
    printf("  graphctl spawn-file <path> <content>  : Create a FILE node with content\n");
    printf("  graphctl read-node <path>             : Read content of a FILE node\n");
    printf("  graphctl draw-edge <src> <dest>       : Draw a CONTAINS edge to an existing node\n");
    printf("  graphctl sever-edge <path>            : Sever an edge (unlinks a node)\n");
    printf("  graphctl sever-all-edges <path>       : Forcefully destroy a node and ALL its incoming edges globally\n");
    printf("  graphctl destroy-node <path>          : Destroy a DIRECTORY node\n");
    printf("  graphctl inspect-graph                : View the live internal graph state\n");
}

void build_path(char *dest, const char *subpath) {
    if (strcmp(subpath, ".graph") == 0) {
        snprintf(dest, 1024, "%s/.graph", MOUNT_POINT);
    } else {
        snprintf(dest, 1024, "%s/%s", MOUNT_POINT, subpath[0] == '/' ? subpath + 1 : subpath);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    char target[1024];

    if (strcmp(argv[1], "spawn-dir") == 0 && argc == 3) {
        build_path(target, argv[2]);
        if (mkdir(target, 0755) == 0) {
            printf("[graphctl] Successfully spawned DIRECTORY node at '%s'\n", argv[2]);
        } else {
            perror("[graphctl] Error spawning DIRECTORY node");
        }
    } 
    else if (strcmp(argv[1], "spawn-file") == 0 && argc >= 3) {
        build_path(target, argv[2]);
        FILE *f = fopen(target, "w");
        if (f) {
            if (argc == 4) fputs(argv[3], f);
            fclose(f);
            printf("[graphctl] Successfully spawned FILE node at '%s'\n", argv[2]);
        } else {
            perror("[graphctl] Error spawning FILE node");
        }
    }
    else if (strcmp(argv[1], "read-node") == 0 && argc == 3) {
        build_path(target, argv[2]);
        FILE *f = fopen(target, "r");
        if (f) {
            char buffer[1024];
            printf("[graphctl] Data in node '%s':\n", argv[2]);
            while (fgets(buffer, sizeof(buffer), f) != NULL) {
                printf("%s", buffer);
            }
            if (buffer[strlen(buffer)-1] != '\n') printf("\n");
            fclose(f);
        } else {
            perror("[graphctl] Error reading node");
        }
    }
    else if (strcmp(argv[1], "draw-edge") == 0 && argc == 4) {
        char src[1024], dest[1024];
        build_path(src, argv[2]);
        build_path(dest, argv[3]);
        if (link(src, dest) == 0) {
            printf("[graphctl] Successfully drew CONTAINS edge from '%s' to node at '%s'\n", argv[3], argv[2]);
        } else {
            perror("[graphctl] Error drawing edge");
        }
    }
    else if (strcmp(argv[1], "sever-edge") == 0 && argc == 3) {
        build_path(target, argv[2]);
        if (unlink(target) == 0) {
            printf("[graphctl] Successfully severed edge at '%s'\n", argv[2]);
        } else {
            perror("[graphctl] Error severing edge");
        }
    }
    else if (strcmp(argv[1], "destroy-node") == 0 && argc == 3) {
        build_path(target, argv[2]);
        if (rmdir(target) == 0) {
            printf("[graphctl] Successfully destroyed DIRECTORY node '%s'\n", argv[2]);
        } else {
            perror("[graphctl] Error destroying node");
        }
    }
    else if (strcmp(argv[1], "inspect-graph") == 0) {
        build_path(target, ".graph");
        FILE *f = fopen(target, "r");
        if (f) {
            char buffer[1024];
            while (fgets(buffer, sizeof(buffer), f) != NULL) {
                printf("%s", buffer);
            }
            fclose(f);
        } else {
            perror("[graphctl] Error: GraphFS is not mounted or accessible");
        }
    }
    else if (strcmp(argv[1], "sever-all-edges") == 0 && argc == 3) {
        char cmd_path[1024];
        snprintf(cmd_path, 1024, "%s/.graph_cmd", MOUNT_POINT);
        FILE *f = fopen(cmd_path, "w");
        if (f) {
            char internal_path[1024];
            snprintf(internal_path, 1024, "%s%s", argv[2][0] == '/' ? "" : "/", argv[2]);
            fprintf(f, "SEVER_ALL %s", internal_path);
            fclose(f);
            printf("[graphctl] Successfully broadcast SEVER_ALL signal for node at '%s'\n", argv[2]);
        } else {
            perror("[graphctl] Error communicating with GraphFS engine");
        }
    }
    else {
        printf("Unknown command or missing arguments.\n\n");
        print_usage();
    }

    return 0;
}
