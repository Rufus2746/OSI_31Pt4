#pragma comment (lib,"Ws2_32.lib")

#include <stdio.h> 
#include <fstream>
#include <iostream>
#include <WinSock.h> 
#include <Windows.h>
using namespace std;

void PrintMAC(FILE *out, char *MAC){
   for(int i = 0; i < 5; i++)
      fprintf_s(out, "%02X:", (unsigned char)MAC[i]);
   fprintf_s(out, "%02X", (unsigned char)MAC[5]);
}

void PrintIP(FILE *out, char *IP){
   for(int i = 0; i < 3; i++)
      fprintf_s(out, "%d.", (unsigned char)IP[i]);
   fprintf_s(out, "%d", (unsigned char)IP[3]);
}

void prog(){
   char *buffer, fileName[256];
   int fileSize = 0, ARPcount = 0, IPv4count = 0, DIXcount = 0,
      SNAPcount = 0, RAWcount = 0, LLCcount = 0;

   printf_s("Enter the name of binary file: ");
   scanf_s("%s", &fileName, 256);

   FILE *in;
   fopen_s(&in, fileName, "rb");
   if(in == NULL)
      printf_s("No available file found\n");
   else{
      FILE *out;
      fopen_s(&out, "out.txt", "w");
      fseek(in, 0, SEEK_END);
      fileSize = ftell(in);
      fseek(in, 0, SEEK_SET);

      buffer = new char[fileSize];
      fread(buffer, fileSize, 1, in);
      fclose(in);

      char *curByte = buffer;
      int frame_number = 1;
      while(curByte < buffer + fileSize){
         fprintf_s(out, "Frame number: %d\n", frame_number);

         fprintf_s(out, "Destination MAC: ");
         PrintMAC(out, curByte);
         fprintf_s(out, "\n");

         fprintf_s(out, "Source MAC: ");
         PrintMAC(out, curByte + 6);
         fprintf_s(out, "\n");

         USHORT LT = ntohs(*(USHORT *)(curByte + 12));
         fprintf_s(out, "Length/Type field: %d\n", LT);

         if(LT>=0x05DC){
            fprintf_s(out, "Type: Ethernet II (DIX)\n");
            switch(LT){
            case 0x0800:
               fprintf_s(out, "Type: IPv4\n");
               fprintf_s(out, "IP package:\n");
               fprintf_s(out, "\tVersion: \n", LT);
               fprintf_s(out, "\t \n", LT+1);
               fprintf_s(out, "\t \n", LT+2);

               break;
            case 0x0806:
               fprintf_s(out, "Type: ARP\n");
               break;
            case 0x86DD:
               fprintf_s(out, "Type: IPv6\n");
               break;
            default:
               break;
            }
         } else{
            fprintf_s(out, "Type: IEEE\n");
         }
         curByte += LT + 14; // длина данных + 14 байт заголовка
         frame_number++;
         fprintf_s(out, "\n");
      }

      fprintf_s(out, "\nEntire frame number: %d\n", frame_number - 1);
      fprintf_s(out, "IPv4: %d\n", IPv4count);
      fprintf_s(out, "ARP: %d\n", ARPcount);
      fprintf_s(out, "DIX: %d\n", DIXcount);
      fprintf_s(out, "RAW: %d\n", RAWcount);
      fprintf_s(out, "SNAP: %d\n", SNAPcount);
      fprintf_s(out, "LLC: %d\n", LLCcount);
      fclose(out);
   }
}

void main(){
   string filePath= "ethers07.bin";
   int fileSize = 0, ARPcount = 0, IPv4count = 0, DIXcount = 0,
      SNAPcount = 0, RAWcount = 0, LLCcount = 0;
   ifstream input;
   ofstream output;
   output.open("output.txt");
   input.open(filePath);
   if(input&&output){
      input.seekg(0, ios::end);
      streamsize fileSize = input.tellg();
      input.seekg(0,ios::beg);
      char* data = new char[fileSize];
      input.read(data, fileSize);
      input.close();
      output.close();
      for(int i = 0; i < fileSize; ++i){
         printf("%02X ", static_cast<unsigned char>(data[i]));
      }
      cout << '\n';

   } else{
      cout<<"Can't open the file"<<endl;
   }
}
