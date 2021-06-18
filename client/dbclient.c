#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>

#define PORT 8080

int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
  // jika gagal menyambungkan ke server manapun maka tampil "Connection Failed"
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // msg for server
    char input[1024];

    // msg from server
    char buffer[1024] = {0};
    char *berhasil = "berhasil";
    int login_user = 0;

    while(1){
        memset(buffer, 0, sizeof(buffer));
        if(geteuid() == 0) {
            send(sock, "c", 1, 0);

            read(sock, buffer, 1024);
            printf("%s\n", buffer);
            memset(buffer, 0, sizeof(buffer));

            do {
                scanf("\n");
                scanf("%[^\n]%*c", input);
                send(sock, input, strlen(input), 0);
                memset(input, 0, sizeof(input));
                
                memset(buffer, 0, sizeof(buffer));
                read(sock, buffer, 1024);
                printf("%s\n", buffer);
            } while(strcmp(buffer, "exit") != 0);
        
        break;

        } else {
            if (argc == 5 && strcmp(argv[1], "-u") == 0 && strcmp(argv[3], "-p") == 0){
                send(sock, "l", 1, 0);

                read(sock, buffer, 1024);
                printf("%s\n", buffer);
                memset(buffer, 0, sizeof(buffer));
                
                sprintf(input, "%s %s", argv[2], argv[4]);
                send(sock, input, strlen(input), 0);
                memset(input, 0, sizeof(input));

                read(sock, buffer, 1024);

                if(strcmp(buffer, "user berhasil login")){
                    login_user = 1;
                    printf("%s\n", buffer);
                } else {
                    printf("%s", buffer);
                    break;
                }
            } else {
                send(sock, "z", 1, 0);

                read(sock, buffer, 1024);
                printf("%s\n", buffer);
                
                break;     
            }
        }

        if(login_user){
            do {            
            scanf("\n");
            scanf("%[^\n]%*c", input);
            send(sock, input, strlen(input), 0);
            memset(input, 0, sizeof(input));

            memset(buffer, 0, sizeof(buffer));
            read(sock, buffer, 1024);
            printf("%s\n", buffer);
            } while(strcmp(buffer, "exit1") != 0);
        }
        memset(input, 0, sizeof(input));
        printf("\n");
    }
}
