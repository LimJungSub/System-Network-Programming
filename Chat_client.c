#include <libc.h>


int main(int argc, char *argv[]) {
    char *sIp = argv[1];
    char *sPort = argv[2];
    struct sockaddr_in sAddr;

    memset(&sAddr, 0, sizeof(sAddr));
    sAddr.sin_family = AF_INET;
    sAddr.sin_addr.s_addr = inet_addr(sIp);
    sAddr.sin_port = htons(atoi(sPort));

    int clntSock = socket(PF_INET, SOCK_STREAM, 0);

    if (connect(clntSock, (struct sockaddr *) &sAddr, sizeof(sAddr)) != -1) {
        printf("[connect 성공] 접속포트(서버): %s\n", sPort);
    } else {
        perror("connect 에러 발생\n");
    }

    while (1) {
        char writeBuf[1000], readBuf[1000];
        //서버와 클라이언트 모두 일관되게, 메시지를 받은 것 부터 출력 후 전송할 메시지 입력 프롬프트 띄우게 구현한다.
        int bytesRead = read(clntSock, readBuf, sizeof(readBuf));
        if(bytesRead!=-1){
            printf("[서버]: %s", readBuf);
        }
        printf("[to 서버]: ");
        fgets(writeBuf, sizeof(writeBuf), stdin);
        if (strcmp(writeBuf, "exit") == 0) {
            close(clntSock); //close()호출 시 서버가 받는 read()의 반환값은 0
        }
        else {
            write(clntSock, writeBuf, sizeof(writeBuf));
        }
    }


}