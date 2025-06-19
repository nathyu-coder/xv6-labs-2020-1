#include "kernel/types.h"
#include "user/user.h"

int
main()
{
    int p_p2c[2]; //使用p来记录读写文件描述符 子进程读 父进程写
    int p_c2p[2]; //使用p来记录读写文件描述符 父进程读 子进程写
    pipe(p_p2c);  //创建管道
    pipe(p_c2p);

    char buf = 'a';

    int status = 0;

    int pid = fork();
    if(pid < 0) { //创建子进程失败
        fprintf(3, "fail to creat fork\n");
        close(p_p2c[0]); //销毁管道
        close(p_p2c[1]);
        close(p_c2p[0]);
        close(p_c2p[1]);

        exit(1);

    }
    if(pid == 0){
        close(p_p2c[1]); //在子进程中讲父进程的管道都复制了，包括写和读，子进程中不需要使用管道的管道那端就关闭
        close(p_c2p[0]);

        if(read(p_p2c[0], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "child fail to read from parent\n");
            status = 1;
        } else {

            fprintf(1, "%d: received ping\n", getpid());
            
        }
        if (write(p_c2p[1], &buf, sizeof(char)) != sizeof(char))
        {
            fprintf(2, "child fail to write\n");
            status = 1;
        }
        
        close(p_p2c[0]);
        close(p_c2p[1]);
        exit(status);
        
    } else {
        close(p_p2c[0]); //在子进程中讲父进程的管道都复制了，包括写和读，父进程中不需要使用管道的管道那端就关闭
        close(p_c2p[1]);
        if (write(p_p2c[1], &buf, sizeof(char)) != sizeof(char)) {

            fprintf(2, "parent fail to write\n");
            status = 1;
        }
        
        if (read(p_c2p[0], &buf, sizeof(char)) != sizeof(char)){
            fprintf(2, "parent fail to read from child\n");
            status = 1;
        } else {
            fprintf(1, "%d: received pong\n", getpid());
        }
        close(p_p2c[1]);
        close(p_c2p[0]);
        exit(status);
        
    }
    
}