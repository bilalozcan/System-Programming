/* Bilal Özcan
 * 18120205035
 * Threadler kullanılarak argüman ile verilen kapı kpadar zombi üreten doorMan vardır
 * Bir tane de slayer vardır. Doormanlar %50 ihtimal ile 2ms de bir zombie üretir
 * Slayer ise 2ms de bir zombie öldürür. Slayer 100 zombie öldürürse kazanır
 * odada 100 veya daha fazla zombie varsa oyun kaybedilir.
 */

/* Eğer Main'in içini döngüye alıp 100 kere aynı doorman sayısı ile çalıştırırsak:
 * Farklı command line argümanları ile çalıştırınca optimum doorman sayısı 3 olarak geliyor.
 * 4 doorman yaptığımda ~%70 ihtimal ile 100 zombie öldürmeyi başarıp kazanıyor.
 * 5 doorman yaptığımda ~%15 ihtimal ile 100 zombie öldürmeyi başarıp kazanıyor.
 * Optimum doorman sayısı 3 
 */
/* Eğer programı döngüye almadan aynı argüman sayısı ile birden fazla çalıştırırsak:
 * Farklı command line argümanları ile çalıştırınca optimum doorman sayısı 4 olarak geliyor
 * 5 doorman yaptığımda ~%80 ihtimal ile 100 zombie öldürmeyi başarıp kazanıyor
 * 6 doorman yaptığımda ~%15 ihtimal ile 100 zombie öldürmeyi başarıp kazanıyor 
 * Optimum doorman sayısı 4
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* number of zombies in the game: 
you should synchronize threads editing this variable*/
int zombieCounter = 0;

/* number of the killed zombies */
int zombieKilledCounter = 0;

/* Thread Mutex Variable*/
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

/* Returns the number of zombies killed.*/
int getKilledCount(){
    return zombieKilledCounter;
}
/* Returns the number of zombies in the room.*/
int getInTheRoomCount(){
    return zombieCounter;
}
/* Keeps track of number of zombies entered.*/
void zombieEntered(){
    zombieCounter++;
    printf("ZOMBIE ENTERED! \n\tZombie : %d\n\tZombie Killed : %d\n", getInTheRoomCount(), getKilledCount());

}
/* Keeps track of number of zombies killed.*/
void zombieKilled(){
    zombieKilledCounter++;
    zombieCounter--;
    printf("ZOMBIE KILLED! \n\tZombie : %d\n\tZombie Killed : %d\n", getInTheRoomCount(), getKilledCount());
}
/* Returns true if number of zombies in the room are 
greater than or equal to 100.*/
int tooManyZombiesInTheRoom(){
    if(getInTheRoomCount() >= 100) return 1;
    return 0;
}

/*Returns true if more than 100 zombies have been killed.*/
int killed100Zombies(){
    if(getKilledCount() >= 100) return 1;
    return 0;
}

/* Returns true if there is at least one zombies in the room.*/
int zombiesExist(){
    if(getInTheRoomCount() > 0)
        return 1;
    else 
        return 0;
}

/* doorman thread
 * Each DoorMan thread lets in a zombie with a 50% chance every 2ms, keeping track of the number of zombies admitted.
 * The DoorMan thread terminates if there are too many zombies (more than 100 zombies) in the room at any time or
 * if the Slayer has killed more than 100 zombies. 
 */
void *doorMan(void *p){
    while(!tooManyZombiesInTheRoom() && !killed100Zombies()){
        usleep(2000); // wait 2ms
        if(rand() % 2){ // %50 odds
            pthread_mutex_lock(&mtx); //start of Critical Section
            zombieEntered(); //let the zombies enter
            pthread_mutex_unlock(&mtx); //end of Critical Section
        }
    }
}

/* slayer thread
 * The Slayer kills a zombie every 2ms (if there is a zombie in the room) keeping track of the number of zombies killed.
 * The Slayer thread terminates if  there are too many zombies (more than 100 zombies) in the room at any time or
 * if he/she has killed more than 100 zombies.*/
void *slayer(void *p){
    while(!tooManyZombiesInTheRoom() && !killed100Zombies()){
        if(zombiesExist()){ // if zombie exist in the room
            usleep(2000); //wait 2ms
            pthread_mutex_lock(&mtx); //start of Critical Section
            zombieKilled(); // Kill zombies
            pthread_mutex_unlock(&mtx);  //end of Critical Section
        }
    }
}
/*simulator main thread*/
int main(int argc, char **argv){
    int n = atoi(argv[1]);
    pthread_t t_doorMan[n];
    pthread_t t_slayer;

    for(int i = 0; i < n; ++i) 
        pthread_create(&t_doorMan[i], NULL, doorMan, NULL); //create the doorMan threads

    pthread_create(&t_slayer, NULL, slayer, NULL); // create the slayer thread

    for(int i = 0; i < n; ++i)
        pthread_join(t_doorMan[i], NULL);
    pthread_join(t_slayer, NULL);

    printf("|- - - - - - - - - - - - - - - - - - - - - - - - - - -|\n");

    if(killed100Zombies()) // If 100 ZOmbies killed you win
        printf("|          You Win!  Killed zombie count: %d         |\n", getKilledCount());

    if(tooManyZombiesInTheRoom()) //If 100 zombies in the room you lose
        printf("|          You Lose!  Zombie count: %d               |\n", getInTheRoomCount());

    printf("|- - - - - - - - - - - - - - - - - - - - - - - - - - -|\n");
    zombieCounter = 0;
    zombieKilledCounter = 0;
    return 0;
}