#ifndef HC12_H
#define HC12_H

#define IDLE_CHARACTER 255

extern void HC12_Init(void);
extern void HC12_TransmitBytes(uint8_t* bytes, uint8_t len);
extern void HC12_RxBufferInit(uint8_t* pBuffer, uint8_t bufferSize);
extern bool HC12_Rx_BufferFull(void);
extern void HC12_RxBufferErrorHandler(uint8_t* pData);
extern bool HC12_IncompleteRxData(void);



#endif /* HC12_H */