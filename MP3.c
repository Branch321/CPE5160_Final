#include "MP3.h"
#include "Directory_Functions_struct.h"
#include "sEOS.h"
#include "main.h"
#include "PORT.H"
#include "Read_Sector.h"
#include "SPI.h"

#define send_data_1 0 //same as below
#define data_send_1 0 // same as above //need to combine
#define data_idle_1 1
#define locate_cluster_2 2
#define load_buf_2 3
#define data_send_2 4
#define load_buf_1 5
#define data_idle_2 6
#define locate_cluster_1 7

sbit BIT_EN = P1^3;
sbit DATA_REQ = P1^2;

xdata uint8_t buffer1[512];
xdata uint8_t buffer2[512];

FS_values_t * Drive_p;
uint32_t idata cluster_g;
uint32_t idata sector_g;
uint32_t idata num_sectors_g;
uint32_t idata index1_g;
uint32_t idata index2_g;
uint8_t idata play_state_g;
uint8_t idata play_status_g;

void Play_MP3_file(uint32_t begin_cluster)
{
	Drive_p = Export_Drive_values();
	cluster_g = begin_cluster;
	sector_g = First_Sector(begin_cluster);
	Read_Sector((sector_g+num_sectors_g),(Drive_p->BytesPerSec),buffer1);
	num_sectors_g++;
	index1_g=0;
	Read_Sector((sector_g+num_sectors_g),(Drive_p->BytesPerSec),buffer2);
	num_sectors_g++;
	index2_g=0;

	play_state_g=send_data_1;
	play_status_g=1;
	sEOS_init(12);
	while(1)
	{
		if(play_status_g==0)
		{
			TR2=0;
			break;
		}
		else
		{
			PCON|=0x01;
		}
	}
}

void MP3_Player_ISR(void) interrupt Timer_2_Overflow
{
	uint8_t temp8;

	TF2 = 0;

	//start timeout here

	switch(play_state_g)
	{
		case data_idle_1:
		{
			if(DATA_REQ==0)
			{
				play_state_g = data_send_1;
			}
			break;
		}
		case data_send_1:
		{
			while((DATA_REQ==0)&&(TF0==0))
			{
				BIT_EN=1;
				SPI_Transfer(buffer1[index1_g],&temp8);
				BIT_EN=0;
				index1_g++;
				if(index1_g>511)
				{
					if(index2_g>511)
					{
						if(num_sectors_g==(Drive_p->SecPerClus))
						{
							play_state_g = locate_cluster_2;
						}
						else
						{
							play_state_g=load_buf_2;
						}
					}
					else
					{
						play_state_g=data_send_2;
					}

				}
			}
			if((DATA_REQ==1)&&(play_state_g==send_data_1))
			{
				if(index2_g>511)
				{
					if(num_sectors_g==(Drive_p->SecPerClus))
					{
						play_state_g=locate_cluster_2;
					}
					else
					{
						play_state_g=load_buf_2;
					}
				}
				else
				{
					//play_state_g=update_lcd_1; //dont know about this one yet 
				}
			}
			break;
		}
		case load_buf_1:
		{
			Read_Sector((sector_g+num_sectors_g),(Drive_p->BytesPerSec),buffer1);
			num_sectors_g++;
			index1_g=0;
			play_state_g=data_idle_2;
			break;
		}
		case load_buf_2:
		{
			Read_Sector((sector_g+num_sectors_g),(Drive_p->BytesPerSec),buffer2);
			num_sectors_g++;
			index2_g=0;
			play_state_g=data_idle_1;
			break;
		}
		case locate_cluster_1:
		{
			cluster_g = Find_Next_Clus(cluster_g,buffer1);
			if(cluster_g!=0x0FFFFFFF)
			{
				sector_g = First_Sector(cluster_g);
				num_sectors_g=0;
				play_state_g=data_idle_2;
			}
			else
			{
				play_status_g=0;
				play_state_g=data_idle_2;
			}
			break;
		}
	}//end of switch case
}


