	 /* 2015.09 */
 /* 广东石油化工学院 */
 /* Ryan_yuans 	袁柏宁*/
 /* 基于液晶显示 — 计算机 */

#include<reg52.h>

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long
	
uchar step = 0;
ulong num1 = 0;
ulong num2 = 0;
uchar mark = 0;
ulong result = 0;

uchar T0RH;
uchar T0RL;

extern void ClearAll();
extern void KeyDriver();
extern void ShowStr(uchar x,uchar y,uchar *str);
extern void ClearArea(uchar x,uchar y,uchar len);
extern void ClearAll();
extern void KeyScan();
	
void main()
{
	void ConfigTimer0(uint ms);
	void Reset1602();

	
	EA = 1;
	ConfigTimer0(1);
	Reset1602();
	ShowStr(15,1,"0");
	
	while(1)
	{
			KeyDriver();
	}
}

void ConfigTimer0(uint ms)			// 配置T0定时器中断
{
	ulong tmp;
	
	tmp = 11059200 / 12;
	tmp = (tmp * ms) / 1000;
	tmp = 65536 - tmp;
	tmp = tmp + 28;
	T0RH = (uchar)(tmp >> 8);
	T0RL = (uchar)tmp;
	TH0 = T0RH;
	TL0 = T0RL;
	TMOD &= 0XF0;
	TMOD |= 0X01;
	ET0 = 1;
	TR0 = 1;
}

unsigned char LongToString(uchar *str,signed long num)		// 长整型数转换为字符串,str为字符串指针
{													// dat为待转换数, 返回值为字符串长度
	uchar i = 0;
	uchar len = 0;
	uchar buf[12];
	
	if(num < 0)
	{
			num = -num;
			*str++ = '-';
			len++;
	}
	do
	{
			buf[i] = num % 10;
			num /= 10;
			i++;
	}while(num > 0);
	
	len += i;
	
	while(i-- > 0)
	{
			*str++ = buf[i] + '0';
	}
	*str++ = '\0';
	
	return len;
}

void ResetKey()			// 计算器复位 , 复位num1 num2 step ..
{
	num1 = 0;
	num2 = 0;
	step = 0;
	ClearAll();
}

void ShowMark(uchar row,uchar type)		// 在指令的行首显示运算符(+ - * /)
{
	
	switch(type)
	{
		case 0:
				ShowStr(0,row,"+"); break;
		case 1:
				ShowStr(0,row,"-"); break;
		case 2:
				ShowStr(0,row,"*"); break;
		case 3:
				ShowStr(0,row,"/"); break;
		default:
				break;
	}
	
}
void NumKeyAction(uchar keycode)		// 当按下 —— 数字键 , 操作1
{
	uchar len;
	uchar str[12];
	
	if(step > 1)
	{
			ResetKey();
	}
	if(step == 0)
	{
			num1 = num1*10 + keycode;
			len = LongToString(str,num1);	// 将长整型num1转换为字符串str, 返回字符串长度len
			ShowStr(16 - len,1,str);		// 将字符串str显示在液晶屏中
			;
	}
	else if(step == 1)
	{
			num2 = num2*10 + keycode;
			len = LongToString(str,num2);
			ShowStr(16 - len,1,str);
	}
}

void FuncKeyAction(uchar type)		// 当按下 —— 功能键 , 操作2
{
	uchar len;
	uchar str[12];
	
	if(step == 0)
	{
			len = LongToString(str,num1);			// 求出 num1 的长度，并将 num1 转换为字符存入 str 中
			ClearArea(0,0,16 - len);
			ShowStr(16 - len,0,str);
			ShowMark(1,type);
			ClearArea(1,1,14);
			ShowStr(15,1,"0");
			mark = type;
			step = 1;
	}
	else if(step == 1)
	{
		switch(mark)
			{
				case 0:
						num1 = num1 + num2; break;
				case 1:
						num1 = num1 - num2; break;
				case 2:
						num1 = num1 * num2; break;
				case 3:
						num1 = num1 / num2; break;
				default:
						break;
			}
		num2 = 0;
		len = LongToString(str,num1);			// 求出 num1 的长度，并将 num1 转换为字符存入 str 中
		ClearArea(0,0,16 - len);
		ShowStr(16 - len,0,str);
		ShowMark(1,type);
		ClearArea(1,1,14);
		ShowStr(15,1,"0");
		mark = type;
		step = 1;
	}
}

void GetResult()		// 当按下 —— 回车键 , 操作3 (执行 功能键指令的操作结果 ) 
{
	uchar len;
	uchar str[12];
	
	if(step == 1)
	{
			step = 1;
			switch(mark)
			{
				case 0:
						result = num1 + num2; break;
				case 1:
						result = num1 - num2; break;
				case 2:
						result = num1 * num2; break;
				case 3:
						result = num1 / num2; break;
				default:
						break;
			}
			len = LongToString(str,num2);
			ShowMark(0,mark);			// 显示第一行(num1)运算符
			ClearArea(1,0,16 - len -1);			// 清除第一行中间
			ShowStr(16 - len,0,str);			// 显示第一行
			len = LongToString(str,result);
			ShowStr(0,1,"=");
			ClearArea(1,1,16 - len - 1);
			ShowStr(16 - len,1,str);
			
	}
}


void KeyAction(uchar keycode)		// 当按键有动作时 执行指定操作
{
	
	if((keycode >= '0') && (keycode <= '9'))
	{
			NumKeyAction(keycode - '0');
	}
	else if(keycode == 0X26)	// 向上按键 ‘+’
	{
			FuncKeyAction(0);	
	}
	else if(keycode == 0X28)	// 向下按键 ‘-’
	{
			FuncKeyAction(1);
	}
	else if(keycode == 0X25)	// 向左按键 ‘*’
	{
			FuncKeyAction(2);
	}
	else if(keycode == 0X27)	// 向右按键 ‘/’
	{
			FuncKeyAction(3);
	}
	else if(keycode == 0X0D)	// 回车键	
	{
			GetResult();
	}
	else if(keycode == 0X1B)	// 复位键
	{
			ResetKey();
			ShowStr(15,1,"0");
	}
}

void Timer0() interrupt 1		// T0中断 执行按键扫描
{
	TH0 = T0RH;
	TL0 = T0RL;
	KeyScan();
}