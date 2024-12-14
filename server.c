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
    const char *authMessage = "Enter password: ";
    int sentBytes = send(clientSocket, authMessage, strlen(authMessage), 0); // Pastikan pesan dikirim
    if (sentBytes < 0) {
        perror("Error sending authentication message");
        close(clientSocket);
        return;
    }

    // Menerima password dari klien
    bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived < 0) {
        perror("Error receiving password");
        close(clientSocket);
        return;
    }
    buffer[bytesReceived] = '\0'; // Null-terminate the string

    // Debug output to see what was received
    printf("Received password: '%s' (length: %d)\n", buffer, bytesReceived);

    // Hapus karakter newline dan carriage return yang mungkin ada di akhir password
    if (bytesReceived > 0) {
        if (buffer[bytesReceived - 1] == '\n') {
            buffer[bytesReceived - 1] = '\0'; // Remove newline
            bytesReceived--; // Decrease the length
        }
        if (bytesReceived > 0 && buffer[bytesReceived - 1] == '\r') {
            buffer[bytesReceived - 1] = '\0'; // Remove carriage return
        }
    }

    // Periksa apakah password yang dimasukkan benar
    if (strcmp(buffer, "mypassword") != 0) {
        send(clientSocket, "Access Denied\n", strlen("Access Denied\n"), 0);
        close(clientSocket);
        logFile = fopen("server.log", "a");
        if (logFile) {
            fprintf(logFile, "Client failed authentication. Socket: %d\n", clientSocket);
            fclose(logFile);
        }
        return;
    }

    // Jika autentikasi berhasil
    send(clientSocket, "Authentication Successful\n", strlen("Authentication Successful\n"), 0);

    // Proses selanjutnya: menerima pesan dari klien
    while (1) {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            perror("Error receiving data");
            break;
        }
        buffer[bytesReceived] = '\0'; // Null-terminate the string

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

        // Kirim kembali pesan yang diterima (echo)
        printf("Client: %s\n", buffer);
        send(clientSocket, buffer, strlen(buffer), 0);
        bzero(buffer, sizeof(buffer));
    }

    close(clientSocket);
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
    serverAddr.sin