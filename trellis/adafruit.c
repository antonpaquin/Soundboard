#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdbool.h>
#include "i2c-dev.h"
#include <unistd.h>

#define ADAFRUIT_DEVICE 0x70
#define HT16K33_BLINK_CMD       0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_CMD_BRIGHTNESS 0xE0

void setLED(int x);
void writeDisplay(int fh);
void clrLED(int x);

__u16 block[I2C_SMBUS_BLOCK_MAX];
int res, i2cbus, daddress, address, size, file;

static const uint8_t ledLUT[16] =
    { 0x3A, 0x37, 0x35, 0x34,
      0x28, 0x29, 0x23, 0x24,
      0x16, 0x1B, 0x11, 0x10,
      0x0E, 0x0D, 0x0C, 0x02 };
static const uint8_t buttonLUT[16] =
    { 0x07, 0x04, 0x02, 0x22,
      0x05, 0x06, 0x00, 0x01,
      0x03, 0x10, 0x30, 0x21,
      0x13, 0x12, 0x11, 0x31 };
uint16_t displaybuffer[8];
unsigned keys[6], lastkeys[6];



void  INThandler(int sig)
{
       // Closing file and turning off Matrix

	unsigned short int clear[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	displayImage(clear,res, daddress, file);

        printf("Closing file and turning off the LED Matrix\n");
        daddress = 0x20;
        for(daddress = 0xef; daddress >= 0xe0; daddress--) {
                res = i2c_smbus_write_byte(file, daddress);
        }

	signal(sig, SIG_IGN);

        close(file);
        exit(0);
}

int displayImage(__u16 bmp[], int res, int daddress, int file)
{
        int i;
        for(i=0; i<8; i++)
        {
             block[i] = (bmp[i]&0xfe) >>1 |
             (bmp[i]&0x01) << 7;
        }
        res = i2c_smbus_write_i2c_block_data(file, daddress, 16,
                (__u8 *)block);
        usleep(100000);
}
bool readSwitches(int fh)
{
	uint8_t i, j;

	memcpy(lastkeys,keys,sizeof(keys));
	for (i=0;i<6;i++){
		keys[i] = i2c_smbus_read_byte_data(fh,0x40);	
	}
	for (i=0; i<6; i++) {
    		if (lastkeys[i] != keys[i]) {
       			for (j=0; j<6; j++) {
       				}
      		return true;
		}
	  }
	return false;
}

bool isKeyPressed(uint8_t k){
	return (keys[buttonLUT[k] >> 4] & (1 << (buttonLUT[k] & 0x0F)));
}

bool wasKeyPressed(uint8_t k){
	return (lastkeys[buttonLUT[k] >> 4] & (1 << (buttonLUT[k] & 0x0F)));
}
bool justPressed(uint8_t k){
	return (isKeyPressed(k) & !wasKeyPressed(k));
}
bool isLED(uint8_t x){
	return (displaybuffer[ledLUT[x] >> 4] & (1 << (ledLUT[x] & 0x0F)));
}

int main(int argc, char **argv)
{
	int fh, result;
	uint8_t i =0;
	
	signal(SIGINT, INThandler);
	fh = open("/dev/i2c-0", O_RDWR);

	if (fh < 0) {
		perror("open");
		exit(1);
	}
	ioctl(fh, 0x0703, 0x70);
	//begin
	i2c_smbus_write_byte(fh, 0x21);
	i2c_smbus_write_byte(fh, (HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (0 << 1)));
	i2c_smbus_write_byte(fh, (HT16K33_CMD_BRIGHTNESS | 15));
	i2c_smbus_write_byte(fh, 0xA1);

	for(i=0;i<16;i++)
	{
		setLED(i);
		sleep(1);
		writeDisplay(fh);
	}

	for(i=0;i<16;i++)
	{
		clrLED(i);
		sleep(1);
		writeDisplay(fh);
	}
	int asd;
	while(1){
		int n = readSwitches(fh);
		if (n){
			for(i=0;i<16;i++){
				if (justPressed(i)){
					if(isLED(i)){
						clrLED(i);
					}
					else
						setLED(i);				
				}
			}
		}
		writeDisplay(fh);
		sleep(1);
	}
}

void setLED(int x)
{
	displaybuffer[ledLUT[x] >> 4] |= (1 << (ledLUT[x] & 0x0F));
}

void writeDisplay(int fh)
{
	uint8_t data[16];
	int i;
	for(i = 0; i<8; i++) 
	{	
		data[2*i] = (displaybuffer[i] & 0xFF);
		data[2*i +1] = (displaybuffer[i] >> 8);
		}
	/*printf("\ndata write\n");
	for(i=0;i<16;i++){
		printf("%d ",data[i]);	
	}*/
	i2c_smbus_write_i2c_block_data(fh, 0x00, 16,
                (__u8 *)data);
}

void clrLED(int x)
{
	displaybuffer[ledLUT[x] >> 4] &= ~(1 << (ledLUT[x] & 0x0F));
}
