// convert the contents of stdin to their ASCII values (e.g., 
// '\n' = 10) and spit out the <prog> array used in Figure 1 in
// Thompson's paper.
#include <stdio.h>
int main(void) {
    // put your code here.
    printf("char prog[] = {\n");
    int global_i = 0;
    char program[100000];
    char c; int ret;
    while (((ret = scanf("%c", &c)) == 1)) {
        program[global_i] = c;
        if (global_i % 8 != 0){
            printf(", ");
        }
        global_i++;
        printf("\t%d", c);
        if (global_i % 8 == 0){
            printf(",\n");
        }
    }

    printf("0 };\n");
    printf("%s", program);
    return 0;


    
	// return 0;
}
