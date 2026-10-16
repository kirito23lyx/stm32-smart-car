#include "stm32f10x.h"                  // Device header
#include "LEDSEG.h"
#include "Delay.h"
#include "robot.h"
#include "Incontrol.h"
#include "Irtracking.h"
#include "Irobstacle.h"
#include "Key.h"
#include "Serial.h"

unsigned char temp = 1;    // 切换程序标志位
void Robot_Traction()                     //机器人循迹子程序
{
	if(Left_Irtracking_Get() == 0 && Right_Irtracking_Get() == 0)
	{
		makerobo_run(70,0);   // 前进
	}
	else if(Left_Irtracking_Get() == 1 && Right_Irtracking_Get() == 0) //左侧压线
	{
		makerobo_Left(70,0);    //左侧修正
	}
	else if(Left_Irtracking_Get() == 0 && Right_Irtracking_Get() == 1) // 右侧压线
	{
		makerobo_Right(70,0); // 右侧修正
	}			
	else if(Left_Irtracking_Get() == 1 && Right_Irtracking_Get() == 1) // 全部压线，停止
	{
	 makerobo_brake(0); // 停止
	}
}

void Robot_Avoidance()     // 红外避障子程序
{
	if(Left_Irobstacle_Get() == 1 && Right_Irobstacle_Get() == 1)
			{
				makerobo_run(70,0);   // 前进
			}
			else if(Left_Irobstacle_Get() == 1 && Right_Irobstacle_Get() == 0) //右侧检测到红外信号
			{
				makerobo_Left(70,400); 
			}
			else if(Left_Irobstacle_Get() == 0 && Right_Irobstacle_Get() == 1) // 左侧检测到红外信号
			{
				makerobo_Right(70,400);
			}			
			else if(Left_Irobstacle_Get() == 0 && Right_Irobstacle_Get() == 0) // 两侧传感器同时检测到红外信号
			{
				makerobo_brake(400);          // 调用电机后退函数
				makerobo_Spin_Right(70,900);  // 调用电机右转函数
			}
}

void ControlCar_Ircontrol(void) // 定义红外遥控器控制
{
	uint8_t command;
	uint32_t receive_code;

	/* 没有新的完整遥控数据时，不读取命令也不改变电机状态。 */
	if(IR_Receiveflag == 0)
	{
		return;
	}

	receive_code = IR_Receivecode;
	IR_Receiveflag = 0;
	IR_Receivecode = 0;
	command = (uint8_t)((receive_code >> 8) & 0xFF);

	printf("红外接收码 %0.8X\r\n", receive_code);
	printf("红外命令字 0x%0.2X\r\n", command);

	switch(command)
	{
		case 0xB1: // 前进2s
			makerobo_run(100,2000);
			makerobo_brake(0);
			break;

		case 0xD4: // 后退2s
			makerobo_back(70,2000);
			makerobo_brake(0);
			break;

		case 0x91: // 左转2s
			makerobo_Spin_Left(70,2000);
			makerobo_brake(0);
			break;

		case 0xE1: // 右转2s
			makerobo_Spin_Right(70,2000);
			makerobo_brake(0);
			break;

		case 0x81: // 停止
		default:   // 未知命令也停车
			makerobo_brake(0);
			break;
	}
}

void Irscan(void)
{
	uint8_t Tim=0,Ok=0,Data,Num=0;
	while(1)
	{
	  if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)==1)
		{
			 Tim=IRremote_Counttime();//获得此次高电平时间

			 if(Tim>=250) break;//不是有用的信号

			 if(Tim>=200 && Tim<250)
			 {
			 	Ok=1;//收到起始信号
			 }
			 else if(Tim>=60 && Tim<90)
			 {
			 	Data=1;//收到数据 1
			 }
			 else if(Tim>=10 && Tim<50)
			 {
			 	Data=0;//收到数据 0
			 }

			 if(Ok==1)
			 {
			 	IR_Receivecode<<=1;
				IR_Receivecode+=Data;

				if(Num>=32)
				{
					IR_Receiveflag=1;
				  break;
				}
			 }
			 Num++;
		}
		ControlCar_Ircontrol();
	}
	EXTI_ClearITPendingBit(EXTI_Line8);	
}

int main(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组分2组
	Key_Init();
	IRremote_Init();           // 红外遥控器初始化
	Irtracking_Init();         // 红外循迹初始化
	Irobstacle_Init();         // 红外避障初始化
	LEDSEG_Init();
	Serial_Init();             // 串口初始化
	robot_Init();              // 机器人初始化
	while (1)
	{
	  if(Key_GetNum() == 1)
		{
			temp ++;
		}
		if(temp > 3)
		{
			temp = 1;
		}
		switch(temp)
		{
			case 1: Digital_Display(1);Robot_Traction();EXTI_DeInit();break;
			case 2: Digital_Display(2);Robot_Avoidance();break;
			case 3: Digital_Display(3);Irscan();break;
		}
	}
}
