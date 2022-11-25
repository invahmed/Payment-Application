
#include "app.h"

ST_cardData_t cardData;
ST_terminalData_t terminalData;

uint8_t cardError; /* EN_cardError_t cardError; */
uint8_t terminalError; /* EN_terminalError_t terminalError; */
uint8_t serverError; /* EN_serverError_t serverError; */
ST_transaction_t transactionDB;

void appStart(void)
{
	printf("\n");
	printf("       ... Payment Application Console ... \n");
	printf("       ................................... \n\n");
	/****************************************************
	   Getting All Card Data from Card Side
	*****************************************************/
	cardError = getCardHolderName(&cardData);
	while (cardError == WRONG_NAME)
	{
		printf("        [ Incorrect Card Holder's Name!. ] \n");
		cardError = getCardHolderName(&cardData);
	}
	cardError = getCardExpiryDate(&cardData);
	while (cardError == WRONG_EXP_DATE)
	{
		printf("        [ Incorrect Card Expiry Date!. ] \n");
		cardError = getCardExpiryDate(&cardData);
	}
	cardError = getCardPAN(&cardData);
	while (cardError == WRONG_PAN)
	{
		printf("        [ Incorrect Primary Account Number!. ] \n");
		cardError = getCardPAN(&cardData);
	}
	/****************************************************
    Getting All Transactions Data at Terminal Side
    ****************************************************/

	terminalError = getTransactionDate(&terminalData);
	if (terminalError == WRONG_DATE) printf("        [ Incorrect Transaction Date. ] \n");
		
	terminalError = isCardExpired(&cardData, &terminalData);
	if (terminalError == EXPIRED_CARD)
	{
		printf("        [ Declined Expired Card. ] \n");
		return;
	}

	terminalError = setMaxAmount(&terminalData);
	while (terminalError == INVALID_AMOUNT)
	{
		printf("        [ Invalid Max. Amount. ] \n");
		terminalError = setMaxAmount(&terminalData);
	}
	terminalError = getTransactionAmount(&terminalData);
	while (terminalError == INVALID_AMOUNT)
	{
		printf("        [ Invalid Amount. ] \n");
		terminalError = getTransactionAmount(&terminalData);
	}
	terminalError = isBelowMaxAmount(&terminalData);
	if (terminalError == EXCEED_MAX_AMOUNT)
	{
		printf("        [ Declined Amount Exceed Limit. ] \n");
		return;
	}
	/***************************************************
    Verifying All Transactions Date at Server Side
    ****************************************************/
	transactionDB.cardHolderData = cardData;
	transactionDB.terminalData = terminalData;
	serverError = (recieveTransactionData(&transactionDB));

	if (serverError == FRAUD_CARD)
	{
		printf("        [ Declined Fraud Card. ] \n");
		return;
	}
	if (serverError == DECLINED_INSUFFECIENT_FUND)
	{
		printf("        [ Declined Insuffcient Fund. ] \n");
		return;
	}
	if (serverError == BLOCKED_ACCOUNT)
	{
		printf("        [ Declined Stolen Card. ] \n");
		return;
	}
	if (serverError == APPROVED)
	{
		updateAccountsDB();
		return;
	}
	printf("  ................................... \n");
}

