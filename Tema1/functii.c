/*GRIGORE IRINA-DOMNICA*/
/*315CB*/

#include <ctype.h>
#include <string.h>

/*---aloc memorie pentru o celula din lista de carduri---*/
TLC AlocareLista(TLSC l)
{
	TLC aux;
	aux = (TLC)calloc(1, sizeof(LC));
	if(!aux)
		return NULL;
	aux->urm_lc = NULL;
	aux->info_lc = l;
	return aux;
}

/*---aloc memorie pentru un card---*/
TLSC AlocareCard(unsigned long long int card_number, char pin[7], 
	char expiry_date[6], char cvv[5], unsigned long long int balance,
	char status[20],THistory history,int wrong)
{
	TLSC aux;
	aux = (TLSC)calloc(1,sizeof(LSC));
	if(!aux)
		return NULL;
	aux->urm_lsc = NULL;
	aux->info_lsc = (TCard)calloc(1,sizeof(Card));
	if(!aux->info_lsc)
	{
		free(aux);
		return NULL;
	}
	aux->info_lsc->card_number = card_number;
	strcpy(aux->info_lsc->pin, pin);
	strcpy(aux->info_lsc->expiry_date, expiry_date);
	strcpy(aux->info_lsc->cvv, cvv);
	aux->info_lsc->balance = balance;
	strcpy(aux->info_lsc->status, status);
	aux->info_lsc->history=history;
	aux->info_lsc->wrong=wrong;
	return aux;
}

/*---aloc memorie pentru istoricul tranzatiilor---*/
THistory AlocareIstoric(char info_history[999])
{
	THistory aux;
	aux = (THistory)calloc(1, sizeof(History));
	if(!aux)
		return NULL;
	aux->urm_history = NULL;
	strcpy(aux->info_history, info_history);
	return aux;
}

int InserareInceput(TLC *L, TLSC l)
{
	TLC aux;
	aux = AlocareLista(l);
	if(!aux)
		return 0;
	aux->urm_lc = *L;
	*L = aux;
	return 1;
}

int InserareInceputIstoric(THistory *history, char info_history[999])
{
	THistory aux;
	aux = AlocareIstoric(info_history);
	if(!aux)
		return 0;
	aux->urm_history = *history;
	*history = aux;
	return 1;
}

/*---functia returneaza 0 in cazul in care cardurile sunt identice si 
	1 daca sunt diferite---*/
int VerificareCarduriIdentice(TLSC l, unsigned long long int card_number)
{
	while(l != NULL)
	{
		if(l->info_lsc->card_number == card_number)
			return 0;
		l = l->urm_lsc;
	}
	return 1;
}

/*---functie folosita pentru a copia cardurile din lista veche in cea noua---*/
int AdaugareCard2(TLSC *l, unsigned long long int card_number,char pin[7], 
		char expiry_date[6], char cvv[5], unsigned long long int balance,
	char status[20], FILE *output_file,THistory history,int wrong) 
{
	TLSC aux;
	aux = AlocareCard(card_number, pin, expiry_date, cvv, balance, status,history,wrong);
	if(!aux)
		return 0;
	aux->urm_lsc = *l;
	*l = aux;
	return 1;
}

/*---1-adaugare reusita; 0-adaugare esuata---*/
int AdaugareCard(TLSC *l, unsigned long long int card_number,char pin[7], 
		char expiry_date[6], char cvv[5], unsigned long long int balance,
	char status[20], FILE *output_file,int wrong) 
{
	TLSC aux2 = *l;
	TLSC aux;
	int ok = 0;
	if(*l == NULL)
	{
		aux = AlocareCard(card_number, pin, expiry_date, cvv, balance, status,NULL,0);
		if(!aux)
			return 0;
		aux->urm_lsc = *l;
		*l = aux;
		return 1;
	}
	else
	{
		ok = VerificareCarduriIdentice(aux2,card_number);
		
		if(ok == 1)
		{
			aux = AlocareCard(card_number, pin, expiry_date, cvv, balance, status,NULL,0);
			if(!aux)
				return 0;
			aux->urm_lsc = *l;
			*l = aux;
			return 1;
		}
		else
			fprintf(output_file, "The card already exists\n");
	}
	return 0;
}

