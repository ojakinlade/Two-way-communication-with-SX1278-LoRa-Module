#ifndef BUTTON_H
#define BUTTON_H

#define BUTTON1	5

#define BUTTON_PRESSED 				0
#define BUTTON_NOT_PRESSED 		1

typedef struct
{
	uint8_t pin;
	bool state;
	bool prevPressed;
	bool isDebounced;
}Button_t;

extern void Button_Init(void);
extern void Button_Poll(Button_t* pButton);
void Button_and_StateTransition(Button_t* pButton, uint8_t state);


#endif /* BUTTON_H */
