#define lcd_d 0xf<<20
#define rs 1 << 17
#define rw 1<<18
#define e 1 << 19
void delay_ms(unsigned int ms)
{
	T0PR=15000-1;
	T0TCR=0x01;
	while(T0TC<ms);
	T0TCR=0x03;
	T0TCR=0x00;
}
void delay_us(unsigned int us)
{
	T0PR=15-1;
	T0TCR=0x01;
	while(T0TC<us);
	T0TCR=0x03;
	T0TCR=0x00;
}

void LCD_command(unsigned char cmd)
{
	IOCLR1=lcd_d;
	IOSET1=(cmd&0XF0)<<16;
	IOCLR1=rs;
	IOSET1=e;
	delay_ms(2);
	IOCLR1=e;

	IOCLR1=lcd_d;
	IOSET1=(cmd&0X0f)<<20;
	IOCLR1=rs;
	IOSET1=e;
	delay_ms(2);
	IOCLR1=e;
}
void LCD_data(unsigned char d)
{
	IOCLR1=lcd_d;
	IOSET1=(d&0XF0)<<16;
	IOSET1=rs;
	IOSET1=e;
	delay_ms(2);
	IOCLR1=e;

	IOCLR1=lcd_d;
	IOSET1=(d&0X0F)<<20;
	IOSET1=rs;
	IOSET1=e;
	delay_ms(2);
	IOCLR1=e;
}
void LCD_init(void)
{
	IODIR1|= lcd_d|rs|e|rw;
	IOCLR1=rw;
	LCD_command(0x01);
	LCD_command(0x02);
	LCD_command(0x0c);
	LCD_command(0x28);
	LCD_command(0x80);
}
void LCD_int(int n)
{
	unsigned char arr[5];
	signed char i=0;
	if(n==0)
		LCD_data('0');
	else
		{
			if(n<0)
			{
				LCD_data('-');
				n=-n;
			}
			while(n>0)
			{
				arr[i++]=n%10;
				n=n/10;
			}
			for(i=i-1;i>=0;i--)
				LCD_data(arr[i]+48);
		}
}
void LCD_float(float f)
{
	int temp;
	temp=f;
	LCD_int(temp);
	LCD_data('.');
	temp=(f-temp)*100;
	LCD_int(temp);
}
void LCD_str(unsigned char *s)
{
	int i=0;
	for(i=0;s[i];i++)
		LCD_data(s[i]);
		
}

void LCD_scroll(unsigned char *s)
{
	unsigned int i=0x80;
	while(1)
	{
		LCD_command(0x01);
		LCD_command(i++);
		LCD_str(s);
		delay_ms(300);
		if(i==0x91)
		{
			break;
		}
	}
}
