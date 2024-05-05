#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <libc.h>


int main(int argc, char* argv[]) {

    char* port = argv[1];

    struct sockaddr_in servAddr, clntAddr;
    memset(&servAddr, 0, sizeof(servAddr));

    servAddr.sin_port = htonl(atoi(port));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int servSock = socket(PF_INET, SOCK_STREAM, 0);
    bind(servSock, (struct sockaddr*)&servAddr,sizeof(servSock));
    //서버쪽에서도 이전과 마찬가지로 fin이 들어오기전 (read의 리턴값이 0이기 이전)까지 계속 받음
    listen(servSock,5);


    while(1){
        char readBuf[1000], writeBuf[1000];
        int bytesRead;

        int clntSock = accept(servSock, (struct sockaddr*)&clntAddr, (socklen_t *) sizeof(clntAddr));
        if(clntSock == -1){
            perror("서버쪽에서 클라이언트 accept실패\n");
        }
        //accept함수가 clntAddr에 값 넣어줌.
        printf("[TCP클라이언트 정보] 클라이언트 IP주소:%s, 클라이언트 PORT번호:%d\n", inet_ntoa(clntAddr.sin_addr), clntAddr.sin_port);
        //accept가 3번째 인자값이 socklent_t*인것과 다르게, socklent_t다(이유 원노트)
        if(connect(servSock, (struct sockaddr*)&clntAddr, (socklen_t) sizeof(clntAddr)) == -1){
            perror("서버쪽에서 클라이언트에 connect실패\n");
        }
        //printf("클라이언트의 입력을 대기 중...\n");
        bytesRead=read(clntSock,readBuf,sizeof(readBuf));
        if(bytesRead != 0) {
            readBuf[bytesRead] = '\0';
            printf("[클라이언트]: %s\n", readBuf);
            memset(readBuf, 0, sizeof(readBuf));
        }
        else{
            printf("\nread()의 반환값 0, FIN도착");
        }
        printf("[to 클라이언트]: ");
        //scanf("%s", writeBuf); 공백 입력 처리를 위해 fgets활용해야
        fgets(writeBuf, sizeof(writeBuf), stdin);
        while(getchar()!= '\n'); //입력버퍼 비워주기
        //(클라->서버 메시지 전송 상황) 클라이언트가 서버에 write하고 서버가 read로 읽듯,
        //(서버->클라 메시지 전송 상황) 마찬가지로 서버가 클라이언트에 write하고, 클라이언트에선 read를 통해 읽으면 된다.
        write(clntSock, writeBuf, sizeof(readBuf));
    }
}
