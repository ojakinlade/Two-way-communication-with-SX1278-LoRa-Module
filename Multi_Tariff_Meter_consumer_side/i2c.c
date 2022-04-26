#include "stm32f10x.h"                  // Device header
#include "i2c.h"

void I2C_Init(I2C_TypeDef* i2cPort, 
							uint32_t i2cClock, 
							uint32_t i2cCCR, 
							uint32_t i2cTRISE)
{
	/*
	Description:
	Initializes I2C peripheral
	
	Parameters:
	1.) i2cPort: pointer to I2C_TypeDef struct which contains
	all registers for the desired I2C peripheral
	e.g. if i2cPort is passed an argument of I2C1, the I2C1 peripheral
	is configured and if passed an argument of I2C2, the I2C2 peripheral
	is configured.
	
	2.) i2cClock: I2C peripheral clock frequency e.g. if passed an argument of
	I2C_PERIPH_FREQ_8MHZ, the selected I2C peripheral is clocked at 8MHz.
	
	3.) i2cCCR: I2C peripheral clock configuration. When this parameter is passed with 
	an argument of I2C_STANDARD_MODE_8MHZ_CCR, the I2C peripheral generates a 100kHz clock
	(standard mode) from an 8MHz input.
	
	4.) i2cTRISE: I2C peripheral maximum rise time. If this parameter is passed with
	an argument of I2C_STANDARD_MODE_8MHZ_TRISE, the I2C peripheral maximum rise time is
	configured as 1000ns (i.e. inverse of 100kHz [standard mode] ).
	
	Return:
	None
	
	*/
	
	//I2C software reset to eliminate BUSY flag glitch
	i2cPort->CR1 |= I2C_CR1_SWRST;
	i2cPort->CR1 &= ~I2C_CR1_SWRST;
	
	i2cPort->CR2 |= i2cClock;
	i2cPort->CCR = i2cCCR;
	i2cPort->TRISE = i2cTRISE;
	i2cPort->CR1 |= I2C_CR1_PE; //Enable I2C1 peripheral
}

void I2C_WriteByte(I2C_TypeDef* i2cPort, 
									 uint8_t slaveAddr,
									 uint8_t regAddr,
									 uint8_t data)
{
	/*
	Description:
	Allows single byte data to be written to the I2C bus
	
	Parameters:
	1.) i2cPort: pointer to I2C_TypeDef struct which contains
	all registers for the desired I2C peripheral
	e.g. if i2cPort is passed an argument of I2C1, the I2C1 peripheral
	is configured and if passed an argument of I2C2, the I2C2 peripheral
	is configured.
	
	2.) slaveAddr: 7-bit address of slave device connected to the stm32's
	I2C bus. The argument passed to this parameter depends on the I2C slave device
	used.
	
	3.) regAddr: address of slave device's register which contains specific
	data. The argument passed to this parameter depends on the I2C slave device
	used.
	
	4.) data: data to be written to the I2C bus.
	
	Return:
	None
	
	*/
	
	volatile uint32_t read_I2C_SR2;
	
	while ((i2cPort->SR2 & I2C_SR2_BUSY) == I2C_SR2_BUSY); //wait for I2C busy bit to be cleared	 
	i2cPort->CR1 |= I2C_CR1_START; //Generate start condition
	while((i2cPort->SR1 & I2C_SR1_SB) != I2C_SR1_SB);//wait for start bit to be set
	i2cPort->DR = slaveAddr << 1; //slave address
	while((i2cPort->SR1 & I2C_SR1_ADDR) != I2C_SR1_ADDR);//wait for ADDR bit to be set
	read_I2C_SR2 = i2cPort->SR2;
	while((i2cPort->SR1 & I2C_SR1_TXE) != I2C_SR1_TXE);//wait for TXE bit to be set
	i2cPort->DR = regAddr;
	while((i2cPort->SR1 & I2C_SR1_TXE) != I2C_SR1_TXE); 
	i2cPort->DR = data;
	while( ((i2cPort->SR1 & I2C_SR1_TXE) != I2C_SR1_TXE) || 
				 ((i2cPort->SR1 & I2C_SR1_BTF) != I2C_SR1_BTF) ); 
	i2cPort->CR1 |= I2C_CR1_STOP; 
		
}

