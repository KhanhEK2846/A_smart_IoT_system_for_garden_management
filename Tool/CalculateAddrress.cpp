#include <iostream>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
using namespace std;

uint8_t AddH = 0;
uint8_t AddL = 0;
uint8_t Channel = 0;

int main()
{
    
    char data[17] = {0};

    do
    {
        cout<<"Input ID: ";
        cin>>data;
    } while (strlen(data) != 17);

    int tempid[6] = {0,0,0,0,0};

    sscanf(data, "%02x:%02x:%02x:%02x:%02x:%02x", &tempid[0], &tempid[1], &tempid[2], &tempid[3], &tempid[4], &tempid[5]);

    AddH = tempid[0] + tempid[1] + tempid[2];
	AddL = tempid[3] + tempid[4] + tempid[5];
	Channel = tempid[0] + tempid[1] + tempid[2] + tempid[3] + tempid[4] + tempid[5];

    while(AddH > 0xFF) AddH -= 0xFF;
    while(AddL > 0xFF) AddL -= 0xFF;
    while(Channel > 0x1F) Channel -= 0x1F;

    cout<<"AddH "<<hex<<unsigned(AddH)<<"\n";
    cout<<"AddL "<<hex<<unsigned(AddL)<<"\n";
    cout<<"Channel "<<hex<<unsigned(Channel)<<"\n";



    return 0;
}