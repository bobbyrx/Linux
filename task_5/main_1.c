#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
        if(argc!=3)
        {
                errx(1,"Invalid number of arguments: %s <fileName1> <fileName2>", argv[0]);
        }
        const char * file1=argv[1];
        const char * file2=argv[2];
        if(strcmp(file1,file2)==0)
        {
                printf("Cannot copy a file with the same name in the same directoty\n");
                exit(0);
        }
        struct stat st;
        if(stat(file1, & st)<0)
        {
                err(2,"Cannot stat %s",file1);
        }
        int fd_file1=open(file1, O_RDONLY);
        if(fd_file1<0)
        {
                err(3,"Cannot open %s",file1);
        }
        off_t stat_size_file1=st.st_size;
        char c[stat_size_file1];
        ssize_t read_file1=read(fd_file1,&c,sizeof(c));
        if(read_file1<0)
        {
                int olderrno=errno;
                close(fd_file1);
                errno=olderrno;
                err(4,"Cannot read %s", file1);
        }
        if(read_file1!=stat_size_file1)
        {
                int olderrno=errno;
                close(fd_file1);
                errno=olderrno;
                errx(5,"Error while reading %s", file1);
        }
        close(fd_file1);
        int fd_file2=open(file2, O_CREAT | O_TRUNC | O_WRONLY , S_IRUSR | S_IWUSR);
        if(fd_file2<0)
        {
                err(6,"Cannot open %s",file2);
        }
        ssize_t write_file2=write(fd_file2,&c,sizeof(c));
        if(write_file2<0)
        {
                int olderrno=errno;
                close(fd_file2);
                errno=olderrno;
                err(7,"Cannot write in %s",file2);
        }
        if(write_file2!=stat_size_file1)
        {
                int olderrno=errno;
                close(fd_file2);
                errno=olderrno;
                errx(8,"Error while writting in %s",file2);
        }
        close(fd_file2);
        exit(0);
}


