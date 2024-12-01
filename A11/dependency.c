#include <stdio.h>
#include "tree.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

/**
 * A program that uses threads to process files and build a dependency tree.
 *
 * This program divides files among threads, extracts dependencies from each 
 * file, and stores them in a binary search tree. Users can interactively query
 * dependencies or view the processed files.
 *
 * @author: Tianyun Song
 * @version: December 1, 2024
 */

// Mutex for thread-safe tree access
pthread_mutex_t tree_mutex;

// Dependency structures
struct dependency_node {
    char dependency[64];
    struct dependency_node* next;
};

struct dependency_map {
    char file_name[64];
    struct dependency_node* dependencies;
};

struct dependency_map dep_map[1024]; // Up to 1024 files
int dep_count = 0;

/**
 * Finds or creates a dependency list for the specified file.
 *
 * @param file_name Name of the file to search for
 * @return Pointer to the list of dependencies for the file
 */
struct dependency_node** find_or_create_dependencies(const char* file_name) {
    for (int i = 0; i < dep_count; i++) {
        if (strcmp(dep_map[i].file_name, file_name) == 0) {
            return &dep_map[i].dependencies;
        }
    }

    // Create new entry
    strcpy(dep_map[dep_count].file_name, file_name);
    dep_map[dep_count].dependencies = NULL;
    return &dep_map[dep_count++].dependencies;
}

/**
 * Adds a dependency to the dependency list for a file.
 *
 * @param file_name Name of the file
 * @param dependency Dependency to add
 */
void add_dependency(const char* file_name, const char* dependency) {
    struct dependency_node** deps = find_or_create_dependencies(file_name);

    // Check for duplicates
    struct dependency_node* current = *deps;
    while (current) {
        if (strcmp(current->dependency, dependency) == 0)
            return; // Dependency already exists
        if (!current->next) break; // Stop at the end of the list
        current = current->next;
    }

    // Create a new dependency
    struct dependency_node* new_dep = malloc(sizeof(struct dependency_node));
    strcpy(new_dep->dependency, dependency);
    new_dep->next = NULL;

    // Append to the end of the list
    if (current) {
        current->next = new_dep;
    } else {
        *deps = new_dep; // First dependency in the list
    }
}

/**
 * Frees the dependency list for a file.
 *
 * @param deps Pointer to the dependency list
 */
void free_dependencies(struct dependency_node* deps) {
    while (deps) {
        struct dependency_node* next = deps->next;
        free(deps);
        deps = next;
    }
}


/**
 * Parses a file to extract dependencies from #include statements.
 *
 * @param file_name Name of the file to parse
 */
void parse_file(const char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char* include = strstr(line, "#include");
        if (include) {
            char dependency[64];

            // Match system headers
            if (sscanf(include, "#include <%63[^>]>",
                       dependency) == 1) {
                add_dependency(file_name, dependency);
            }
            // Match local headers
            else if (sscanf(include, "#include \"%63[^\"]\"",
                            dependency) == 1) {
                add_dependency(file_name, dependency);
            }
        }
    }

    fclose(file);
}

// Structure to store thread arguments
struct thread_args {
    char** files;
    int start;
    int end;
    struct tree_node** root;
};

/**
 * Thread function to process files and build the dependency tree.
 *
 * @param args Pointer to thread arguments
 * @return NULL
 */
void* thread_function(void* args) {
    struct thread_args* targs = args;

    for (int i = targs->start; i < targs->end; i++) {
        const char* file_name = targs->files[i];

        // Validate file before processing
        FILE* test_file = fopen(file_name, "r");
        if (!test_file) {
            perror("Skipping invalid file");
            continue;
        }
        fclose(test_file);

        // Insert the file into the tree
        pthread_mutex_lock(&tree_mutex);
        *targs->root = insert(file_name, *targs->root);
        pthread_mutex_unlock(&tree_mutex);

        // Parse the file for dependencies
        parse_file(file_name);
    }

    return NULL;
}

/**
 * Command-line prompt to interact with the dependency tree.
 *
 * @param root Root of the dependency tree
 */
