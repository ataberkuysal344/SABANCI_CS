#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void fork_and_exec(const char *program, int input1, int input2, int *result) {
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

        // Redirect stdin from the pipe
        int pipefd_in[2];
        if (pipe(pipefd_in) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        pid_t pid_in = fork();
        if (pid_in == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid_in == 0) { // Child process for input redirection
            close(pipefd_in[0]);
            dup2(pipefd_in[1], STDIN_FILENO);
            close(pipefd_in[1]);

            char input1_str[10], input2_str[10];
            snprintf(input1_str, sizeof(input1_str), "%d", input1);
            snprintf(input2_str, sizeof(input2_str), "%d", input2);

            printf("%s\n%s\n", input1_str, input2_str);
            exit(EXIT_SUCCESS);
        } else { // Parent process for input redirection
            close(pipefd_in[1]);
            wait(NULL);
            dup2(pipefd_in[0], STDIN_FILENO);
            close(pipefd_in[0]);

            execl(program, program, (char *)NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        }
    } else { // Parent process
        close(pipefd[1]);
        wait(NULL);
        char buffer[128];
        int bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            *result = atoi(buffer);
        } else {
            *result = input1; // If there's an error, use the input as default
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
        fork_and_exec("./left", num1, num1, &left_result);
        printf("---> Left child result: %d\n", left_result);

        // Process right child
        printf("---> Current depth: %d, lr: %d\n", curDepth + 1, 1);
        fork_and_exec("./right", left_result, left_result, &right_result);
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