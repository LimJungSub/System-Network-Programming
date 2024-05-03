#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    char cmd[100];
    char *args[10];
    int should_run = 1;

    while (should_run) {
        char *cwd;
        cwd = getcwd(NULL, 100);
        printf("[%s]$ ", cwd);

        // 사용자로부터 명령어 입력
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
            printf("An error occurred while reading input.\n");
            continue;
        }

        // fgets를 통해 입력된 끝\n을 제거하는 로직; strcspn활용하자
        cmd[strcspn(cmd, "\n")] = '\0';

        // "exit" 입력 시 반복 종료
        if (strcmp(cmd, "exit") == 0) {
            should_run = 0;
            continue;
        }

        // 사용자입력을 공백 기준으로 토큰화하여 args에 저장
        int i = 0;
        args[i] = strtok(cmd, " ");
        while(args[i] != NULL) {
            args[++i] = strtok(NULL, " ");
        }
        args[i] = 0; //끝 인자는 항상 0(null)인 것 주의

        // 새 프로세스 생성
        pid_t pid = fork();

        if (pid < 0) { // fork 실패
            fprintf(stderr, "Fork Failed\n");
            return 1;
        } else if (pid == 0) { // 자식 프로세스
            if (execve(args[0], args, 0) == -1) {
                perror("execve 문제 발생");
            }
            //exit(0); 없어도 된다. 어차피 exec성공하면 아무 줄도 더 실행X
        } else { // 부모 프로세스
            wait(NULL); // 자식 프로세스가 종료될 때까지 대기
        }
    }

    printf("Bye\n");
    return 0;
}
