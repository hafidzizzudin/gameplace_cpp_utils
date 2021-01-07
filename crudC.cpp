#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int id = 0;

struct Datas
{
    int id;
    char name[20];
    int age;
} data;

void create()
{
    struct Datas newdata;
    char tmp[20];
    newdata.id = ++id;
    printf("Enter a name (max. 20): ");
    scanf("%s", tmp);
    memcpy(newdata.name, tmp, strlen(tmp));

    printf("Enter an age: ");
    scanf("%s", tmp);
    newdata.age = atoi(tmp);

    printf("new data created: %d, %*.*s, %d\n", newdata.id, strlen(newdata.name), newdata.name, newdata.age);
}

void read(int id)
{
}

void update() {}

struct Maps
{
    int key;
    struct Datas value;
};

static struct Maps container[100];

int main()
{
    int address(21);
    printf("%d\n", address);
    char operation[100] = "";

    while (strcmp(operation, "exit") != 0)
    {
        printf("Masukkan operasi yang diinginkan\n");
        printf("\tCreate(c), Read(r), Update(u), Delete(d)\n");
        scanf("%s", operation);
        if (strcmp(operation, "c") == 0 || strcmp(operation, "Create") == 0)
            create();
        else if (strcmp(operation, "r") == 0 || strcmp(operation, "Read") == 0)
        {
            int id;
            scanf("Masukkan id: %d", id);
            read(id);
        }
        else if (strcmp(operation, "u") == 0 || strcmp(operation, "Update") == 0)
        {
            char data[512];
            scanf("");
        }
        else if (strcmp(operation, "d") == 0 || strcmp(operation, "Delete") == 0)
        {
            /* code */
        }
        else
        {
            printf("Invalid keyword");
        }
    }

    return 0;
}