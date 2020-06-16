/* 2-a.c
 * parent process'in "a", 1.child process'in "B" ve 2.child process'in "c"
 * yazdırdığı bir programda program çıktısı aBcaBcaBcaBc...
 * olacak şekilde çıktı veren program*/
#include <stdio.h> //perror
#include <stdlib.h> //exit fonksiyonu için gerekli kütüphane
#include <unistd.h> //fork() için gerekli kütüphane
#include <sys/wait.h>  //wait fonksiyonu için gerekli kütüphane


int main() 
{
    pid_t  pidFirstChild, pidSecondChild;  //1. ve 2.Child oluşup oluşmadığı değerini tutan değişken 

    for(int i = 0; i < 5; i++){
        pidFirstChild = fork();
        if(pidFirstChild > 0){  //Parent ise 
            pidSecondChild = fork();
            if(pidSecondChild > 0){ //parent ise "a" yazdır
                printf("a");
                fflush(stdout);
            }else if(pidSecondChild == 0){ //2. Child ise c yazdır
                printf("c");
                fflush(stdout);
                exit(0);
            }else
                perror("Fork Error!");  //Fork Oluşamadı Hatası
        }else if(pidFirstChild == 0){ //1.Child ise "B" yazdır
            printf("B");
            fflush(stdout);
            exit(0);
        }else
            perror("Fork Error!");  //Fork Oluşamadı Hatası
        sleep(1);
    }
    return 0;
}
