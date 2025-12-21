//this program displays some words on the screen as forward
//but just for saying hi

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include <unistd.h>



void clr_screen(void)
{
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

int main()
{
    char name[30];
    char message[200];
    char line[200];
    FILE *fp;
    fp = fopen("whatsapp2.txt", "r+");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    printf("Username: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;
    printf("message > ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = 0;
    printf("\nMessage!! ...\n");
    


    if (fp != NULL)
    {
        while (fgets(line, sizeof(line), fp))
        {
            printf("%s", line);
        }
        printf("\n");

        fclose(fp);
    }
    else 
    {
        printf("No data found on the file\n");
    }

    printf("English version of the boxes is:\n\n");
    fp = fopen("whatsapp.txt", "r+");
    if (fp == NULL)
    {
        printf("Error opening the file!!\n");
        return 1;
    }
    
    if(fp != NULL)
    {
        while(fgets(line, sizeof(line), fp))
        {
            printf("%s", line);
        }
        printf("\n");
        fclose(fp);

    }
    else
    {
        printf("No data found on the file!!\n");
    }
    usleep(10000000); // 10 seconds (10,000,000 microseconds)

    clr_screen();


    printf("now the voice:..enjoy!!\n");


    return 0;
}