int CalcPoz(unsigned long long int card_number, int nr_max_carduri)
{
	int suma = 0;
	int poz; 
	while(card_number != 0)
	{
		suma = suma + card_number % 10;
		card_number = card_number / 10;
	}
	poz = suma % nr_max_carduri;
	return poz;
}

/*---elimin cardul din baza de date si refac legaturile---*/
int EliminaCard(TLSC *l, unsigned long long int card_number)
{
	TLSC ant,p;
	for(p = *l, ant = NULL; p != NULL; ant = p, p = p->urm_lsc)
		if(p->info_lsc->card_number == card_number)
			break;
	if(p == NULL)
		return 0;
	if(ant == NULL)
		*l = p->urm_lsc;
	else
		ant->urm_lsc = p->urm_lsc;
	free(p);
	return 1;
}

/* 1-inserare reusita; 0-inserare esuata*/
int InserareCard(TLC L, unsigned long long int card_number, char pin[7], 
													FILE *output_file)
{
	TLC aux1;
	TLSC aux2;
	char history[999];
	for(aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
		for(aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc)
			{
				if(aux2->info_lsc->card_number == card_number)
				{
					if(strcmp(aux2->info_lsc->pin,pin) != 0)
					{
						if(strcmp(aux2->info_lsc->status,"LOCKED")!=0)
							fprintf(output_file, "Invalid PIN\n");
						sprintf(history,"(FAIL, insert_card %016llu %s)", 
									card_number, pin);
						InserareInceputIstoric(&aux2->info_lsc->history,history);
						return 0;
					}
					else
						if(strcmp(aux2->info_lsc->status,"NEW") == 0)
						{
							fprintf(output_file, "You must change your PIN.\n");
							sprintf(history,"(SUCCESS, insert_card %016llu %s)", 
										card_number, pin);
							InserareInceputIstoric(&aux2->info_lsc->history,history);
							aux2->info_lsc->wrong=0;/*---resetez numarul de incercari 
								gresite de introducere a pinului---*/
							return 1;
						}
						else
							if(strcmp(aux2->info_lsc->status,"ACTIVE") == 0)
							{
								sprintf(history,"(SUCCESS, insert_card %016llu %s)", 
												card_number, pin);
								InserareInceputIstoric(&aux2->info_lsc->history, history);
								aux2->info_lsc->wrong=0;/*---resetez numarul de incercari 
								gresite de introducere a pinului---*/
								return 1;
							}
							else
							{
								sprintf(history,"(FAIL, insert_card %016llu %s)", 
												card_number, pin);
								InserareInceputIstoric(&aux2->info_lsc->history,history);
								return 0;
							}
				}
			}
	return 0;
}

void RetragereCard(TLC L, unsigned long long int card_number)
{
	TLC aux1;
	TLSC aux2;
	char history[999];
	for(aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
		for(aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc)
		{
			if(aux2->info_lsc->card_number == card_number)
			{
				sprintf(history,"(SUCCESS, cancel %016llu)", card_number);
				InserareInceputIstoric(&aux2->info_lsc->history,history);
			}
		}
}

void DeblocareCard(TLC L, unsigned long long int card_number)
{
	TLC aux1;
	TLSC aux2;
	for(aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
		for(aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc)
		{
			if(aux2->info_lsc->card_number == card_number && 
				(strcmp(aux2->info_lsc->status,"LOCKED") == 0 || strcmp(aux2->info_lsc->status,"NEW") == 0))
			{
				strcpy(aux2->info_lsc->status,"ACTIVE");
				aux2->info_lsc->wrong=0;
			}
		}
}

/*---verifica daca pinul nou introdus este alcatuit doar din 4 cifre 
	(1-pin valid;0-pin invalid) ---*/
int VerificarePinValid(char new_pin[7])
{
	int i;
	int contor = 0;
	for(i = 0; i < strlen(new_pin); i++)
	{
		if(new_pin[i] >= '0' && new_pin[i] <= '9')
		{
			contor++;
		}
	}
	if(contor == 4)
		return 1;
	else
		return 0;
}

/*---schimb pinul doar daca pinul introdus este valid---*/
void SchimbarePin(TLC L, unsigned long long int card_number, char new_pin[7], 
													FILE *output_file)
{
	TLC aux1;
	TLSC aux2;
	char history[999];
	int ok =0;
	ok = VerificarePinValid(new_pin);
	for(aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
			for(aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc)
				if(ok == 1 && aux2->info_lsc->card_number == card_number)
				{
					strcpy(aux2->info_lsc->pin,new_pin); 
					strcpy(aux2->info_lsc->status,"ACTIVE");
					sprintf(history,"(SUCCESS, pin_change %016llu %s)", 
									card_number, new_pin);
					InserareInceputIstoric(&aux2->info_lsc->history, history);
				}
				else
					if(aux2->info_lsc->card_number == card_number)
					{
						fprintf(output_file, "Invalid PIN\n");
						sprintf(history,"(FAIL, pin_change %016llu %s)", 
										card_number, new_pin);
						InserareInceputIstoric(&aux2->info_lsc->history, history);
					}
}

void VizualizareSoldCurent(TLC L, unsigned long long int card_number, FILE *output_file)
{
	TLC aux1;
	TLSC aux2;
	char history[999];
	for(aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
		for(aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc)
			if(aux2->info_lsc->card_number == card_number)
			{
				fprintf(output_file, "%llu\n", aux2->info_lsc->balance);
				sprintf(history,"(SUCCESS, balance_inquiry %016llu)", card_number);
				InserareInceputIstoric(&aux2->info_lsc->history, history);
			} 
}

void DepunereNumerar(TLC L, unsigned long long int card_number, 
						unsigned long long int sum, FILE *output_file)
{
	TLC aux1;
	TLSC aux2;
	char history[999];
	for(aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
		for(aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc)
			if(aux2->info_lsc->card_number == card_number)
			{
				if(sum % 10 == 0)
				{
					aux2->info_lsc->balance =  aux2->info_lsc->balance + sum;
					fprintf(output_file, "%llu\n", aux2->info_lsc->balance);
					sprintf(history,"(SUCCESS, recharge %016llu %llu)", card_number, sum);
					InserareInceputIstoric(&aux2->info_lsc->history, history);
				}
				else
				{
					fprintf(output_file, "The added amount must be multiple of 10\n");
					sprintf(history,"(FAIL, recharge %016llu %llu)", card_number, sum);
					InserareInceputIstoric(&aux2->info_lsc->history, history);
				}
			}
	
}
void RetragereNumerar(TLC L, unsigned long long int card_number, 
						unsigned long long int sum, FILE *output_file)
{
	TLC aux1;
	TLSC aux2;
	char history[999];
	for(aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
		for(aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc)
			if(aux2->info_lsc->card_number == card_number)
			{
				if(sum % 10 == 0)
				{
					if(aux2->info_lsc->balance >= sum)
					{
						/*---recalculeaza soldul ---*/
						aux2->info_lsc->balance =  aux2->info_lsc->balance - sum;
						fprintf(output_file, "%llu\n", aux2->info_lsc->balance);
						sprintf(history,"(SUCCESS, cash_withdrawal %016llu %llu)", 
											card_number, sum);
						InserareInceputIstoric(&aux2->info_lsc->history, history);
					}
					else
					{
						fprintf(output_file, "Insufficient funds\n");
						sprintf(history,"(FAIL, cash_withdrawal %016llu %llu)", 
											card_number, sum);
						InserareInceputIstoric(&aux2->info_lsc->history, history); 
					}
				}
				else
				{
					fprintf(output_file, "The requested amount must be multiple of 10\n");
					sprintf(history,"(FAIL, cash_withdrawal %016llu %llu)", card_number, sum);
					InserareInceputIstoric(&aux2->info_lsc->history, history); 
				}
			}
}

void Tranzactie(TLC L, unsigned long long int card_number_source, 
	unsigned long long int card_number_dest, unsigned long long int sum, FILE *output_file)
{
	TLC aux1;
	TLSC aux2;
	int ok = 0; 
	char history[999];
	for(aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
		for(aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc)
		{
			if(aux2->info_lsc->card_number == card_number_source)
			{
				if(sum % 10 == 0)
				{
					if(aux2->info_lsc->balance >= sum)
					{
						/*---recalculez soldul---*/
						aux2->info_lsc->balance =  aux2->info_lsc->balance - sum;
						fprintf(output_file, "%llu\n", aux2->info_lsc->balance);
						sprintf(history,"(SUCCESS, transfer_funds %016llu %016llu %llu)", 
							card_number_source, card_number_dest, sum);
						InserareInceputIstoric(&aux2->info_lsc->history, history);
					}
					else
					{
						/*---daca ok devine 1 atunci nu se va adauga nimic in istoricul cardului destinatie---*/
						ok = 1;
						fprintf(output_file, "Insufficient funds\n");
						sprintf(history,"(FAIL, transfer_funds %016llu %016llu %llu)",
							 card_number_source, card_number_dest, sum);
						InserareInceputIstoric(&aux2->info_lsc->history, history); 
					}
				}
				else
				{
					fprintf(output_file, "The transferred amount must be multiple of 10\n");
					sprintf(history,"(FAIL, transfer_funds %016llu %016llu %llu)", 
							card_number_source, card_number_dest, sum);
					InserareInceputIstoric(&aux2->info_lsc->history, history); 
				}
			}
			if(aux2->info_lsc->card_number == card_number_dest)
			{
				if(sum % 10 == 0 && ok == 0)
				{
					/*---recalculez soldul---*/
					aux2->info_lsc->balance =  aux2->info_lsc->balance + sum;
					sprintf(history,"(SUCCESS, transfer_funds %016llu %016llu %llu)", 
							card_number_source, card_number_dest, sum);
					InserareInceputIstoric(&aux2->info_lsc->history, history);
				}
			}
		}
}

/*---imi sterge o anumita operatie din istoric---*/
int EliminaIstoric(THistory *h, char history[999])
{
	THistory ant,p;
	for(p = *h, ant = NULL; p != NULL; ant = p, p = p->urm_history)
		if(strcmp(p->info_history,history) == 0)
			break;
	if(p == NULL)
		return 0;
	if(ant == NULL)
		*h = p->urm_history;
	else
		ant->urm_history = p->urm_history;
	free(p);
	return 1;
}

void AnulareTranzactie(TLC L, unsigned long long int card_number_source, 
	unsigned long long int card_number_dest, unsigned long long int sum, FILE *output_file)
{
	TLC aux1;
	TLSC aux2;
	int ok = 0;
	char history[999];
	for(aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
		for(aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc)
		{
			if(aux2->info_lsc->card_number == card_number_dest)
			{
				if(aux2->info_lsc->balance >= sum)
				{
					/*---daca ok devine 1 atunci operatia va aparea in istoricul cardului sursa */
					ok = 1;
					aux2->info_lsc->balance =  aux2->info_lsc->balance - sum;
					sprintf(history,"(SUCCESS, transfer_funds %016llu %016llu %llu)", 
							card_number_source, card_number_dest, sum);
					EliminaIstoric(&aux2->info_lsc->history,history);
				}
				else
				{
					fprintf(output_file, "The transaction cannot be reversed\n");
				}
			}
		}
		for(aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
			for(aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc)
			{
				if(aux2->info_lsc->card_number == card_number_source && ok == 1)
				{
					aux2->info_lsc->balance =  aux2->info_lsc->balance + sum;
					sprintf(history,"(SUCCESS, reverse_transaction %016llu %016llu %llu)", 
						card_number_source, card_number_dest, sum);
					InserareInceputIstoric(&aux2->info_lsc->history, history);
				}
			}
}

void AfisareCarduri(TLC L, FILE *output_file,int poz_max)
{
	TLC aux1;
	TLSC aux2;
	THistory aux3;
	int i = 0;
	for(aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
	{
		if(aux1->info_lc != NULL )
		{
			fprintf(output_file,"pos%d: [\n", i);
			for(aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc)
			{
				fprintf(output_file,"(card number: %016llu, ", aux2->info_lsc->card_number);
				fprintf(output_file,"PIN: %s, ", aux2->info_lsc->pin);
				fprintf(output_file,"expiry date: %s, ",  aux2->info_lsc->expiry_date);
				fprintf(output_file,"CVV: %s, ", aux2->info_lsc->cvv);
				fprintf(output_file,"balance: %llu, ",  aux2->info_lsc->balance);
				fprintf(output_file,"status: %s, ", aux2->info_lsc->status);
				fprintf(output_file, "history: [");
				if(aux2->info_lsc->history!=NULL)
					for(aux3= aux2->info_lsc->history; aux3!=NULL;aux3=aux3->urm_history)
					{
						if(aux3->info_history != NULL && aux3->urm_history != NULL)
						{
							fprintf(output_file, "%s, ", aux3->info_history);
						}
						else
						{
							fprintf(output_file, "%s])\n", aux3->info_history);
						}
					}
				else
					fprintf(output_file, "])\n");
			}
			fprintf(output_file,"]\n");
		}
		else
		{
			if(i <= poz_max)/*---afiseaza campuri goale pana la ultima pozitie ocupata---*/
				fprintf(output_file,"pos%d: []\n", i);
		}
		i++;
	}
}

void AfisareCard(TLC L, unsigned long long int card_number, FILE *output_file)
{
	TLC aux1;
	TLSC aux2;
	THistory aux3;
	for(aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
	{
		for(aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc)
		{
			if(aux2->info_lsc->card_number == card_number)
			{
			fprintf(output_file,"(card number: %016llu, ", aux2->info_lsc->card_number);
			fprintf(output_file,"PIN: %s, ", aux2->info_lsc->pin);
			fprintf(output_file,"expiry date: %s, ",  aux2->info_lsc->expiry_date);
			fprintf(output_file,"CVV: %s, ",  aux2->info_lsc->cvv);
			fprintf(output_file,"balance: %llu, ",  aux2->info_lsc->balance);
			fprintf(output_file,"status: %s, ", aux2->info_lsc->status);
			fprintf(output_file, "history: [");
			if(aux2->info_lsc->history!=NULL)
				for(aux3= aux2->info_lsc->history; aux3!=NULL;aux3=aux3->urm_history)
				{
					if(aux3->info_history != NULL && aux3->urm_history != NULL)
					{
						fprintf(output_file, "%s, ", aux3->info_history);
					}
					else
					{
						fprintf(output_file, "%s])\n", aux3->info_history);
					}
				}
			else
				fprintf(output_file, "])\n");
			}
		}
	}
}

/*---eliberez memoria pentru istoric---*/
void EliberareIstoric(THistory *h)
{
	THistory aux;
	while(*h)
	{
		aux = *h;
		if(aux != NULL)
			*h = aux->urm_history;
		free(aux);
	}
}

/*---dealoc memoria listei LSC---*/
void EliberareElem(TLSC *l)
{
	TLSC aux;
	while(*l)
	{
		aux = *l;
		if(aux != NULL)
			*l = aux->urm_lsc;
		EliberareIstoric(&aux->info_lsc->history);
		free(aux->info_lsc);
		free(aux);
	}
}

/*---dealoc memoria listei LC---*/
void DistrugeLista(TLC *L)
{
	TLC aux;
	while(*L)
	{
		aux = *L;
		if(aux != NULL)
			*L = aux->urm_lc;
		EliberareElem(&aux->info_lc);
		free(aux);
	}
}



