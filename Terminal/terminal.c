#include "terminal.h"

/*
1. This function will ask for the transaction data and store it in terminal data.
2. Transaction date is 10 characters string in the format DD/MM/YYYY
3. If the transaction date is NULL, less than 10 characters or wrong format will return WRONG_DATE error,
   else return TERMINAL_OK.
*/
EN_terminalError_t getTransactionDate(ST_terminalData_t* termData)
{
	uint8_t TransDate[12] = { 0 };
	time_t Time;
	struct tm* TimeFrame;
	time(&Time);
	TimeFrame = localtime(&Time);
	strftime(TransDate, 12, "%d/%m/%Y", TimeFrame);
	TransDate[10] = '\0';
	if ((TransDate == NULL) || (strlen(TransDate) < 10) || (TransDate[2] != '/') || (TransDate[5] != '/')) return WRONG_DATE;
	else
	{
		strcpy (termData->transactionDate, TransDate);
		return TERMINAL_OK;
	}
}

/*
1. This function compares the card expiry date with the transaction date.
2. If the card expiration date is before the transaction date will return EXPIRED_CARD,
   else return TERMINAL_OK.

                    0 1 2 3 4 5 6 7 8 9
   Transaction Date D D / M M / Y Y Y Y
   Card Expiry Date M M / Y Y
*/
EN_terminalError_t isCardExpired(ST_cardData_t *cardData, ST_terminalData_t *termData)
{
	uint8_t CEMM = (cardData->cardExpirationDate[0] - '0') * 10 + (cardData->cardExpirationDate[1] - '0');
	uint8_t CEYY = (cardData->cardExpirationDate[3] - '0') * 10 + (cardData->cardExpirationDate[4] - '0');
	uint8_t TDMM = (termData->transactionDate[3]    - '0') * 10 + (termData->transactionDate[4]    - '0');
	uint8_t TDYY = (termData->transactionDate[8]    - '0') * 10 + (termData->transactionDate[9]    - '0');
	if ((CEYY == TDYY && CEMM >= TDMM) || (CEYY > TDYY)) return TERMINAL_OK;
	else return EXPIRED_CARD;
}

/*
1. This function asks for the transaction amount and saves it into terminal data.
2. If the transaction amount is less than or equal to 0 will return INVALID_AMOUNT,
   else return TERMINAL_OK.
*/
EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
	float TransAmount;
	printf("> Transaction Amount (e.g. 1500) : ");
	scanf("%f", &TransAmount);
	if (TransAmount <= 0) return INVALID_AMOUNT;
	else
	{
		termData->transAmount = TransAmount;
		return TERMINAL_OK;
	}
}

/*
1. This function takes the maximum allowed amount into terminal data.
2. Transaction max amount is a float number.
3. If transaction max amount less than or equal to 0 will return INVALID_MAX_AMOUNT error,
   else return TERMINAL_OK.
*/
EN_terminalError_t setMaxAmount(ST_terminalData_t* termData)
{
	float maxAmount = 0;
	printf("> Max Transaction Amount (e.g. 3000) : ");
	scanf("%f", &maxAmount);
	if (maxAmount <= 0) return INVALID_MAX_AMOUNT;
	else
	{
		termData->maxTransAmount = maxAmount;
		return TERMINAL_OK;
	}
}

/*
1. This function compares the transaction amount with the terminal max allowed amount.
2. If the transaction amount is larger than the terminal max allowed amount will return EXCEED_MAX_AMOUNT,
   else return TERMINAL_OK.
*/
EN_terminalError_t isBelowMaxAmount(ST_terminalData_t *termData)
{
	if (termData->transAmount < termData->maxTransAmount) return TERMINAL_OK;
	else return EXCEED_MAX_AMOUNT;
}

