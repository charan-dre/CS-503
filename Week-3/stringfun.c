#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
int reverse_string(char *, int);
int word_print(char *, int);
int replace_string(char *, int, char *, char *);


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    if (!buff || !user_str || len <= 0)
    {
        return -2;
    }
    
    char *buff_ptr = buff;
    char *str_ptr = user_str;
    int count = 0;
    int last_was_space = 1;

    while (*str_ptr != '\0' && count < len)
    {
        if (*str_ptr == ' ' || *str_ptr == '\t')
        {
            if (!last_was_space)
            {
                *buff_ptr++ = ' ';
                count++;
            }
            last_was_space = 1;
        } else {
            *buff_ptr++ = *str_ptr;
            count++;
            last_was_space = 0;
        }
        str_ptr++;
    }

    if (*str_ptr != '\0')
    {
        return -1;
    }
    
    //Filling remaining buffer space with dots
    while (count < len)
    {
        *buff_ptr++ = '.';
        count++;
    }

    return count; //for now just so the code compiles. 
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i = 0; i < len; i++){
        putchar(*(buff + i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT
    if (!buff || len <= 0 || str_len <= 0)
    {
        return -2;
    }
    
    int word_count = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++)
    {
        if (*(buff + i) != ' ')
        {
            if (!in_word)
            {
                word_count++;
                in_word = 1;
            }
        } else {
            in_word = 0;
        }
    }
    
    return word_count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
int reverse_string(char *buff, int str_len){
    if (!buff || str_len <= 0)
    {
        printf("Error: Invalid input for the reverse_string.\n");
        return -2;
    }

    int start = 0;
    int end = str_len - 1;
    char temp;

    while (start < end)
    {
        temp = buff[start];
        buff[start] = buff[end];
        buff[end] = temp;

        start++;
        end--;
    }
    buff[str_len] = '\0';
    
    return 0;
}

int word_print(char *buff, int str_len){
    if (!buff || str_len <=0)
    {
        printf("Error: Invalid input for word_print.\n");
        return -2;
    }

    printf("Word Print\n");
    printf("----------\n");

    int word_start = 0;
    for (int i = 0; i < str_len; i++)
    {
        if (buff[i] == ' ' || buff[i] == '\0')
        {
            if (word_start > 0)
            {
                putchar('\n');
                word_start = 0;
            }
        } else {
            putchar(buff[i]);
            word_start++;
        }

        if (buff[i] == '\0')
        {
            break;
        }
    }

    return 0;
}

int replace_string(char *buff, int len, char *search, char *replacement)
{
    if (!buff || !search || !replacement || len <= 0)
    {
        return -2;
    }
    
    char temp[BUFFER_SZ];
    char *pos = strstr(buff, search);
    if (!pos)
    {
        return -1;
    }
    
    int prefix_len = pos - buff;
    int search_len = strlen(search);
    int replacement_len = strlen(replacement);
    int suffix_len = strlen(pos + search_len);

    if (prefix_len + replacement_len + suffix_len >= len)
    {
        printf("Error: Buffer overflow detected. Truncating output.\n");
        suffix_len = len - prefix_len - replacement_len - 1;
    }
    
    strncpy(temp, buff, prefix_len);
    temp[prefix_len] = '\0';
    strncat(temp, replacement, len - strlen(temp) - 1);
    strncat(temp, pos + search_len, suffix_len);

    int temp_len = strlen(temp);
    while (temp_len < len)
    {
        temp[temp_len++] = '.';
    }
    temp[len] = '\0';

    strncpy(buff, temp, len);

    return 0;
}



int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // This is safe because the condition (argc < 2) ensures that argv[1] exists.
    // If argv[1] does not exist, the program exits before accessing it.
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // This ensures the user has provided the required input string for processing.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char *)malloc(BUFFER_SZ);
    if (!buff)
    {
        printf("Error: Memory allocation failed.\n");
        exit(99);
    }
    
    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d\n", user_str_len);
        free(buff);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                free(buff);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            rc = reverse_string(buff, user_str_len);
            if (rc < 0)
            {
                printf("Error reversing string, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            printf("Reversed String: %s\n", buff);
            break;
        
        case 'w':
            rc = word_print(buff,user_str_len);
            if (rc < 0)
            {
                printf("Error printing words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            break;

        case 'x':
            if (argc < 5)
            {
                printf("sage for -x: %s -x \"string\" \"target\" \"replacement\"\n", argv[0]);
                free(buff);
                exit(1);
            }
            rc = replace_string(buff, BUFFER_SZ, argv[3], argv[4]);
            if (rc < 0)
            {
                if (rc == -1)
                {
                    printf("Search string not found in the buffer.\n");
                } else {
                    printf("Error replacing string, rc = %d\n", rc);
                }
                free(buff);
                exit(2);
            }
            printf("Modified String: %s\n", buff);
            break;
            

        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    //print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE
//This ensures that the function has all the information it needs to 
//safely iterate through the buffer. It avoids reliance on external 
//assumptions (like knowing BUFFER_SZ is 50).