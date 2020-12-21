#include <Project.h>
#include <stdio.h>

int main (int argc,char **argv){
    if(argc!=3) {
        printf("Unknown error!");
        return 1;
    }
    char *fileInfoRead = NULL;

    FileRead(argv[1],&fileInfoRead);

    int Freq[256];
    char b[256];
    int setSize = Frequence_Count(fileInfoRead,Freq,b);
    HTNodep T=buildTree(setSize*2,Freq,b);
    FILE *fp=NULL;
    fp=fopen("C:/Users/H/Documents/GitHub/HuffmanTree/bin/zip.ezip","w+");
    fputs("Huffmantree_Compress_file\n",fp);
    printStruct(T,fp);
    //code
    fputs("EOF",fp);
    fclose(fp);
}
