#ifdef WS2812B_TYPE2

#define RED_INDEX   0
#define GREEN_INDEX 1
#define BLUE_INDEX  2
#define LED_NUM     4
#define GRB_BIT     24   //3*8

#define TIMING_ONE  40
#define TIMING_ZERO 18

//DMA以半字格式发出数据，避免数据强转使用半字的缓存区
uint16_t LED_BYTE_Buffer[300];

//功能：用于打开PWM，发出数据使灯亮起来
void WS281x_Show(uint16_t send_len)
{
    HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_4, (uint32_t *)&LED_BYTE_Buffer, send_len);  
}

//传参：需要显示的RGB数据和灯的个数
//功能：把RGB数据转换成WS2812需要的GRB数据，并按照24bit格式打包给每个灯的数据
void WS2812_send(uint8_t *rgb, uint16_t len)
{
	uint8_t i;
	uint16_t memaddr = 0;
	uint16_t buffersize;
	buffersize = len * GRB_BIT;	// number of bytes needed is #LEDs * 24 bytes
    
	while (len)
	{
        for(i=0; i<8; i++) // GREEN data
        {
            LED_BYTE_Buffer[memaddr] = ((rgb[GREEN_INDEX]<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
            memaddr++;
        }
        for(i=0; i<8; i++) // RED
        {
            LED_BYTE_Buffer[memaddr] = ((rgb[RED_INDEX]<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
            memaddr++;
        }
        for(i=0; i<8; i++) // BLUE
        {
            LED_BYTE_Buffer[memaddr] = ((rgb[BLUE_INDEX]<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
            memaddr++;
        }
        
        len--;
	}
	
    WS281x_Show(memaddr);
}




#define PROCESS_DELAY   5
#define SECTION_DELAY   100

/*
实现彩虹灯通过手动扫描可以发现有几个阶段，每个阶段R/G/B三基色的数值按规律变化，得到以下代码中switch内的几个状态，直观但有点笨，希望有更好的实现方式有小伙伴可以跟我交流
*/
void Rainbow_Flow(void)
{
    uint8_t state = 1;
    uint8_t RGBList[3] = {255, 0, 0};	//从红色开始彩虹灯
    
    while(1)
    {
        if (退出彩虹灯的条件为真（如按键等）)
        {
            break;
        }
        for(uint8_t cnt = 0; cnt < (255-1); cnt++)	//cnt为uint8_t时此处如果条件为<255将死循环，不会有全光谱彩虹灯
        {
            switch (state)
            {
              case 1:
                RGBList[GREEN_INDEX] = cnt;
                break;
              case 2:
                RGBList[RED_INDEX] = 255 - cnt;
                break;
              case 3:
                RGBList[BLUE_INDEX] = cnt;
                break;
              case 4:
                RGBList[GREEN_INDEX] = 255 - cnt;
                break;
              case 5:
                RGBList[RED_INDEX] = cnt;
                break;
              case 6:
                RGBList[BLUE_INDEX] = 255 - cnt;
                break;
              default:
                break;
            }
            SysDelay_Xms(PROCESS_DELAY);
            
            WS2812_send(RGBList, LED_NUM);
        }
        if (state < 6)
            state++;
        else
            state = 1;
        
        SysDelay_Xms(SECTION_DELAY);
    }
}

#endif
