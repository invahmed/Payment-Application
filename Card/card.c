#include "card.h"

/*
1. This function will ask for the card holder's name and store it into card data.
2. Card holder name is 24 alphabetic characters string max and 20 min.
3. If the card holder name is NULL, less than 20 characters or more than 24 will return WRONG_NAME error,
   else return CARD_OK.
*/
EN_cardError_t getCardHolderName(ST_cardData_t* cardData)
{
	if (!cardData) return WRONG_NAME;
	fseek(stdin, 0, SEEK_END);
	printf("%s\n", "> Card Holder's Name (20 ~ 24 Alphabetic Character):");
	uint8_t CardHolderName[30] = {0};
	fgets(CardHolderName, 30, stdin);
	if (strlen(CardHolderName) > 25 || strlen(CardHolderName) < 21) return WRONG_NAME;
	for (uint16_t i = 0; i < strlen(CardHolderName) - 1; i++)
	{
		if (!isalpha(CardHolderName[i]) && CardHolderName[i] != ' ') return WRONG_NAME;
	}
	CardHolderName[24] = '\0';
	strcpy(cardData->cardHolderName, CardHolderName);
	return CARD_OK;
}

/*
 1. This function will ask for the card expiry date and store it in card data.
 2. Card expiry date is 5 characters string in the format "MM/YY".
 3. If the card expiry date is NULL, less or more than 5 characters,
	or has the wrong format will return WRONG_EXP_DATE error, else return CARD_OK.
	                 0 1 2 3 4
	Card Expiry Date M M / Y Y
 */
EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData)
{
	if (!cardData) return WRONG_EXP_DATE;
	fseek(stdin, 0, SEEK_END);
	printf("%s\n", "> Card Expiry Date ( MM/YY e.g. 12/22): ");
	uint8_t CED[6] = { 0 };
	fgets(CED, 6, stdin);
	for (uint8_t i = 0; i < 5; i++)
	{
		if (i == 2) continue;
		if (!isdigit(CED[i])) return WRONG_EXP_DATE;
	}
	uint8_t MM = (CED[0] - '0') * 10 + (CED[1] - '0');
	uint8_t YY = (CED[3] - '0') * 10 + (CED[4] - '0');
	if (!(MM < 13 && MM > 0 && YY < 100 && strlen(CED) == 5 && CED[2] == '/')) return WRONG_EXP_DATE;
	CED[5] = '\0';
	strcpy(cardData->cardExpirationDate, CED);
	return CARD_OK;
}

/*
1. This function will ask for the card's Primary Account Number and store it in card data.
2. PAN is 20 numeric characters string 19 character max, and 16 character min.
3. If the PAN is NULL, less than 16 or more than 19 characters, will return WRONG_PAN error, else return CARD_OK
*/
EN_cardError_t getCardPAN(ST_cardData_t* cardData)
{
	if (!cardData) return WRONG_PAN;
	fseek(stdin, 0, SEEK_END);
	printf("%s\n", "> Primary Account Number (16 ~ 19 Numeric Characters) : ");
	uint8_t PAN[20] = {0};
	fgets(PAN, 20, stdin);
	if (strlen(PAN) < 17 || strlen(PAN) > 20 || PAN == NULL) return WRONG_PAN;
	for (uint8_t i = 0; i < strlen(PAN) - 1; i++) 
	{
		if (!isdigit(PAN[i])) return WRONG_PAN;
	}
	PAN[19] = '\0';
	strcpy(cardData->primaryAccountNumber, PAN);
	return CARD_OK;
}

