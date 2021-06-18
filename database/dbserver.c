#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <wait.h>
#include <time.h>

//mendefinisikan port server
#define PORT 8080

int n;

char username_now[1024] = "";
char password_now[1024] = "";

struct user {
    char name[50];
    char pwd[50];
    char db[50];
};

struct perm {
    char name[50];
    char db[50];
    char status[2];
};

void createLog(char* name, char *command){
    FILE *log = fopen("/home/nanda/db.log" , "a");
    time_t now = time(0);
    struct tm tstruct = *localtime(&now);
    int tahun=0,bulan=0,hari=0,jam=0,menit=0,detik=0;
    tahun = tstruct.tm_year + 1900;
    bulan = tstruct.tm_mon + 1;
    hari = tstruct.tm_mday;
    jam = tstruct.tm_hour;
    menit = tstruct.tm_min;
    detik = tstruct.tm_sec;
    fprintf(log, "%d%02d%02d %02d:%02d:%02d:%s:%s\n",tahun,bulan,hari,jam,menit,detik,name,command);
    fclose(log);
}

void createDatabase(char *database){
    char filepath[100];
    sprintf(filepath, "databases/%s", database);
    
    DIR* dir = opendir(filepath);
    if (dir) {
        closedir(dir);
    } else {
        mkdir(filepath, 0755);
    }
}

void reg(char *name, char *pwd) {
    FILE *fptr;

    //membuat file dengan nama akun.txt
    fptr = fopen("databases/akun/akun.tsv", "a");

    // isinya id sama password yg terdaftar
    fprintf(fptr, "%s\t%s\n", name, pwd);
    
    fclose(fptr);
}

int find_user1(char *name, char *pwd) {
    // printf("%s:%s %d:%d\n", name, pwd, strlen(name), strlen(pwd));
    int n = 0, mode = 0;

    struct user users[50];

    for (int i = 0; i < 50; ++i) {
        memset(users[i].name, 0, sizeof(users[i].name));
        memset(users[i].pwd, 0, sizeof(users[i].pwd));
    }

    FILE *fptr;
    //membuka dan membaca di file akun.txt
    fptr = fopen("databases/akun/akun.tsv", "r");

    fseek(fptr, 0, SEEK_END);
    long fsize = ftell(fptr);
    rewind(fptr);

    char *str = (char *)malloc(sizeof(char) * (fsize + 1));
    fread(str, 1, fsize, fptr);

    for (int i = 0; i <= fsize; ++i) {
        if (str[i] == '\t') {
            mode ^= 1;
            continue;
        }
        if (str[i] == '\n') {
            mode ^= 1;
            ++n;
            continue;
        }

        if (mode == 0) {
            sprintf(users[n].name, "%s%c", users[n].name, str[i]);
        } else {
            sprintf(users[n].pwd, "%s%c", users[n].pwd, str[i]);
        }
    }

    fclose(fptr);

    for (int i = 0; i < n; ++i) {
        // printf("%s:%s %d:%d\n", users[i].name, users[i].pwd, strlen(users[i].name), strlen(users[i].pwd));
        if (strcmp(users[i].name, name) == 0 && strcmp(users[i].pwd, pwd) == 0) {
            return 1;
        }
    }
    
    return 0;
}

