#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <libc.h>

//상호 채팅이 가능하게 함 : client도 listen호출
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

    int bytesRead;
    char readBuf[1000];
    char writeBuf[1000];
    while(1){
        int clntSock = accept(servSock, (struct sockaddr*)&clntAddr, (socklen_t *) sizeof(clntAddr));
        //accept함수가 clntAddr에 값 넣어줌.
        printf("[TCP클라이언트 정보] 클라이언트 IP주소:%s, 클라이언트 PORT번호:%d", inet_ntoa(clntAddr.sin_addr), clntAddr.sin_port);
        printf("클라이언트의 입력을 대기 중...\n");
        bytesRead=read(clntSock,readBuf,sizeof(readBuf));
        if(bytesRead != 0) {
            readBuf[bytesRead] = '\0';
            printf("입력받은 내용은: %s\n", readBuf);
            memset(readBuf, 0, sizeof(readBuf));
            printf("출력받을 내용을 입력: ");
            scanf("%s", writeBuf);
            while(getchar())
        }
        else{

        }
    }
    //그냥 강노 서버코드에서, 서버에서 입력받고, 클라이언트쪽으로 send해주는 함수를 사용하면 될 것이다.
    //recvn은 뭘까?
}
