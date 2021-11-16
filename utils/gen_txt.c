#include <stdio.h>
#include <stdlib.h>
#define MAX_VAL (int)(1e6)
 
void make_txt(char *fname, int n) {
    FILE *file;
    if (!(file = fopen(fname, "w"))) {
        perror("Could not open file");
        exit(1);
    }
 
    for (int i = 0; i < n; ++i)
        fprintf(file, "%d\n", rand() % MAX_VAL + 1);
 
    fclose(file);
}
 
int main(int argc, char* argv[]) {
    srand(42);
 
    int n2, n3;
    printf("Number of Elements Required in c2.txt: ");
    scanf("%d", &n2);
 
    make_txt("c2.txt", n2);
    printf("c2.txt created!\n");
 
    printf("Number of Elements Required in c3.txt: ");
    scanf("%d", &n3);
 
    make_txt("c3.txt", n3);
    printf("c3.txt created!\n");
}