int find_user(char *name) {
    // printf("%s:%s %d:%d\n", name, pwd, strlen(name), strlen(pwd));
    int n = 0, mode = 0;

    struct user users[50];

    for (int i = 0; i < 50; ++i) {
        memset(users[i].name, 0, sizeof(users[i].name));
        memset(users[i].pwd, 0, sizeof(users[i].pwd));
    }

    FILE *fptr;
    //membuka dan membaca di file akun.txt
    fptr = fopen("databases/akun/akun.tsv", "r");

    fseek(fptr, 0, SEEK_END);
    long fsize = ftell(fptr);
    rewind(fptr);

    char *str = (char *)malloc(sizeof(char) * (fsize + 1));
    fread(str, 1, fsize, fptr);

    for (int i = 0; i <= fsize; ++i) {
        if (str[i] == '\t') {
            mode ^= 1;
            continue;
        }
        if (str[i] == '\n') {
            mode ^= 1;
            ++n;
            continue;
        }

        if (mode == 0) {
            sprintf(users[n].name, "%s%c", users[n].name, str[i]);
        } else {
            sprintf(users[n].pwd, "%s%c", users[n].pwd, str[i]);
        }
    }

    fclose(fptr);

    for (int i = 0; i < n; ++i) {
        // printf("%s:%s %d:%d\n", users[i].name, users[i].pwd, strlen(users[i].name), strlen(users[i].pwd));
        if (strcmp(users[i].name, name) == 0) {
            return 1;
        }
    }
    
    return 0;
}

int find_db(char *db){
    int n = 0;
    struct user dir[50];

    for (int i = 0; i < 50; ++i) {
        memset(dir[i].db, 0, sizeof(dir[i].db));
    }

    struct dirent *de;  // Pointer for directory entry
  
    // opendir() returns a pointer of DIR type. 
    DIR *dr = opendir("databases");
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
        return 0;
    }
  
    while ((de = readdir(dr)) != NULL){
            sprintf(dir[n].db, "%s", de->d_name);
            // printf("%s\n", dir[n].db);
            n++;
    }

    for(int i = 0; i < n; i++){
        if (strcmp(dir[i].db, db) == 0) {
            return 1;
        }
        
    }
            
    closedir(dr);
}

int permission(char *name, char *database, char *status) {

    if(find_user(name)){
        if(find_db(database)){
            FILE *fptr;

            //membuat file dengan nama akun.txt
            fptr = fopen("databases/akun/permission.tsv", "a");

            // isinya id sama password yg terdaftar
            fprintf(fptr, "%s\t%s\t%s\n", name, database, status);
            
            fclose(fptr);
            return 1;
        } else{
            return 0;
        }
    } else {
        return 0;
    }

    return 0;
}

void createTable(char * database, char * table, char * col){
    char filepath[100];
    sprintf(filepath, "databases/%s/%s.tsv", database, table);

    FILE *fptr;
    fptr = fopen(filepath, "a");

    fprintf(fptr, "%s\n", col);
    fclose(fptr);

}

struct perm *parse_files() {
    n = 0;
    int mode = 0;

    struct perm *perms;
    perms = (struct perm *)malloc(sizeof(struct perm) * 50);

    for (int i = 0; i < 3; ++i) {
        //data yang akan diisikan berupa nama perm, path filenya, nama publisher dan tahun publishnya
        memset(perms[i].name, 0, sizeof(perms[i].name));
        memset(perms[i].db, 0, sizeof(perms[i].db));
        memset(perms[i].status, 0, sizeof(perms[i].status));
    }

    FILE *fptr;
    //membuka perm.tsv untuk menambahkan data di atas
    fptr = fopen("databases/akun/permission.tsv", "r");

    //untuk membaca perm  mulai dari offset yang diinginkan
    fseek(fptr, 0, SEEK_END); 
    long fsize = ftell(fptr); 
    rewind(fptr);

    char *str = (char *)malloc(sizeof(char) * (fsize + 1));
    //membaca kalimat dalam sebuah FILE yang sudah dibuka di perm.tsv
    fread(str, 1, fsize, fptr);

    for (int i = 0; i <= fsize; ++i) {
        if (str[i] == '\t') {
            mode += 1;
            continue;
        }
        if (str[i] == '\n') {
            mode = 0;
            ++n;
            continue;
        }

        if (mode == 0) {
            sprintf(perms[n].name, "%s%c", perms[n].name, str[i]);
        } else if (mode == 1) {
            sprintf(perms[n].db, "%s%c", perms[n].db, str[i]);
        } else {
            sprintf(perms[n].status, "%s%c", perms[n].status, str[i]);
        }
    }

