/*
    Simple udp client
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>

#define SERVER "147.32.216.27"
#define BUFLEN 1024 //Max length of buffer
#define PORT 55888   //The port on which to send data
#define FILENAME "in.txt"


void die(char *s);
int getFileSize(FILE* fp);
 
int main(void)
{
    struct sockaddr_in si_other;
    int s;
    unsigned int slen=sizeof(si_other);
    char buf[BUFLEN];
 
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
     
    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
 
    // HERE
    // HERE

    char* filename = FILENAME;
    int filename_len = strlen(filename);
    if (sendto(s, &filename_len, sizeof(filename_len), 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
        die("Failed to send filename lenght\n");
    }

    if (sendto(s, filename, filename_len, 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
        die("Failed to send filename\n");
    }


    FILE* fp = fopen(filename, "rb");
    if(fp == NULL){
        die("Failed to open file\n");
    }

    int filesize = getFileSize(fp);
     
    //send the buf
    if (sendto(s, &filesize, sizeof(filesize) , 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
        die("Failed to send filesize");
    }

    int tracker = filesize;
    while(tracker > 0)
    {
        if(tracker > BUFLEN){
            if(fread(buf, BUFLEN, 1, fp) == 0) {
                die("error while reading file\n");
            }
            if (sendto(s, buf, BUFLEN, 0 , (struct sockaddr *) &si_other, slen)==-1)
            {
                die("Failed to send file");
            }
            tracker -= BUFLEN;
        }
        else{
            if(fread(buf, tracker, 1, fp) == 0) {
                die("error while reading end of file\n");
            }
            if (sendto(s, buf, tracker, 0, (struct sockaddr *) &si_other, slen)==-1)
            {
                die("Failed to send filesize");
            }
            tracker = 0;
        }
    }

    printf("File sent!\n");
    fclose(fp);
    close(s);
    return 0;
}

void die(char *s)
{
    perror(s);
    exit(1);
}

// return filesize of opened file fp
int getFileSize(FILE* fp)
{
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    rewind(fp);
    return size;
}