/*
    Simple udp server
*/
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
 
#define BUFLEN 1024  //Max length of buffer
#define PORT 55888   //The port on which to listen for incoming data
#define ADDTOFILENAME "new_"
 
void die(char *s)
{
    perror(s);
    exit(1);
}

char* stradd(const char* a, const char* b);

 
int main(void)
{
    struct sockaddr_in si_me, si_other;
    unsigned int slen = sizeof(si_other);
    int s,filesize, recv_len, filename_len, fsize_tracker;
    char *filename;
    char *newfilename;
    char buf[BUFLEN];
    FILE *fp;
     
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
     
    //keep listening for data
   
    printf("Waiting for data...");
    fflush(stdout);


    //try to receive length of filename
    if ((recv_len = recvfrom(s, &filename_len, sizeof(filename_len), 0, (struct sockaddr *) &si_other, &slen)) == -1)
    {
        die("recvfrom()");
    } 
    
    //prepare filename
    filename = (char *) malloc(sizeof(char)* filename_len);    
    //try to receive filename
    if ((recv_len = recvfrom(s, filename, filename_len, 0, (struct sockaddr *) &si_other, &slen)) == -1)
    {
        die("recvfrom()");
    }

    //prepare new filename
    newfilename = stradd(filename, ADDTOFILENAME);
    //try to receive size of file
    if ((recv_len = recvfrom(s, &filesize, sizeof(filesize), 0, (struct sockaddr *) &si_other, &slen)) == -1)
    {
        die("recvfrom()");
    }
    //prepare file for saving data
    fp = fopen(newfilename, "wb");
    fsize_tracker = filesize;

     while(fsize_tracker > 0)
    {
        
        if(fsize_tracker >= BUFLEN) {
            recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen);
            if(recv_len == -1){
                die("Receiving file error");
            }    
            if(fwrite(buf, BUFLEN, 1, fp) == 0){
                die("fwrite fail");
            }
            fsize_tracker -= BUFLEN;
        } else {
            recv_len = recvfrom(s, buf, fsize_tracker, 0, (struct sockaddr *) &si_other, &slen);
            if(recv_len == -1) {
                die("Receiving file error");
                }    
            if(fwrite(buf, fsize_tracker, 1, fp) == 0){
                die("fwrite fail");
            }
            fsize_tracker = 0;
        }
    }
        
    //print details of the client/peer and the data received
    printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
    printf("Filesize: %d\n" , filesize);
    printf("Filenamelen: %d\n" , filename_len);
    printf("Filename: %s\n", filename);
    printf("Saved as: %s\n", newfilename);
    printf("Transfer completed\n");
         
    
    free(filename);
    free(newfilename);
    close(s);
    fclose(fp);
    return 0;
}


char* stradd(const char* a, const char* b){
    size_t len = strlen(a) + strlen(b);
    char *ret = (char*)malloc(len * sizeof(char) + 1);
    *ret = '\0';
    return strcat(strcat(ret, b) ,a);
}