void I2C_WriteMultiByte(I2C_TypeDef* i2cPort, 
												uint8_t slaveAddr,
												uint8_t regAddr,
												uint8_t* pData,
												uint32_t length)

{
	/*
	Description:
	Allows multiple byte data to be written to the I2C bus
	
	Parameters:
	1.) i2cPort: pointer to I2C_TypeDef struct which contains
	all registers for the desired I2C peripheral
	e.g. if i2cPort is passed an argument of I2C1, the I2C1 peripheral
	is configured and if passed an argument of I2C2, the I2C2 peripheral
	is configured.
	
	2.) slaveAddr: 7-bit address of slave device connected to the stm32's
	I2C bus. The argument passed to this parameter depends on the I2C slave device
	used.
	
	3.) regAddr: address of slave device's register which contains specific
	data. The argument passed to this parameter depends on the I2C slave device
	used.
	
	4.) pData: pointer to data to be written to the I2C bus.
	
	5.) length: number of bytes of data to be written to the I2C bus.
	
	Return:
	None
	
	*/
	
	volatile uint32_t read_I2C_SR2;
	
	while ((i2cPort->SR2 & I2C_SR2_BUSY) == I2C_SR2_BUSY); //wait for I2C busy bit to be cleared	 
	i2cPort->CR1 |= I2C_CR1_START; //Generate start condition
	while((i2cPort->SR1 & I2C_SR1_SB) != I2C_SR1_SB);//wait for start bit to be set
	i2cPort->DR = slaveAddr << 1; //slave address
	while((i2cPort->SR1 & I2C_SR1_ADDR) != I2C_SR1_ADDR);//wait for ADDR bit to be set
	read_I2C_SR2 = i2cPort->SR2;
	while((i2cPort->SR1 & I2C_SR1_TXE) != I2C_SR1_TXE);//wait for TXE bit to be set
	i2cPort->DR = regAddr;
	
	for (uint32_t i = 0; i < length; i++)
	{
		while((i2cPort->SR1 & I2C_SR1_TXE) != I2C_SR1_TXE); 
		i2cPort->DR = pData[i];
	}
	
	while( ((i2cPort->SR1 & I2C_SR1_TXE) != I2C_SR1_TXE) || 
				 ((i2cPort->SR1 & I2C_SR1_BTF) != I2C_SR1_BTF) ); 
	i2cPort->CR1 |= I2C_CR1_STOP; 
}

