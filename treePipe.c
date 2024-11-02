#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void fork_and_exec(const char *program, int input, int *result) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        // Redirect stdout to the pipe
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        char input_str[10];
        snprintf(input_str, sizeof(input_str), "%d", input);

        execl(program, program, input_str, (char *)NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else { // Parent process
        close(pipefd[1]);
        wait(NULL);
        char buffer[128];
        int bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            *result = atoi(buffer);
        } else {
            *result = input; // If there's an error, use the input as default
        }
        close(pipefd[0]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <current depth> <max depth> <left-right>\n", argv[0]);
        return 1;
    }

    int curDepth = atoi(argv[1]);
    int maxDepth = atoi(argv[2]);
    int lr = atoi(argv[3]);

    int num1 = 0;
    if (curDepth == 0) {
        printf("> Current depth: %d, lr: %d\n", curDepth, lr);
        printf("Please enter num1 for the root: ");
        fflush(stdout);
        scanf("%d", &num1);
    }

    if (curDepth == maxDepth) {
        printf("> My num1 is: %d\n", num1);
        printf("> My result is: %d\n", num1 + 1);
        return 0;
    }

    // Continue post-order traversal logic
    if (curDepth < maxDepth) {
        int left_result = 0, right_result = 0;

        // Process left child
        printf("---> Current depth: %d, lr: %d\n", curDepth + 1, 0);
        fork_and_exec("./left", num1, &left_result);
        printf("---> Left child result: %d\n", left_result);

        // Process right child
        printf("---> Current depth: %d, lr: %d\n", curDepth + 1, 1);
        fork_and_exec("./right", left_result, &right_result);
        printf("---> Right child result: %d\n", right_result);

        // Calculate final result for the current node
        int final_result = left_result * right_result;
        printf("> Current depth: %d, lr: %d, my num1: %d, my num2: %d\n", curDepth, lr, left_result, right_result);
        printf("> My result is: %d\n", final_result);

        if (curDepth == 0) {
            printf("The final result is: %d\n", final_result);
        }
    }

    return 0;
}