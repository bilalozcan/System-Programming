/* Bilal Özcan
 * 18120205035
 * 1-b.c
 * parent process'in "a", child process'in "B" yazdırdığı bir programda 
 * program çıktısı BBBBBBBBBBBBBBBBBBBBBBaaaaaaaaaaaaaaaaaaaaaaaaaa olacak
 * şekilde çıktı veren program*/

#include <sys/types.h>  //pid_t type'ı için gerekli kütüphane
#include <unistd.h> //fork() için gerekli kütüphane
#include <stdio.h>  //perror
#include <sys/wait.h>  //wait fonksiyonu için gerekli kütüphane

int  main(){
    pid_t  pidFirstChild;  //Child oluşup oluşmadığı değerini tutan değişken 
    for(int i = 0; i < 6; i++){
        pidFirstChild = fork();
    }
    if (pidFirstChild == 0){ //Child ise "B" yazdır
        printf("B");
        fflush(stdout);
    }else if (pidFirstChild > 0){  //Parent ise "a" yazdır
        sleep(1);
        printf("a");
        fflush(stdout);
    }else
        perror("Fork Error!");  //Fork Oluşamadı Hatası
}