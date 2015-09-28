	 /* 2015.09 */
 /* �㶫ʯ�ͻ���ѧԺ */
 /* Ryan_yuans 	Ԭ����*/
 /* ����Һ����ʾ �� ����� */

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

void ConfigTimer0(uint ms)			// ����T0��ʱ���ж�
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

unsigned char LongToString(uchar *str,signed long num)		// ��������ת��Ϊ�ַ���,strΪ�ַ���ָ��
{													// datΪ��ת����, ����ֵΪ�ַ�������
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

void ResetKey()			// ��������λ , ��λnum1 num2 step ..
{
	num1 = 0;
	num2 = 0;
	step = 0;
	ClearAll();
}

void ShowMark(uchar row,uchar type)		// ��ָ���������ʾ�����(+ - * /)
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
void NumKeyAction(uchar keycode)		// ������ ���� ���ּ� , ����1
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
			len = LongToString(str,num1);	// ��������num1ת��Ϊ�ַ���str, �����ַ�������len
			ShowStr(16 - len,1,str);		// ���ַ���str��ʾ��Һ������
			;
	}
	else if(step == 1)
	{
			num2 = num2*10 + keycode;
			len = LongToString(str,num2);
			ShowStr(16 - len,1,str);
	}
}

void FuncKeyAction(uchar type)		// ������ ���� ���ܼ� , ����2
{
	uchar len;
	uchar str[12];
	
	if(step == 0)
	{
			len = LongToString(str,num1);			// ��� num1 �ĳ��ȣ����� num1 ת��Ϊ�ַ����� str ��
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
		len = LongToString(str,num1);			// ��� num1 �ĳ��ȣ����� num1 ת��Ϊ�ַ����� str ��
		ClearArea(0,0,16 - len);
		ShowStr(16 - len,0,str);
		ShowMark(1,type);
		ClearArea(1,1,14);
		ShowStr(15,1,"0");
		mark = type;
		step = 1;
	}
}

void GetResult()		// ������ ���� �س��� , ����3 (ִ�� ���ܼ�ָ��Ĳ������ ) 
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
			ShowMark(0,mark);			// ��ʾ��һ��(num1)�����
			ClearArea(1,0,16 - len -1);			// �����һ���м�
			ShowStr(16 - len,0,str);			// ��ʾ��һ��
			len = LongToString(str,result);
			ShowStr(0,1,"=");
			ClearArea(1,1,16 - len - 1);
			ShowStr(16 - len,1,str);
			
	}
}


void KeyAction(uchar keycode)		// �������ж���ʱ ִ��ָ������
{
	
	if((keycode >= '0') && (keycode <= '9'))
	{
			NumKeyAction(keycode - '0');
	}
	else if(keycode == 0X26)	// ���ϰ��� ��+��
	{
			FuncKeyAction(0);	
	}
	else if(keycode == 0X28)	// ���°��� ��-��
	{
			FuncKeyAction(1);
	}
	else if(keycode == 0X25)	// ���󰴼� ��*��
	{
			FuncKeyAction(2);
	}
	else if(keycode == 0X27)	// ���Ұ��� ��/��
	{
			FuncKeyAction(3);
	}
	else if(keycode == 0X0D)	// �س���	
	{
			GetResult();
	}
	else if(keycode == 0X1B)	// ��λ��
	{
			ResetKey();
			ShowStr(15,1,"0");
	}
}

void Timer0() interrupt 1		// T0�ж� ִ�а���ɨ��
{
	TH0 = T0RH;
	TL0 = T0RL;
	KeyScan();
}