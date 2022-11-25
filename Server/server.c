
#include "server.h"

EN_serverError_t transState;
uint8_t accountDBError;
uint8_t* transactionState;

uint8_t ID = 0; /* Customer ID in Accounts Database */
ST_transaction_t transactionsDB[255] = { 0 };
uint32_t TSN = 1; /* Last Saved Transaction Sequence Number */
uint8_t STATE; /* Transaction State Massage */

ST_cardData_t cardData;
ST_terminalData_t terminalData;
ST_accountsDB_t accountRefrence;

/* There are 10 Luhn verified PANs in  Database. Supposed to be 225 */
ST_accountsDB_t accountsDB[10] = 
{
	{10000.00, RUNNING, "3171581229228826"},
	{20000.00, RUNNING, "1870490307934263"},
	{30000.00, RUNNING, "4264938796199457"},
	{40000.00, RUNNING, "7938441356623296"},
	{50000.00, RUNNING, "5393662746266668"},
	{60000.00, RUNNING, "1029055916669780"},
	{70000.00, RUNNING, "0586376993488259"},
	{80000.00, RUNNING, "2969384271668595"},
	{90000.00, BLOCKED, "8726454632611542"},
	{99990.00, BLOCKED, "6331870948703395"}
};

/*
1. This function will take card data and validate if the account related to this card exists or not.
2. It checks if the PAN exists or not in the server's database (searches for the card PAN in the DB).
3. If the PAN doesn't exist will return ACCOUNT_NOT_FOUND and the account reference will be NULL
   else will return SERVER_OK and return a reference to this account in the DB.
*/
EN_serverError_t isValidAccount(ST_cardData_t* cardData, ST_accountsDB_t* accountRefrence)
{
	/*
	FILE *ADB = fopen("AccountsDB.txt", "r");
	if (ADB == NULL) printf("        [ Accounts Database Read Failed. ]\n");
	uint8_t i = 0;
	uint8_t ROW[50] = { 0 };
	uint8_t *TOKEN;
	float BALANCE;
	uint8_t STATE[8];
	uint8_t PAN[20];
	while (fgets(ROW, 50, ADB) && i < 10)
	{
		// printf("%s", ROW);
		TOKEN = strtok(ROW, "{, ");
		BALANCE = strtof(TOKEN, NULL);
		TOKEN = strtok(NULL, ", ");
		strcpy(STATE, TOKEN);
		TOKEN = strtok(NULL, "} \"");
		strcpy(PAN, TOKEN);
		// printf("%0.2f %s %s\n", BALANCE, STATE, PAN);
	    accountsDB[i].balance = BALANCE;
		if (STATE[8] = "RUNINNG") accountsDB[i].state = RUNNING;
		if (STATE[8] = "BLOCKED") accountsDB[i].state = BLOCKED;
		*accountsDB[i].primaryAccountNumber = PAN;
	}
	*/

	bool EXISTED = false;
	for (uint8_t i = 0; i < 10; i++)
	{
		if (strstr(cardData->primaryAccountNumber, accountsDB[i].primaryAccountNumber))
		{
			ID = i;
			EXISTED = true;
			break;
		}
	}

	if (EXISTED)
	{
		*accountRefrence = accountsDB[ID];
		/*
		accountRefrence->balance = accountsDB[ID].balance;
		accountRefrence->state = accountsDB[ID].state;
		strcpy(accountRefrence->primaryAccountNumber, accountsDB[ID].primaryAccountNumber);
		*/
		accountDBError = SERVER_OK;
	}
	else
	{
		accountRefrence = NULL;
		accountDBError = ACCOUNT_NOT_FOUND;
	}
	// fclose(ADB);
	return accountDBError;
}

/*
This function will take terminal data and validate these data.
It checks if the transaction's amount is available or not.
If the transaction amount is greater than the balance in the database will return LOW_BALANCE,
else will return SERVER_OK
*/
EN_serverError_t isAmountAvailable(ST_terminalData_t* termData, ST_accountsDB_t *accountRefrence)
{
	if (termData->transAmount > accountRefrence->balance) accountDBError = LOW_BALANCE;
	else accountDBError = SERVER_OK;

	return accountDBError;
}

/*
1. This function takes a reference to the account into the database and verifies if it is blocked or not.
2. If the account is running it will return SERVER_OK,
else if the account is blocked it will return BLOCKED_ACCOUNT.
*/
EN_serverError_t isBlockedAccount(ST_accountsDB_t* accountRefrence)
{
	if (accountRefrence->state == BLOCKED) accountDBError = BLOCKED_ACCOUNT;
	else accountDBError = SERVER_OK;

	return accountDBError;
}

/*
1. This function will take all transaction data and validate its data, it contains all server logic.
2. It checks the account details and amount availability.
3. If the account does not exist return FRAUD_CARD,
   if the amount is not available will return DECLINED_INSUFFECIENT_FUND,
   if the account is blocked will return DECLINED_STOLEN_CARD,
   if a transaction can't be saved will return INTERNAL_SERVER_ERROR, else returns APPROVED.
4. It will update the database with the new balance.
*/
EN_transState_t recieveTransactionData(ST_transaction_t* transData)
{
	if (isValidAccount(&cardData, &accountRefrence) == ACCOUNT_NOT_FOUND) 
		transState = FRAUD_CARD;
	if (isBlockedAccount(&accountRefrence) == BLOCKED_ACCOUNT) 
		transState = DECLINED_STOLEN_CARD;
	if (isAmountAvailable(&terminalData, &accountRefrence) == LOW_BALANCE) 
		transState = DECLINED_INSUFFECIENT_FUND;
	if (saveTransaction(transData) == SAVING_FAILED) 
		transState = INTERNAL_SERVER_ERROR;

	return transState;
}

