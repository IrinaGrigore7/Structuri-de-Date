/*GRIGORE IRINA-DOMNICA*/
/*315CB*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct histry
{
	char info_history[999];
	struct histry* urm_history;
}History, *THistory, **AHistory;

typedef struct card
{
	unsigned long long int card_number;
	char pin[7];
	char expiry_date[6];
	char cvv[5];
	unsigned long long int balance;
	char status[20];
	int wrong;
 	History *history;
}Card, *TCard, **ACard;

typedef struct lsc
{
	Card* info_lsc;
	struct lsc *urm_lsc;
}LSC, *TLSC, **ALSC;

typedef struct lc
{
	LSC* info_lc;
	struct lc *urm_lc;
}LC, *TLC, **ALC;
