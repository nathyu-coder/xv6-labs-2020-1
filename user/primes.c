#include "kernel/types.h"
#include "user/user.h"

#define COUNT 35

void primes(int p_read) {

    int p[2];

    int first;//第一个数字
    int num; //用来储存后面的数字

    if (read(p_read, &first, sizeof(int)) == 0){ //退出递归
        close(p_read);
        exit(0);
    }
    fprintf(1, "prime %d\n", first);//输出第一个

    if (pipe(p) < 0) {
        fprintf(2, "pipe error\n");
        close(p_read);
        exit(1);
    }

    
    int pid = fork(); //创建子进程

    if(pid < 0) { //创建子进程失败
        fprintf(3, "fail to creat fork\n");
        close(p_read);
        close(p[0]);
        close(p[1]);
        exit(1);
    }

    if(pid == 0) { // 进入子线程
        close(p_read);
        close(p[1]);
        primes(p[0]); //传递读端口

    } else { //父进程

        close(p[0]); //关闭读管道

        while ((read(p_read, &num, sizeof(int))) != 0) //没读完
        {
            if (num % first != 0) { //除不尽
                if((write(p[1], &num, sizeof(int))) < 0) { //写失败
                    fprintf(3, "fail to write\n");
                    close(p_read);
                    close(p[1]);
                    exit(1);
                }
            }
        }
        close(p_read);
        close(p[1]);
        wait(0);
        exit(0);
    }



}


int
main()
{
    //发送数据
    int num[COUNT];
    for(int i = 0; i < COUNT; i++) {
        num[i] = i + 2;
    }


    int p[2]; //创建管道
    pipe(p);

    int pid = fork();
    if (pid < 0) {
        fprintf(3, "fail to creat fork\n");
        close(p[0]);
        close(p[1]);
        exit(1);
    }

    if(pid == 0) { //进入子线程
        close(p[1]);
        primes(p[0]);
        close(p[0]);  // 关闭读端
        exit(0);
        
    } else { //父线程发送数据
        close(p[0]);

        for (int i = 0; i < COUNT; i++) {
            if ((write(p[1], num + i, sizeof(int))) < 0) {
                fprintf(2, "parent fail to write\n");
                exit(1);
            }
        }
        
        close(p[1]); //传输完就关闭
        wait(0);
        exit(0);
    }

    
}