void command_prompt(struct tree_node* root) {
    char command[256];
    while (1) {
        printf("> ");
        if (!fgets(command, sizeof(command), stdin)) break;

        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "quit") == 0) {
            break;
        } else if (strcmp(command, "list") == 0) {
            printSorted(root);
        } else {
            struct tree_node* node = find(command, root);
            if (node) {
                printf("%s has the following dependencies:\n", node->data.name);
                struct dependency_node* deps = *find_or_create_dependencies(node->data.name);
                while (deps) {
                    printf("  %s\n", deps->dependency);
                    deps = deps->next;
                }
            } else {
                printf("%s not found\n", command);
            }
        }
    }
}

/**
 * Frees all dependencies in the dependency map.
 */
void free_all_dependencies() {
    for (int i = 0; i < dep_count; i++) {
        if (dep_map[i].dependencies) {
            free_dependencies(dep_map[i].dependencies);
            dep_map[i].dependencies = NULL; // Nullify pointer to prevent dangling references
        }
    }
    dep_count = 0; // Reset count after freeing all dependencies
}

/**
 * Main driver function to execute the program.
 *
 * @param argc Number of command-line arguments
 * @param argv Command-line arguments
 * @return 0 on success, 1 on failure
 */
int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <num_threads> <find command> <find args> ...\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        fprintf(stderr, "Invalid number of threads\n");
        return 1;
    }

    // Build the find command
    char find_command[1024] = {0};
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], ".h") == 0 || strcmp(argv[i], ".cpp") == 0) {
            strcat(find_command, "\"*");
            strcat(find_command, argv[i] + 1); // Skip the leading dot
            strcat(find_command, "\"");
        } else {
            strcat(find_command, argv[i]);
        }
        strcat(find_command, " ");
    }

    // debug println
    // printf("Executing find command: %s\n", find_command);

    // Execute the find command and capture output
    FILE* find_output = popen(find_command, "r");
    if (!find_output) {
        perror("Failed to execute find command");
        return 1;
    }

    // Read filenames into a dynamic array
    char** file_list = malloc(1024 * sizeof(char*)); // Allocate space for up to 1024 files
    int file_count = 0;
    char file_name[256];
    while (fgets(file_name, sizeof(file_name), find_output)) {
        file_name[strcspn(file_name, "\n")] = '\0'; // Remove newline
        // printf("Found file: %s\n", file_name); // Debug print
        file_list[file_count] = strdup(file_name); // Duplicate the filename
        file_count++;
    }

    pclose(find_output);

    if (file_count == 0) {
        fprintf(stderr, "No files found. Check the find command or file paths.\n");
        free(file_list);
        return 1;
    }

    printf("Processing %d files\n", file_count);

    // Initialize timing
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Allocate threads and thread args
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    struct thread_args* args = malloc(num_threads * sizeof(struct thread_args));
    if (!threads || !args) {
        perror("Failed to allocate memory");
        return 1;
    }

    struct tree_node* root = NULL;
    pthread_mutex_init(&tree_mutex, NULL);

    // Divide files among threads
    int files_per_thread = file_count / num_threads;
    int remainder = file_count % num_threads;

    int start_idx = 0;
    for (int i = 0; i < num_threads; i++) {
        args[i].files = file_list;
        args[i].start = start_idx;
        args[i].end = start_idx + files_per_thread + (i < remainder ? 1 : 0);
        args[i].root = &root;

        printf("Thread %d processing %d files (arguments %d to %d)\n", i,
               args[i].end - args[i].start, args[i].start + 2, args[i].end + 2);

        pthread_create(&threads[i], NULL, thread_function, &args[i]);
        start_idx = args[i].end;
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Calculate elapsed time
    gettimeofday(&end, NULL);
    double elapsed_time = (end.tv_sec - start.tv_sec) +
                          (end.tv_usec - start.tv_usec) / 1e6;
    printf("Elapsed time is %.6f\n", elapsed_time);

    // Start the command prompt
    command_prompt(root);

    // Cleanup
    for (int i = 0; i < file_count; i++) {
        free(file_list[i]);
    }
    free(file_list);
    free(threads);
    free(args);

    free_all_dependencies();
    clear(root);
    pthread_mutex_destroy(&tree_mutex);

    return 0;
}