void I2C_ReadByte(I2C_TypeDef* i2cPort,
									uint8_t slaveAddr,
									uint8_t regAddr,
									uint8_t* pData)
{
	/*
	Description:
	Allows single byte data to be read from the I2C bus
	
	Parameters:
	1.) i2cPort: pointer to I2C_TypeDef struct which contains
	all registers for the desired I2C peripheral
	e.g. if i2cPort is passed an argument of I2C1, the I2C1 peripheral
	is configured and if passed an argument of I2C2, the I2C2 peripheral
	is configured.
	
	2.) slaveAddr: 7-bit address of slave device connected to the stm32's
	I2C bus. The argument passed to this parameter depends on the I2C slave device
	used.
	
	3.) regAddr: address of slave device's register which contains specific
	data. The argument passed to this parameter depends on the I2C slave device
	used.
	
	4.) pData: pointer to data to be read from the I2C bus.
	
	Return:
	None
	
	*/
	
	volatile uint32_t read_I2C_SR2;
	
	while ((i2cPort->SR2 & I2C_SR2_BUSY) == I2C_SR2_BUSY); //wait for I2C busy bit to be cleared 
	i2cPort->CR1 |= I2C_CR1_START; //Generate start condition
	while((i2cPort->SR1 & I2C_SR1_SB) != I2C_SR1_SB);//wait for start bit to be set
	i2cPort->DR = slaveAddr << 1; //slave address
	while((i2cPort->SR1 & I2C_SR1_ADDR) != I2C_SR1_ADDR); //wait for ADDR bit to be set
	read_I2C_SR2 = i2cPort->SR2;
	while((i2cPort->SR1 & I2C_SR1_TXE) != I2C_SR1_TXE); //wait for TXE bit to be set
	i2cPort->DR = regAddr;
		
	while((i2cPort->SR1 & I2C_SR1_TXE) != I2C_SR1_TXE); 
	i2cPort->CR1 |= I2C_CR1_START; 
	while((i2cPort->SR1 & I2C_SR1_SB) != I2C_SR1_SB);
	i2cPort->DR = slaveAddr << 1 | 1;
	
	while((i2cPort->SR1 & I2C_SR1_ADDR) != I2C_SR1_ADDR); 
	i2cPort->CR1 &= ~I2C_CR1_ACK;//Send NACK
	read_I2C_SR2 = i2cPort->SR2;
		
	i2cPort->CR1 |= I2C_CR1_STOP; //Send STOP
	while((i2cPort->SR1 & I2C_SR1_RXNE) != I2C_SR1_RXNE); //Wait for RXNE bit to be set
	*pData = i2cPort->DR;
		
}

void I2C_Read2Bytes(I2C_TypeDef* i2cPort, 
										uint8_t slaveAddr,
										uint8_t regAddr,
										uint8_t* pData)
{
	/*
	Description:
	Allows two byte data to be read from the I2C bus
	
	Parameters:
	1.) i2cPort: pointer to I2C_TypeDef struct which contains
	all registers for the desired I2C peripheral
	e.g. if i2cPort is passed an argument of I2C1, the I2C1 peripheral
	is configured and if passed an argument of I2C2, the I2C2 peripheral
	is configured.
	
	2.) slaveAddr: 7-bit address of slave device connected to the stm32's
	I2C bus. The argument passed to this parameter depends on the I2C slave device
	used.
	
	3.) regAddr: address of slave device's register which contains specific
	data. The argument passed to this parameter depends on the I2C slave device
	used.
	
	4.) pData: pointer to data to be read from the I2C bus.
	
	Return:
	None
	
	*/
	
	volatile uint32_t read_I2C_SR2;
	
	while ((i2cPort->SR2 & I2C_SR2_BUSY) == I2C_SR2_BUSY); //wait for I2C busy bit to be cleared 
	i2cPort->CR1 |= I2C_CR1_START; //Generate start condition
	while((i2cPort->SR1 & I2C_SR1_SB) != I2C_SR1_SB);//wait for start bit to be set
			
	i2cPort->DR = slaveAddr << 1; //slave address
	while((i2cPort->SR1 & I2C_SR1_ADDR) != I2C_SR1_ADDR); //wait for ADDR bit to be set
	read_I2C_SR2 = i2cPort->SR2;
	while((i2cPort->SR1 & I2C_SR1_TXE) != I2C_SR1_TXE);//wait for TXE bit to be set
	i2cPort->DR = regAddr;
		
	while((i2cPort->SR1 & I2C_SR1_TXE) != I2C_SR1_TXE); 
	i2cPort->CR1 |= I2C_CR1_START; 
	while((i2cPort->SR1 & I2C_SR1_SB) != I2C_SR1_SB);
	i2cPort->DR = slaveAddr << 1 | 1;
	
	while((i2cPort->SR1 & I2C_SR1_ADDR) != I2C_SR1_ADDR); 
	i2cPort->CR1 &= ~I2C_CR1_ACK;//Send NACK
	i2cPort->CR1 |= I2C_CR1_POS; 
	read_I2C_SR2 = i2cPort->SR2;
		
	while((i2cPort->SR1 & I2C_SR1_BTF) != I2C_SR1_BTF);//Wait for BTF bit to be set
	i2cPort->CR1 |= I2C_CR1_STOP; //Send STOP
	pData[0] = i2cPort->DR;
	pData[1] = i2cPort->DR;
	
}

