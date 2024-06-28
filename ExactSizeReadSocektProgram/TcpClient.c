#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

int recvn(int s, char *buf, int len, int flag) {
    int received;
    char *ptr = buf;
    int left = len;
    while (left > 0) {
        received = (int)recv(s, ptr, left,0);
        if (received == -1)
            return -1;
        else if (received == 0)
            break;
        left -= received;
        ptr += received;

    }
    return (len - left);
}

int main(int argc, char *argv[]) {
    char *PORT = argv[1];

    struct sockaddr_in serv_addr;
    int sock = 0;
    char sendbuffer[BUF_SIZE] = {0};
    char recvbuffer[BUF_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() error!");
    }

    //루프백 주소는 따로 commandline argument로 받지 않고 상수로 사용하자.
    char *const localAddr = "127.0.0.1";
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(PORT));
    serv_addr.sin_addr.s_addr = inet_addr(localAddr);

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
        perror("connection() error!\n");
    }

    printf("[서버에 연결 완료]\n");

    //(클라->서버 메시지 전송 상황) 클라이언트가 서버에 send하고 서버가 recv로 읽듯,
    //(서버->클라 메시지 전송 상황) 마찬가지로 서버가 클라이언트에 send하고, 클라이언트에선 recv를 통해 읽으면 된다.
    while (1) {
        /***** 발신 *****/
        printf("[Client][to Server]: ");
        char *string = fgets(sendbuffer, BUF_SIZE, stdin);
        // 클라이언트가 exit 명령 전송 시 break하여 while문 빠져나올 시 바로 다음의 close로 fin 전송
        if (strcmp(string, "exit\n") == 0) {
            break;
        }
        // 데이터 크기를 먼저 보냄
        int data_size = strlen(string);
        uint32_t net_data_size = htonl(data_size); // 호스트 바이트 순서를 네트워크 바이트 순서로 변환
        send(sock, &net_data_size, sizeof(net_data_size), 0);
        // 데이터를 보냄
        int retval = (int)send(sock, sendbuffer, data_size, 0);
        printf("bytes send:%d\n", retval);

        //memset(buffer, 0, BUF_SIZE); //recvBuf sendBuf따로 사용할 수도 있겠지만 초기화하여 재사용하자
        //EchoClient코드 구현 중 문제 : recvBuffer의 크기가 0으로 전달됨. 즉 미리 데이터의 크기를 알아야함.
        //printf("recvBufferSize:%d",strlen(recvbuffer));

        /***** 수신 *****/
        // 데이터 크기를 먼저 읽음
        recv(sock, &net_data_size, sizeof(net_data_size), 0);
        data_size = ntohl(net_data_size); // 네트워크 바이트 순서를 호스트 바이트 순서로 변환

        // 실제 메시지 수신 (recvn사용)
        retval = recvn(sock, recvbuffer, data_size, 0);

        printf("\nbytes recved:%d",retval);
        //break하여 while문 빠져나올 시 바로 다음의 close로 fin 전송
        if(retval == 0)
            break;

        printf("\n[Server]: %s\n", recvbuffer);
    }

    close(sock);
    return 0;
}

