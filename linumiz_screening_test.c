#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#define PATH_MAX 4096

// Define a structure for the stack node
typedef struct Node {
    char path[PATH_MAX];
    struct Node* next;
} Node;

// Function to create a new node
Node* createNode(const char* path) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode) {
        strncpy(newNode->path, path, PATH_MAX - 1);
        newNode->path[PATH_MAX - 1] = '\0'; // Ensure null-termination
        newNode->next = NULL;
    } else {
        perror("malloc");
    }
    return newNode;
}

// Function to push a directory onto the stack
void pushd(Node** top, const char* path) {
    Node* newNode = createNode(path);
    if (newNode) {
        newNode->next = *top;
        *top = newNode;
    }
}

// Function to pop a directory from the stack
char* popd(Node** top) {
    if (*top == NULL) {
        return NULL;
    }
    Node* temp = *top;
    *top = (*top)->next;
    char* path = strdup(temp->path);
    if (!path) {
        perror("strdup");
    }
    free(temp);
    return path;
}

// Function to change directory and handle errors
int changeDirectory(const char* path) {
    if (chdir(path) != 0) {
        perror("chdir");
        return -1;
    }
    return 0;
}

// Function to print the stack (for debugging purposes)
void printStack(Node* top) {
    Node* current = top;
    while (current != NULL) {
        printf("%s\n", current->path);
        current = current->next;
    }
}

int main(int argc, char *argv[]) {
    static Node* stack = NULL;  // Initialize the stack

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [directory]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "pushd") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s pushd <directory>\n", argv[0]);
            return 1;
        }
        
        char currentDir[PATH_MAX];
        if (getcwd(currentDir, sizeof(currentDir)) == NULL) {
            perror("getcwd");
            return 1;
        }

        // Push current directory onto the stack
        pushd(&stack, currentDir);

        // Change to new directory
        if (changeDirectory(argv[2]) != 0) {
            return 1;
        }

        // Print current stack (for debugging purposes)
        printf("Directory stack:\n");
        printStack(stack);

    } else if (strcmp(argv[1], "popd") == 0) {
        if (argc != 2) {
            fprintf(stderr, "Usage: %s popd\n", argv[0]);
            return 1;
        }

        char* poppedDir = popd(&stack);
        if (poppedDir == NULL) {
            fprintf(stderr, "Directory stack is empty\n");
            return 1;
        }

        // Change to the popped directory
        if (changeDirectory(poppedDir) != 0) {
            free(poppedDir);
            return 1;
        }

        // Print current stack (for debugging purposes)
        printf("Directory stack:\n");
        printStack(stack);

        free(poppedDir);

    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return 1;
    }

    return 0;
}
