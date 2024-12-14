#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <sys/socket.h>

#include <sys/types.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <signal.h>



#define PORT 4444

#define MAX_CLIENTS 5 // Batas jumlah klien maksimal



int clientCount = 0; // Jumlah klien yang terhubung saat ini

FILE *logFile; // File untuk mencatat log aktivitas



// Fungsi untuk menangani klien

void handleClient(int clientSocket) {

    char buffer[1024];

    int bytesReceived;



    // Logging koneksi baru

    logFile = fopen("server.log", "a");

    if (logFile) {

        fprintf(logFile, "Client connected. Socket: %d\n", clientSocket);

        fclose(logFile);

    }



    // Proses autentikasi klien

    send(clientSocket, "Enter password: ", strlen("Enter password: "), 0);

    bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    buffer[bytesReceived] = '\0';



    if (strcmp(buffer, "mypassword") != 0) {

        send(clientSocket, "Access Denied\n", strlen("Access Denied\n"), 0);

        close(clientSocket);

        logFile = fopen("server.log", "a");

        if (logFile) {

            fprintf(logFile, "Client failed authentication. Socket: %d\n", clientSocket);

            fclose(logFile);

        }

        clientCount--;

        exit(0);

    }



    send(clientSocket, "Authentication Successful\n", strlen("Authentication Successful\n"), 0);



    while (1) {

        // Menerima data dari klien

        bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0) {

            perror("Error in receiving data");

            break;

        }

        buffer[bytesReceived] = '\0'; // Pastikan string null-terminated



        // Jika klien mengirim ":exit", tutup koneksi

        if (strcmp(buffer, ":exit") == 0) {

            printf("Client disconnected.\n");

            break;

        }



        // Logging pesan klien

        logFile = fopen("server.log", "a");

        if (logFile) {

            fprintf(logFile, "Message from client (Socket %d): %s\n", clientSocket, buffer);

            fclose(logFile);

        }



        // Cetak pesan klien dan kirim kembali (echo)

        printf("Client: %s\n", buffer);

        send(clientSocket, buffer, strlen(buffer), 0);

        bzero(buffer, sizeof(buffer));

    }



    close(clientSocket);

    clientCount--;

    exit(0);

}



// Fungsi utama

int main() {

    int sockfd, ret;

    struct sockaddr_in serverAddr, newAddr;

    socklen_t addr_size;

    pid_t childpid;



    // Menangani proses anak yang telah selesai untuk mencegah zombie process

    signal(SIGCHLD, SIG_IGN);



    // Membuat socket

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {

        perror("Error in socket creation");

        exit(1);

    }

    printf("[+]Server socket created.\n");



    // Mengatur alamat server

    memset(&serverAddr, '\0', sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;

    serverAddr.sin_port = htons(PORT);

    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");



    // Binding

    ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    if (ret < 0) {

        perror("Error in binding");

        exit(1);

    }

    printf("[+]Bind to port %d\n", PORT);



    // Listening

    if (listen(sockfd, 10) == 0) {

        printf("[+]Listening...\n");

    } else {

        perror("Error in listening");

        exit(1);

    }



    // Menerima koneksi klien

    while (1) {

        if (clientCount >= MAX_CLIENTS) {

            printf("Server full. Rejecting new connections.\n");

            sleep(1); // Menunggu sebelum menerima koneksi baru

            continue;

        }



        addr_size = sizeof(newAddr);

        int newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);

        if (newSocket < 0) {

            perror("Error in accepting connection");

            continue;

        }



        clientCount++;

        printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));



        // Membuat proses anak untuk menangani klien

        if ((childpid = fork()) == 0) {

            close(sockfd);

            handleClient(newSocket);

        }

        close(newSocket);

    }



    close(sockfd);

    return 0;

}
