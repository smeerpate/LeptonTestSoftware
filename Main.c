#include "stdio.h"
#include "SPI.h"
#include "Lepton_I2C.h"

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)
#define FPS 27

#define WIDTH 80
#define HEIGHT 60

uint8_t result[PACKET_SIZE*PACKETS_PER_FRAME];
uint16_t *frameBuffer;
uint8_t mbIntervalCnt = 0;

// open the spi port
void main()
{
    printf("[INFO] Opening SPI port...\n");
    SpiOpenPort(0);

    while(1)
    {
        // read temperatures from FPA via SPI
        int resets = 0;
        for(int j=0;j<PACKETS_PER_FRAME;j++) {
            //if it's a drop packet, reset j to 0, set to -1 so he'll be at 0 again loop
            read(spi_cs_fd, result+sizeof(uint8_t)*PACKET_SIZE*j, sizeof(uint8_t)*PACKET_SIZE);
            int packetNumber = result[j*PACKET_SIZE+1];
            if(packetNumber != j) {
                j = -1;
                resets += 1;
                usleep(1000);
                //Note: we've selected 750 resets as an arbitrary limit, since there should never be 750 "null" packets between two valid transmissions at the current poll rate
                //By polling faster, developers may easily exceed this count, and the down period between frames may then be flagged as a loss of sync
                if(resets == 750) {
                    SpiClosePort(0);
                    usleep(750000);
                    SpiOpenPort(0);
                }
            }
        }
        if(resets >= 30) {
            printf("[INFO] Got more than 30 resets, done reading.\n");
        }
        
        // Parse the received data
        frameBuffer = (uint16_t *)result;
        int row, column;
        uint16_t value;
        uint16_t minValue = 65535;
        uint16_t maxValue = 0;

        uint32_t totalCounts = 0;
        
        for(int i=0;i<FRAME_SIZE_UINT16;i++) 
        {
            //skip the first 2 uint16_t's of every packet, they're 4 header bytes
            if(i % PACKET_SIZE_UINT16 < 2)
            {
                continue;
            }
            //flip the MSB and LSB at the last second
            int temp = result[i*2];
            result[i*2] = result[i*2+1];
            result[i*2+1] = temp;
            
            value = frameBuffer[i];
            totalCounts += value;
            if(value > maxValue)
            {
                maxValue = value;
            }
            if(value < minValue)
            {
                minValue = value;
            }
            column = i % PACKET_SIZE_UINT16 - 2;
            row = i / PACKET_SIZE_UINT16 ;
        }
        
        float radValue = 0;
        radValue = frameBuffer[((HEIGHT/2) * PACKET_SIZE_UINT16)+ PACKET_SIZE_UINT16/2 - 1];
        
        //Radiometry values are the temperature in Kelvin * 100.
        float tempK = radValue/100;
        float tempC = tempK - 273.15;
        
        printf("[INFO] #Temperature = %.2f C\n", tempC);
        usleep(5000000);
        if (mbIntervalCnt>=6)
        {
            mbIntervalCnt = 0;
            lepton_perform_ffc();
        }
        else
        {
            mbIntervalCnt += 1;
        }
    }
    SpiClosePort(0);
}