/*
1. This function will store all transaction data in the transactions database.
2. It gives a sequence number to a transaction,
   this number is incremented once a transaction is processed into the server,
   check the last sequence number in the server to give the new transaction a new sequence number.
3. It saves any type of transaction,
   APPROVED, DECLINED_INSUFFECIENT_FUND, DECLINED_STOLEN_CARD, FRAUD_CARD, INTERNAL_SERVER_ERROR.
4. It will list all saved transactions using the listSavedTransactions function.
5. Assuming that the connection between the terminal and server is always connected, then it will return SERVER_OK.
   .............................
   Transaction Sequence Number:
   Transaction Date:
   Transaction Amount:
   Transaction State:
   Max. Transaction Amount:
   Cardholder Name:
   PAN:
   Card Expiration Date:
   .............................
*/
EN_serverError_t saveTransaction(ST_transaction_t* transData)
{   
	transData->cardHolderData = cardData;
	transData->terminalData = terminalData;
	transData->transState = transState;

	if (transState == APPROVED) transactionState = "(APPROVED)";
	if (transState == DECLINED_INSUFFECIENT_FUND) transactionState = "(DECLINED INSUFFECIENT FUND)";
	if (transState == DECLINED_STOLEN_CARD) transactionState = "(DECLINED STOLEN CARD)";
	if (transState == FRAUD_CARD) transactionState = "(FRAUD CARD)";
	if (transState == INTERNAL_SERVER_ERROR) transactionState = "(INTERNAL SERVER ERROR)";

	printf("\n#################################################\n\n");
	printf("Transaction Date: %s\n", transData->terminalData.transactionDate);
	printf("Transaction Amount: %0.2f\n", transData->terminalData.transAmount);
	printf("Transation State: %d %s\n", transData->transState, transactionState);
	printf("Max. Transaction Amount: %0.2f\n", transData->terminalData.maxTransAmount);
	printf("Card Expiration Date: %s\n", transData->cardHolderData.cardExpirationDate);
	printf("Card Holder's Name: %s\n", transData->cardHolderData.cardHolderName);
	printf("Card PAN: %s\n", transData->cardHolderData.primaryAccountNumber);
	printf("#################################################\n\n");

	/* Transaction DataBase Saving */
	FILE *TSEQN = fopen("TransSN.txt", "r");
	FILE *TSEQN_ = fopen("TransSN_.txt", "w");
	if (TSEQN == NULL)
	{
		printf("        [ Transaction Database Read Failed. ]\n");
		return INTERNAL_SERVER_ERROR;
	}
	if (TSEQN_ == NULL)
	{
		printf("        [ Transaction Database Save Failed. ]\n");
		return SAVING_FAILED;
	}
	fscanf(TSEQN, "%d", &TSN);
	TSN += 1;
	fprintf(TSEQN_, "%d\n", TSN);
	fclose(TSEQN);
	fclose(TSEQN_);
	remove("TransSN.txt");
	rename("TransSN_.txt", "TransSN.txt");

	transactionsDB[TSN].cardHolderData = transData->cardHolderData;
	transactionsDB[TSN].terminalData = transData->terminalData;
	transactionsDB[TSN].transState = transState;

	listSavedTransactions();
	
	return SERVER_OK;
}

void listSavedTransactions(void)
{
	FILE *TDB = fopen("TransactionsDB.txt", "a");
	if (TDB == NULL) printf("        [ Transaction Database Access Failed. ]\n");

	fprintf(TDB, "Transaction Sequence Number: %d\n", TSN);
	fprintf(TDB, "Transaction Date: %s\n", transactionsDB[TSN].terminalData.transactionDate);
	fprintf(TDB, "Transaction Amount: %0.2f\n", transactionsDB[TSN].terminalData.transAmount);
	fprintf(TDB, "Transaction State: %d %s\n", transactionsDB[TSN].transState, transactionState);
	fprintf(TDB, "Max. Transaction Amount: %0.2f\n", transactionsDB[TSN].terminalData.maxTransAmount);
	fprintf(TDB, "Card Expiration Date: %s\n", transactionsDB[TSN].cardHolderData.cardExpirationDate);
	fprintf(TDB, "Card Holder's Name: %s\n", transactionsDB[TSN].cardHolderData.cardHolderName);
	fprintf(TDB, "Card PAN: %s\n", transactionsDB[TSN].cardHolderData.primaryAccountNumber);
	fprintf(TDB, "#################################################\n\n");
	
	fclose(TDB);
	printf("[ Transactions Database has been Successfully Saved. ]\n");
}


void updateAccountsDB(void)
{
	FILE* ADB = fopen("AccountsDB.txt", "w");
	if (ADB == NULL)  printf("[ Accounts Database Read Failed. ]\n");
	uint8_t *STATE;
	for (uint8_t i = 0; i < 10; i++)
	{
		if (ID == i)
		{
			fprintf(ADB, "{%0.2f, %s, \"%s\"}\n", 
				accountRefrence.balance -= terminalData.transAmount, 
				"RUNINNG", 
				accountRefrence.primaryAccountNumber);
		}
		if (accountsDB[i].state == RUNNING) STATE = "RUNNING";
		else STATE = "BLOCKED";
		fprintf(ADB, "{%0.2f, %s, \"%s\"}\n", 
			accountsDB[i].balance, 
			STATE,
			accountsDB[i].primaryAccountNumber);
	}
	fclose(ADB);
	printf("[ Accounts Database has been Successfully Updated. ]\n");
}
