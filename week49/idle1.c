#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int main()
{
    char name[50];
    char message[200];
    int mood;
    char date[100];
    char line[256];

    FILE *fp;
    fp = fopen("wondering.txt", "a+");
    if(fp == NULL)
    {
        printf("Error! creating the file");
        return 1;
    }


    printf("Enter your name/best name known: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")]= 0;

    printf("whats the date today!?: ");
    fgets(date, sizeof(date), stdin);
    date[strcspn(date, "\n")] = 0;

    printf("seems you have some to tell yoursself!!");
    printf("message > ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = 0;

    printf("In a scale of 10:\nRate today's mood: ");
    scanf("%d", &mood);
    getchar();

    fprintf(fp, "DATE: %s\nMESSAGE: %s\nTODAY'S MOOD: %d\n", date, message, mood);

    if(fp != NULL)
    {
        while(fgets(line, sizeof(line), fp))
        {
            printf("%s", line);
            printf("\n");
        }
    }


    fclose(fp);



    return 0;
}