#ifndef LCD_H
#define LCD_H

extern void LCD_Init(void);
extern void LCD_Write_Data(char data);
extern void LCD_Write_String(char* pData);
extern void LCD_Clear(void);
extern void LCD_Write_Int(uint32_t data);
extern void LCD_Set_Cursor(uint8_t row, uint8_t column);

#endif /* LCD_H */