    fclose(fptr);

    return perms;
}


void dropDatabase(char * database){
    char filepath[100];
    sprintf(filepath, "databases/%s", database);

    DIR *dir = opendir(filepath);

    if (dir)
    {
        pid_t child_id;
        child_id = fork();

        if (child_id == 0) {
            char *argv[] = {"rmdir", "-r", filepath,NULL};
            execv("/bin/rm", argv);
            
            exit(EXIT_SUCCESS);
        } else if (child_id > 0) {
            wait(NULL);
        }
    }else
    {
        printf("Database tidak ditemukan");
    }
}

void dropTable(char * database, char * tablename){   
        char filepath[100];
        char dirpath[100];
        sprintf(dirpath, "databases/%s", database);
        sprintf(filepath, "databases/%s/%s.tsv", database, tablename);

        DIR *dir = opendir(dirpath);

        if (dir) {
            pid_t child_id;
            child_id = fork();

            if (child_id == 0) {
                char *argv[] = {"rm", filepath,NULL};
                execv("/bin/rm", argv);
                
                exit(EXIT_SUCCESS);
            } else if (child_id > 0) {
                wait(NULL);
            }
        } else {
            closedir(dir);
        }
}

int main(int argc, char const *argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    char buffer[1024] = {0};
    char users[1024];
    char *login_berhasil = "user berhasil login";
    char *login_gagal = "user gagal login";
    char *sedang_login = "user sedang login";
    char *login_berhasil1 = "user root berhasil login";
    char *creatu_berhasil = "user root berhasil create user";
    char *grant_berhasil = "user root berhasil grant permission";
    char *grant_gagal = "user/database tidak ditemukan";
    char *db_berhasil = "berhasil create database";
    char *tab_berhasil = "berhasil create table";
    char *gagal = "salah input";
    char *exit1 = "exit";
    char *exit2 = "exit1";
    char *use1 = "sedang menggunakan database";
    char *nouse = "tidak bisa menggunakan database";
    char *berhasil = "berhasil";

    char msg[100];
    char dbname[40] = "0";
    char tablename[40];
    char columnname[40];
    char *token2;

    int login_user = 0;
    int login_root = 0;
    int akses = 0;

    mkdir("databases", 0755);

    while(1){
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

            memset(buffer, 0, sizeof(buffer));
            read(new_socket, buffer, 1024);
            memset(users, 0, sizeof(users));

            if (strncmp(buffer, "l", 1) == 0) {
                printf("user login\n");
                send(new_socket, sedang_login, strlen(sedang_login), 0);
                printf("%s\n", sedang_login);

                read(new_socket, users, 1024);
                printf("username password: %s\n", users);

                const char s[2] = " ";
                char *token;

                token = strtok(users, s);
                char username[20];
                sprintf(username, "%s", token);

                token = strtok(NULL, s);
                char password[20];
                sprintf(password, "%s", token);

                memset(username_now, 0, sizeof(username_now));
                memset(password_now, 0, sizeof(password_now));
            
                if(find_user1(username, password)){
                    login_user = 1;

                    sprintf(username_now, "%s", username);
                    sprintf(password_now, "%s", password);                    
                    
                } else {
                    send(new_socket, login_gagal, strlen(login_gagal), 0);
                    printf("%s\n", login_gagal);
                }
                // break;

            } else if (strncmp(buffer, "c", 1) == 0) {
                printf("user root login\n");
                send(new_socket, login_berhasil1, strlen(login_berhasil1), 0);

                login_root = 1;

            } else {
                printf("user ga ngapa2in\n");
                send(new_socket, gagal, strlen(gagal), 0);

                break;
            }

            if(login_user){
                send(new_socket, login_berhasil, strlen(login_berhasil), 0);
                printf("%s\n", login_berhasil);
                send(new_socket, "!", 1, 0);
                int exit = 0;
                while(exit != 1) {
                    memset(buffer, 0, sizeof(buffer));
                    memset(users, 0, sizeof(users));

                    read(new_socket, users, 1024);
                    printf("%s\n", users);

                    if(strcmp(users, "exit") == 0){
                        exit = 1;
                        akses = 0;
                        printf("user exit\n");
                        send(new_socket, exit1, strlen(exit1), 0);
                        break;
                    }

                    char * ext;
                    ext = strstr(users, ";");
                    int pos = ext - users;
                    sprintf(users, "%.*s", pos, users);

                    const char s[2] = " ";
                    char *token2;

                    token2 = strtok(users, s);
                    char command1[20];
                    sprintf(command1, "%s", token2);

                    token2 = strtok(NULL, s);
                    char command2[20];
                    sprintf(command2, "%s", token2);

                    char command3[40];
                    sprintf(command3, "%s %s", command1, command2);

                    if (strcmp(command1, "USE") == 0) {   
                        // char dbname[20];
                        sprintf(dbname, "%s", command2);

                        // pengecekan
                        int found = 0;
                        struct perm *perms = parse_files();
                        // printf("%d\n", n);
                        for (int i = 0; i < n; ++i)
                        {
                            if (strcmp(perms[i].name, username_now) == 0 && strcmp(perms[i].db, dbname) == 0) {
                                found = 1;
                                break;
                            }
                        }

                        if (found) {
                            akses = 1;
                            createLog(username_now, command3);
                            send(new_socket, use1, strlen(use1), 0);
                            printf("boleh pake database\n");
                        } else {
                            send(new_socket, nouse, strlen(nouse), 0);
                            printf("nggak boleh pake database\n");
                        }

                        // printf("%s\n", dbname);
                    } else if(strcmp(command3, "CREATE DATABASE") == 0) {
                        token2 = strtok(NULL, " ");
                        // char dbname[40];
                        sprintf(dbname, "%s", token2);
                        char fullCommand[200];
                        sprintf(fullCommand, "%s %s", command3, dbname);
                       
                        createDatabase(dbname);
                        permission(username_now, dbname, "1");

                        createLog(username_now, fullCommand);
                        send(new_socket, berhasil, strlen(berhasil), 0);

                    } else if(strcmp(command3, "CREATE TABLE") == 0) {   
                        if (akses > 0){
                            token2 = strtok(NULL, " ");
                            char tablename[40];
                            char col[512] = "";
                            sprintf(tablename, "%s", token2);
                            printf("%s", tablename);

                            while(token2 != NULL)
                            {
                                token2 = strtok(NULL, ",()");
                                // printf("ini dalem while: %s\n", token2);

                                if (token2 != NULL)
                                {   
                                    char columname[20];
                                    char datatype[20];
                                    
                                    sscanf(token2, "%s %s", columname, datatype);
                                    strcat(columname, "\t");
                                    strcat(col, columname);
                                }     
                            }

                            createTable(dbname, tablename, col);
                            char fullCommand[200];
                            sprintf(fullCommand, "%s %s", command3, tablename);
                            createLog(username_now, fullCommand);
                            send(new_socket, berhasil, strlen(berhasil), 0);
                        } else {   
                            sprintf(msg, "Database belum ditemukan, Mohon gunakan USE [nama_database]\n");
                            send(new_socket, msg, strlen(msg), 0);
                            continue;
                        }
                        
                    } else if(strcmp(command3, "DROP DATABASE") == 0) {
                        // printf("ini drop database\n");
                        token2 = strtok(NULL, " ");
                        sprintf(dbname, "%s", token2);
                        // printf("ini nama dbnya: %s\n", dbname);
                        dropDatabase(dbname);
                        char fullCommand[200];
                        sprintf(fullCommand, "%s %s", command3, dbname);
                        createLog(username_now, fullCommand);
                        send(new_socket, berhasil, strlen(berhasil), 0);

                    } else if(strcmp(command3, "DROP TABLE") == 0) {   
                        if (akses > 0) {
                            token2 = strtok(NULL, " ");
                            char tablename[40];
                            sprintf(tablename, "%s", token2);

                            dropTable(dbname, tablename);
                            char fullCommand[200];
                            sprintf(fullCommand, "%s %s", command3, tablename);
                            createLog(username_now, fullCommand);
                            send(new_socket, berhasil, strlen(berhasil), 0);                        
                        } else {   
                            sprintf(msg, "Database belum ditemukan, Mohon gunakan USE [nama_database]\n");
                            send(new_socket, msg, strlen(msg), 0);
                            continue;
                        }
                            
                    } else {
                        sprintf(msg, "Gagal menjalankan command\n");
                        send(new_socket, msg, strlen(msg), 0);
                        continue;
                    }
                    

                }
                memset(dbname, 0, sizeof(dbname));
                memset(tablename, 0, sizeof(tablename));
            }

            if(login_root){
                int exit = 0;
                while(exit != 1) {
                    memset(buffer, 0, sizeof(buffer));
                    memset(users, 0, sizeof(users));

                    read(new_socket, users, 1024);

                    if(strcmp(users, "exit") == 0){
                        exit = 1;
                        printf("user root exit\n");
                        send(new_socket, exit1, strlen(exit1), 0);
                        break;
                    }

                    const char s[2] = " ";
                    char *token;

                    token = strtok(users, s);
                    char command1[20];
                    sprintf(command1, "%s", token);

                    token = strtok(NULL, s);
                    char command2[20];
                    sprintf(command2, "%s", token);

                    char command3[40];
                    sprintf(command3, "%s %s", command1, command2);

                    if(strcmp(command3, "CREATE USER") == 0) {         
                        token = strtok(NULL, s);
                        char username[20];
                        sprintf(username, "%s", token);

                        token = strtok(NULL, s);
                        char iden[20];
                        sprintf(iden, "%s", token);

                        token = strtok(NULL, s);
                        char by[20];
                        sprintf(by, "%s", token);

                        token = strtok(NULL, s);
                        char password[20];
                        sprintf(password, "%s", token);

                        createDatabase("akun");
                        reg(username, password);

                        char fullCommand[200];
                        sprintf(fullCommand, "%s %s %s %s %s", command3, username, iden, by, password);
                        createLog(username_now, fullCommand);

                        send(new_socket, creatu_berhasil, strlen(creatu_berhasil), 0);
                        printf("%s\n", creatu_berhasil);

                    } else if(strcmp(command3, "CREATE DATABASE") == 0) {
                        token = strtok(NULL, s);
                        char database[20];
                        sprintf(database, "%s", token);

                        createDatabase(database);
                        char fullCommand[200];
                        sprintf(fullCommand, "%s %s", command3, database);
                        createLog(username_now, fullCommand);

                        send(new_socket, creatu_berhasil, strlen(creatu_berhasil), 0);
                        printf("%s\n", creatu_berhasil);
                        
                    } else if(strcmp(command3, "GRANT PERMISSION") == 0) {
                        token = strtok(NULL, s);
                        char database[20];
                        sprintf(database, "%s", token);

                        token = strtok(NULL, s);
                        char into[20];
                        sprintf(into, "%s", token);

                        token = strtok(NULL, s);
                        char username[20];
                        sprintf(username, "%s", token);

                        createDatabase("akun");
                        if(permission(username, database, "1")){
                            char fullCommand[200];
                            sprintf(fullCommand, "%s %s %s %s", command3, database, into, username);
                            createLog(username_now, fullCommand);
                            send(new_socket, grant_berhasil, strlen(grant_berhasil), 0);
                            printf("%s\n", grant_berhasil);
                        } else {
                            send(new_socket, grant_gagal, strlen(grant_gagal), 0);
                            printf("%s\n", grant_gagal);
                        }
                        
                    } else {
                        printf("user root salah input\n");
                        send(new_socket, gagal, strlen(gagal), 0);
                    }
                }
                
            }
            printf("\n");
        
        close(new_socket);
    }

}
