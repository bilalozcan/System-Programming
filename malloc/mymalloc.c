/* Bilal Özcan
 * 18120205035
 * mymalloc.c
 * malloc fonksiyonunun yazılması*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"

int roundingnumber(size_t size){
    size_t newsize;
    if(size%16 != 0){
        int temp = size / 16;
        newsize = (temp + 1) * 16;
        return newsize;
    }else{
        return size;
    }
}
Block *firstFitAlg(Block *block, int size){
    while(block->next != NULL && block->info.size < size)
        block = ((Block *)block)->next;
    return block;
}
Block *bestFitAlg(Block *block, int size){
    Block *temp = block;
    int tempsize = temp->info.size;
    while(block->next != NULL){
        block = ((Block *)block)->next;
        if(tempsize >= block->info.size && block->info.size >= size){
            temp = block;
            tempsize = block->info.size;
        }
    }
    if(tempsize >= size)
        return temp;
    else
        return NULL;
}
Block *worstFitAlg(Block *block, int size){
    Block *temp = block;
    int tempsize = 0;
    while(block != NULL){
        if(tempsize <= block->info.size && block->info.size >= size){
            temp = block;
            tempsize = block->info.size;
        }
        block = block->next; 
    }
    if(tempsize >= size)
        return temp;
    else
        return NULL;
}
Block *split(Block *b, size_t size){
    int oldsize = b->info.size;
    b->info.size = size;
    b->bTag = (void*)b + (sizeof(Block)/2) + b->info.size;
    b->bTag->size = size;
    Block *q = (void*)b + sizeof(Block) + b->info.size;
    q->info.size = oldsize - size - sizeof(Block);
    q->bTag = (void*)q+q->info.size+sizeof(Block)/2;
    q->bTag->size =  q->info.size;
    q->next = b->next;
    q->prev = b->prev;
    return q;
}
void *mymalloc(size_t size){
    static int firstcall = 1;
    size_t newsize = roundingnumber(size);
    if(firstcall){
        heap_start = sbrk(sizeof(Block)+1024);
        heap_start->info.size = 1024; //metadata size iliklendirimesi
        heap_start->info.isfree = 1; //metadata isfree iliklendirimesi
        heap_start->next = NULL; 
        heap_start->bTag = (void*)heap_start + sizeof(Block)/2 + heap_start->info.size; //bTag in point edilmesi
        heap_start->bTag->size = 1024; //bTag size iliklendirimesi
        heap_start->bTag->isfree = 1; //bTag isfree iliklendirimesi
        heap_start->prev = NULL;
        end = (void*)heap_start + sizeof(Block) + heap_start->info.size; // Heap'in sonunun pointer ile tutulması
        free_list = heap_start;
        firstcall = 0;
    }
    Block *p = free_list;
    Block *b;
    if(p == NULL){ // free_list boş ise yeterli alan yoktur
        fprintf(stderr,"no available spcae");
        return NULL;
    }else {
        //Stratejinin tespit edilmesi
        if(strategy == 0)
            p = bestFitAlg(p, newsize);
        else if(strategy == 1)
            p = worstFitAlg(p, newsize);
        else if(strategy == 2)
            p = firstFitAlg(p, newsize);
        //Eğer free_liste sonuna kadar bakılıp uygun block bulunamadıysa yeterli alan yoktur
        if(p == NULL){
            fprintf(stderr,"no available spcae");
            return NULL;
        }
        //Eğer free_liste sonuna kadar bakılıp uygun block bulunamadıysa yeterli alan yoktur
        if((p->next == NULL && p->info.size < newsize)){
            fprintf(stderr,"no available spcae");
            return NULL;
        }else if(newsize == p->info.size){ //Bulunan block bizden istenen size boyutunda ise
            p->info.isfree = 0;            //free listen çıkarılması
            p->bTag->isfree = 0;
            deleteBlock(p);
        }else if(newsize < p->info.size){ // Bulunan block bizden istenenden fazla ise split edilip
            b = split(p, newsize);        // blockun free_listde güncellenmesi
            p->info.isfree = 0; 
            p->bTag->isfree = 0;
            b->info.isfree = 1;
            b->bTag->isfree = 1;
            if(p->prev == NULL && p->next == NULL){
                free_list = b;
                b->next = NULL;
                b->prev = NULL;
            }else if(p->prev == NULL && p->next != NULL){
                free_list = b;
                b->next = p->next;
                b->prev = NULL;
            }else if(p->prev != NULL && p->next == NULL){
                b->prev = p->prev;
                b->next = NULL;
                ((Block*)p->prev)->next = b;
            }else{
                b->prev = p->prev;
                b->next = p->next;
                ((Block*)p->prev)->next = b;
                ((Block*)p->next)->prev = b;
            }
        }
    }
    return (void*)p + sizeof(Block)/2;
}
//Alınan bir alanın free edilmesi
void *myfree(void *p){
    Block *b = (Block*)(p-sizeof(Block)/2);
    b->info.isfree = 1;
    b->bTag->isfree = 1;
    Block *next = NULL;
    Block *prev = NULL;
    if(((p + b->info.size +sizeof(Block)/2) < end)){ //Sonrasında block var mı kontrol etme

        next = p + b->info.size + sizeof(Block)/2;
        if(next->info.isfree){                              //Sonrasında block var ise boş mu kontrol etme
            b->info.size += next->info.size + sizeof(Block); //Eğer boş ise coalescing işlemi yapma
            next->bTag->size = b->info.size;
            b->bTag = next->bTag;
        }

    }
    if(((void *)b - sizeof(Block)/2) > heap_start){ //Öncesinde block var mı kontrol etme v
        Info *prevBtag = (void *)b - sizeof(Block)/2;  
        prev = (void*)b - sizeof(Block) - prevBtag->size;;
        if(prevBtag->isfree){ //Öncesinde block var ise boş mu kontrol etme
            prev->info.size += b->info.size + sizeof(Block); //Eğer boş ise coalescing işlemi yapma
            b->bTag->size = prev->info.size;
            prev->bTag = b->bTag;
            b = prev;
        }
    }
    //Heap üzerindeki işlemlerden sonra free_list üzerinde free edilen blocku ekleme işlemleri

    
    if(prev != NULL && next != NULL){ //Eğer free edilen blockun nexti ve previ var ise 
        if(prev->info.isfree && next->info.isfree){ // Hem next hem previ boş ise nexti free_list den silip
            deleteBlock(next);                      // previni güncelleme (BÖylece 3 blocku da tutabilecek)
        }else if (!prev->info.isfree && next->info.isfree){
            deleteBlock(next);
            addBlock(b);
        }else if(prev->info.isfree && !next->info.isfree){
            prev->bTag->isfree = prev->info.size;
        }
        else{
            addBlock(b);
        }   
    }else if(prev == NULL && next != NULL){ //Eğer free edilen blockun sadece nexti var ise
        if(next->info.isfree){              
            deleteBlock(next);
            addBlock(b);
        }else if(!next->info.isfree){
            addBlock(b);
        }
    }else if(prev != NULL && next ==NULL){ // Eğer free edilen blockun sadece previ var ise
        if(prev->info.isfree){
        }else if(!prev->info.isfree){
            addBlock(b);
        }
    }else{ //EĞer free edilen blockun nexti ve previ yok ise 
        addBlock(b);
    }
    return (void*)b;
}
//Free_list den block silme
void deleteBlock(Block *block){
     if(block->prev == NULL && block->next == NULL){
        free_list = NULL;
    }else if(block->prev == NULL && block->next != NULL){
        free_list = block->next;
        ((Block*)block->next)->prev = NULL;
    }else if(block->prev != NULL && block->next == NULL){
        ((Block*)block->prev)->next = NULL;
    }else{
        ((Block*)block->prev)->next = block->next;
        ((Block*)block->next)->prev = block->prev;
    }
}
//free_liste_block ekleme
void addBlock(Block *block){
    Block *tempBlock = free_list;
    if(tempBlock == NULL){
        free_list = block;
        block->next = NULL;
        block->prev = NULL;
    }else{
        while(1){
            if(tempBlock->next != NULL)
                tempBlock = tempBlock->next;
            else 
                break;
        }
        tempBlock->next = block;
        block->next = NULL;
        block->prev = tempBlock;
    }
}//Heap print etme
void printHeap(){
    Block *Heap = heap_start;   
    int i = 1;
    while((void*)Heap + Heap->info.size + sizeof(Block) <= end){
        printf("BLOCK %d:\n", i++);
        printf("\tmetadata: %d\n", Heap);
        printf("\t\tsize: %d\n",Heap->info.size);
        printf("\t\tisfree: %d\n\n",Heap->info.isfree);
        printf("\tbtag: %d\n", Heap->bTag);
        printf("\t\tsize: %d\n",Heap->bTag->size);
        printf("\t\tisfree: %d\n",Heap->bTag->isfree);
        printf("\tprev: %d\n", Heap->prev);
        printf("\tnext: %d\n", Heap->next);

        Heap = (void*)Heap + Heap->info.size + sizeof(Block);
    }
}
//Free_List print etme
void printFreeList(){
    Block *freeList = free_list;
    int i = 1;
    while(freeList != NULL){
        printf("FREE BLOCK %d:\n", i++);
        printf("\tmetadata: %d\n", (void*)freeList + sizeof(Block)/2);
        printf("\t\tsize: %d\n",freeList->info.size);
        printf("\t\tisfree: %d\n\n",freeList->info.isfree);
        printf("\tbtag: %d\n", freeList->bTag);
        printf("\t\tsize: %d\n",freeList->bTag->size);
        printf("\t\tisfree: %d\n",freeList->bTag->isfree);
        printf("\tprev: %d\n", freeList->prev);
        printf("\tnext: %d\n", freeList->next);

        freeList = freeList->next;
    }
}
int  test(int fitStrategy){
    int * ptr = mymalloc(16); 
    int * ptr2 = mymalloc(48); //
    int * ptr3 = mymalloc(32); //
    int * ptr4 = mymalloc(64); //
    int * ptr5 = mymalloc(64); //
    int * ptr6 = mymalloc(160);
    int * ptr7 = mymalloc(16); //
    int * ptr8 = mymalloc(100);
    printFreeList();
    myfree(ptr2); //48
    printFreeList();
    myfree(ptr4); // 64
    printFreeList();
    myfree(ptr7); //16
    printFreeList();
    int * ptr9 = mymalloc(18);
    printFreeList();
    int * ptr10 = mymalloc(30);
    printFreeList();
    myfree(ptr3); //
    printFreeList();
    myfree(ptr5); //
    printFreeList();
    //Kalan blockların da free edilmesi
    myfree(ptr);
    printFreeList();
    myfree(ptr6);
    printFreeList();
    myfree(ptr8);
    printFreeList();
    myfree(ptr9);
    printFreeList();
    myfree(ptr10);
    printFreeList();
}

int main(){
    //test(bestFit);
    test(firstFit);
    //test(worstFit);
}