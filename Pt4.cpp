#pragma comment (lib,"Ws2_32.lib")

#include <stdio.h> 
#include <WinSock.h> 
#include <Windows.h>

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

void main(){
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
            switch(LT){
               case 
}
         }

         switch(LT){
         case 0x0800:
            fprintf_s(out, "Type: IPv4\n");

            fprintf_s(out, "Source IP: ");
            PrintIP(out, curByte + 26); // 14 байт заголовка + смещение IP-адреса отправителя в IPv4-пакете(12 байт) = 26 байт
            fprintf_s(out, "\n");

            fprintf_s(out, "Destination IP: ");
            PrintIP(out, curByte + 30); // IP-адрес назначения следует за адресом отправителя в IPv4-пакете (IP-адреса занимают 4 байта)
            fprintf_s(out, "\n");

            LT = ntohs(*(USHORT *)(curByte + 16)) + 14; // полный размер пакета указывается во 2 и 3-м байтах IPv4-пакета(в кадре 16 и 17-е байты),
            // поэтому сначала считывается двухбайтовое число по адресу currbyte + 16, а затем прибавляется длина заголовка
            fprintf_s(out, "Size: %d\n", LT);

            curByte += LT;
            IPv4count++;
            break;

         case 0x0806:
            fprintf_s(out, "Frame type: ARP\n");

            curByte += 28 + 14; // длина стандартного ARP-пакета 28 байт(является фиксированной) + 14 байт заголовка
            ARPcount++;
            break;

         default:
            if(LT > 0x05DC){
               fprintf_s(out, "Frame Ethernet DIX (Ethernet II)\n");
               DIXcount++;
            } else{
               USHORT frameType = ntohs(*(USHORT *)(curByte + 14));
               switch(frameType){
               case 0xFFFF:
                  fprintf_s(out, "Frame Raw 802.3 (Novell 802.3)\n");
                  RAWcount++;
                  break;

               case 0xAAAA:
                  fprintf_s(out, "Frame Ethernet SNAP\n");
                  SNAPcount++;
                  break;

               default:
                  fprintf_s(out, "Frame 802.3/LLC (Frame 802.3/802.2 or Novell 802.2)\n");
                  LLCcount++;
                  break;
               }
            }
            curByte += LT + 14; // длина данных + 14 байт заголовка
            break;
         }
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
