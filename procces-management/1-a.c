/* Bilal Özcan
 * 18120205035
 * 1-a.c
 * parent process'in "a", child process'in "B" yazdırdığı bir programda 
 * program çıktısı aBaBaBaBaBaBaBaB olacak şekilde çıktı veren program
 * Emirhan Yılmaz ve Mehmet Arsay arkadaşlar ile fikir alışverişinde bulundum*/

#include <sys/types.h> //pid_t type'ı için gerekli kütüphane
#include <unistd.h> //fork() için gerekli kütüphane
#include <stdio.h> //perror
#include <sys/wait.h> //wait fonksiyonu için gerekli kütüphane
#include <stdlib.h> //exit fonksiyonu için gerekli kütüphane

int  main(){

   pid_t  pidFirstChild; //Child oluşup oluşmadığı değerini tutan değişken 
   for(int i = 0; i < 8; i++){ 
      wait(NULL);
      pidFirstChild = fork();
      if (pidFirstChild == 0){ //Child ise "B" yazdır
         printf("B");
         fflush(stdout);
         exit(1);
      }else if (pidFirstChild > 0){ //Parent ise "a" yazdır
         printf("a");
         fflush(stdout);
      }else
         perror("Fork Error!");  //Fork Oluşamadı Hatası
   }
   return 0;
}