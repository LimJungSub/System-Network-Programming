#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int recvn(int s, char *buf, int len, int flag) {
    int received;
    char *ptr = buf;
    int left = len;
    while (left > 0) {
        received = (int)recv(s, ptr, left, 0);
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
    char* PORT = argv[1];

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[BUF_SIZE];

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("socket() error!");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(PORT));

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind() error!");
    }

    if (listen(server_sock, 1) == -1) {
        perror("listen() error!");
    }

    printf("클라이언트의 연결을 기다리는 중...\n");

    // 클라이언트 연결 수락, 이제 클라이언트 소켓으로 통신
    // accept의 세번 째 인자값을 주소형태로 전달해야함에 주의하자. (커넥트는 그냥 숫자로 전달해도됨) ... 이유는 원노트에 기록
    addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
    //;; CLion이 추천해준대로, socklen_t*로 캐스팅하여 전송하지 말자, 자꾸 에러난다. 위 처럼 &addr_size로 전송
    if (client_sock == -1) {
        perror("accept() error!");
    }

    printf("[클라이언트 연결]\n");

    // 채팅 시작
    //서버쪽에서 fin이 들어오기전 (recv의 리턴값이 0이기 이전)까지 계속 받음
    while(1) {
        memset(buffer, 0, BUF_SIZE); //recvBuf sendBuf따로 사용할 수도 있겠지만 초기화하여 재사용하자
        /***** 수신 *****/
        // 데이터 크기를 먼저 읽음
        uint32_t net_data_size;
        recv(client_sock, &net_data_size, sizeof(net_data_size), 0);
        int data_size = ntohl(net_data_size); // 네트워크 바이트 순서를 호스트 바이트 순서로 변환
        if(data_size == 1){ // 클라이언트로부터 \n만 입력된 상황
            break;
        }

        // 실제 메시지 수신
        int retval = recvn(client_sock, buffer, data_size, 0);
        if (retval == -1) {
            perror("recv() error!");
            continue;
        }
        else if (retval == 0){
            break;
        }
        // 수신 내용 출력
        printf("\n[Client]: %s", buffer);
        printf("\nbytes recved:%d\n",retval);

        // 메시지를 클라이언트로 부터 받은 후,
        // 서버에서 클라이언트로 메시지 전송
        //(클라->서버 메시지 전송 상황) 클라이언트가 서버에 send하고 서버가 recv로 읽듯,
        //(서버->클라 메시지 전송 상황) 마찬가지로 서버가 클라이언트에 send하고, 클라이언트에선 recv를 통해 읽으면 된다.

        memset(buffer, 0, BUF_SIZE); //recvBuf sendBuf따로 사용할 수도 있겠지만 초기화하여 재사용하자
        /***** 발신 *****/
        printf("\n[Server][to Client]: ");

        char *string = fgets(buffer, BUF_SIZE, stdin);
        // 데이터 크기를 먼저 보냄
        data_size = strlen(string);
        net_data_size = htonl(data_size); // 호스트 바이트 순서를 네트워크 바이트 순서로 변환

        // 실제 메시지 전송
        send(client_sock, &net_data_size, sizeof(net_data_size), 0);
        printf("\nbytes send:%lu\n",strlen(buffer));
        send(client_sock, buffer, data_size, 0);
    }

    // 소켓 종료
    close(client_sock);
    printf("\n[TCP서버] 클라이언트 종료: IP주소=%s, 클라이언트 포트번호=%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    close(server_sock);

    return 0;
}