void I2C_ReadMultiByte(I2C_TypeDef* i2cPort,
											 uint8_t slaveAddr,
											 uint8_t regAddr,
											 uint8_t* pData, 
											 uint32_t length)
{
	/*
	Description:
	Allows multiple byte data (greater than or equal to 3 bytes) to be 
	read from the I2C bus
	
	Parameters:
	1.) i2cPort: pointer to I2C_TypeDef struct which contains
	all registers for the desired I2C peripheral
	e.g. if i2cPort is passed an argument of I2C1, the I2C1 peripheral
	is configured and if passed an argument of I2C2, the I2C2 peripheral
	is configured.
	
	2.) slaveAddr: 7-bit address of slave device connected to the stm32's
	I2C bus. The argument passed to this parameter depends on the I2C slave device
	used.
	
	3.) regAddr: address of slave device's register which contains specific
	data. The argument passed to this parameter depends on the I2C slave device
	used.
	
	4.) pData: pointer to data to be read from the I2C bus.
	
	5.) length: number of bytes of data to be read from the I2C bus.
	
	Return:
	None
	
	*/
	
	if (length == 0)
	{//Invalid input
		return;
	}
	
	else if (length == 1)
	{
		I2C_ReadByte(i2cPort,slaveAddr,regAddr,pData);
	}
	
	else if (length == 2)
	{
		I2C_Read2Bytes(i2cPort,slaveAddr,regAddr,pData);
	}
	
	else
	{
		volatile uint32_t read_I2C_SR2;
		while ((i2cPort->SR2 & I2C_SR2_BUSY) == I2C_SR2_BUSY); //wait for I2C busy bit to be cleared
		i2cPort->CR1 |= I2C_CR1_START; //Generate start condition
		while((i2cPort->SR1 & I2C_SR1_SB) != I2C_SR1_SB); //wait for start bit to be set
				
		i2cPort->DR = slaveAddr << 1; //slave address
		while((i2cPort->SR1 & I2C_SR1_ADDR) != I2C_SR1_ADDR); //wait for ADDR bit to be set
		read_I2C_SR2 = i2cPort->SR2;
		while((i2cPort->SR1 & I2C_SR1_TXE) != I2C_SR1_TXE); //wait for TXE bit to be set
		i2cPort->DR = regAddr;
			
		while((i2cPort->SR1 & I2C_SR1_TXE) != I2C_SR1_TXE); 
		i2cPort->CR1 |= I2C_CR1_START; 
		while((i2cPort->SR1 & I2C_SR1_SB) != I2C_SR1_SB); 
		i2cPort->DR = slaveAddr << 1 | 1;
		
		while((i2cPort->SR1 & I2C_SR1_ADDR) != I2C_SR1_ADDR); 
		read_I2C_SR2 = i2cPort->SR2;
		i2cPort->CR1 |= I2C_CR1_ACK; //Send ACK
		
		//Read incoming data
		for (uint32_t i = 0; i < length - 3; i++)
		{
			while((i2cPort->SR1 & I2C_SR1_RXNE) != I2C_SR1_RXNE); //Wait for RXNE bit to be set
			pData[i] = i2cPort->DR;
		}
		
		while((i2cPort->SR1 & I2C_SR1_BTF) != I2C_SR1_BTF);//Wait for BTF bit to be set
		i2cPort->CR1 &= ~I2C_CR1_ACK;//Send NACK	
		pData[length - 3] = I2C1->DR;
			
		while((i2cPort->SR1 & I2C_SR1_BTF) != I2C_SR1_BTF);
		i2cPort->CR1 |= I2C_CR1_STOP; //Send STOP
		pData[length - 2] = i2cPort->DR;
		pData[length - 1] = i2cPort->DR;
	}
}
