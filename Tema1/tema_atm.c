/*GRIGORE IRINA-DOMNICA*/
/*315CB*/

#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include "tlista.h"
#include "functii.c"

int main()
{
	char str[199];
	unsigned long long int card_number = 0, sum = 0, card_number_source =0,
		 card_number_dest = 0;
	char pin[7];
	char new_pin[7];
	char expiry_date[6];
	char cvv[5];
	int poz = -1, ok = 0, nr_carduri = 0;
	TLC L = NULL, Laux, aux, aux1, aux3, aux4;
	TLSC aux2;
	int nr_max_carduri, i = 0, poz_max = -1;
	char operatie[10];
	FILE *input_file = fopen("input.in", "r");
	if (input_file == NULL)
		fprintf(stderr, "ERROR: Can't open file %s\n", "input.in");
	FILE *output_file = fopen("output.out", "w");
	fscanf(input_file, "%d", &nr_max_carduri);
	/*---adaug n = nr_max_carduri celule in lista L*/
	for (i = 0; i < nr_max_carduri; i++)
		InserareInceput(&L, NULL);
	while (fscanf(input_file, "%s", str) != EOF) {
		strcpy(operatie, str);
		if (strcmp(operatie, "add_card") == 0) {
			Laux = NULL;
			fscanf(input_file, "%llu", &card_number);
			fscanf(input_file, "%s", pin);
			fscanf(input_file, "%s", expiry_date);
			fscanf(input_file, "%s", cvv);
			int k = -1, poz1;
			poz1 = CalcPoz(card_number, nr_max_carduri);
			for (aux4 = L; aux4 != NULL; aux4 = aux4->urm_lc)
			{
				k++;
				if (k == poz1) 
				{
					ok = VerificareCarduriIdentice(aux4->info_lc,
								       card_number);
					break;
				}
			}
			/*---daca cardul adaugat nu exista deja in lista atunci maresc 
				numarul de carduri existente---*/
			if (ok == 1)
				nr_carduri++;
			
			/*---varific daca s-a depasit numarul maxim de carduri si fac 
				realocare listei---*/
			if (nr_carduri > nr_max_carduri && ok == 1) 
			{
				nr_max_carduri = 2 * nr_max_carduri;
				/*---aloc spatiu pentru jumatate din noul numar maxim de 
					carduri in lista veche---*/
				for (i = 0; i < nr_max_carduri / 2; i++)
					InserareInceput(&L, NULL);
				/*---aloc spatiu pentru noua lista---*/
				for (i = 0; i < nr_max_carduri; i++)
					InserareInceput(&Laux, NULL);
				/*---parcurg lista veche---*/
				for (aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
					for (aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc) 
					{
						/*---recalculez noua pozitie---*/
						poz = CalcPoz(aux2->info_lsc->card_number,nr_max_carduri);
						i = 0;
						for (aux3 = Laux; aux3 != NULL; aux3 = aux3->urm_lc) 
						{
							if (i == poz) 
							{
								/*---copiez informatia din vechea lista in alta lista---*/
								AdaugareCard2(&aux3->info_lc, aux2->info_lsc->card_number,
								    aux2->info_lsc->pin,aux2->info_lsc->expiry_date,
								    aux2->info_lsc->cvv,aux2->info_lsc->balance,
								    aux2->info_lsc->status,output_file,aux2->info_lsc->history,
								    aux2->info_lsc->wrong);
								if (poz > poz_max)
									poz_max = poz;
							}
							i++;
						}

					}
				L = Laux;
			}
			/*---calculez pozitia pe care trebuie introdus cardul---*/
			poz = CalcPoz(card_number, nr_max_carduri);
			i = 0;
			/*---parcurg lista, iar atunci cand gasesc pozitia adaug cardul---*/
			for (aux = L; aux != NULL; aux = aux->urm_lc) 
			{
				if (i == poz) 
				{
					AdaugareCard(&aux->info_lc, card_number, pin, expiry_date, cvv, 0,"NEW", output_file,0);
					if (poz > poz_max)
						poz_max = poz;
				}
				i++;
			}
		}

		if (strcmp(operatie, "delete_card") == 0) 
		{
			/*---atunci cand un card este sters din lista scad numarul cardurilor adaugate---*/
			nr_carduri--;
			fscanf(input_file, "%llu", &card_number);
			for (aux = L; aux != NULL; aux = aux->urm_lc) 
			{
				EliminaCard(&aux->info_lc, card_number);
			}
		}
		if (strcmp(operatie, "reverse_transaction") == 0) 
		{
			fscanf(input_file, "%llu", &card_number_source);
			fscanf(input_file, "%llu", &card_number_dest);
			fscanf(input_file, "%llu", &sum);
			AnulareTranzactie(L, card_number_source,
					  card_number_dest, sum, output_file);

		}
		if (strcmp(operatie, "unblock_card") == 0) 
		{
			fscanf(input_file, "%llu", &card_number);
			DeblocareCard(L, card_number);
		}
		if (strcmp(operatie, "show") == 0) 
		{
			if (fscanf(input_file, "%llu", &card_number) == 1) 
			{
				AfisareCard(L, card_number, output_file);
			} else
			{
				AfisareCarduri(L, output_file, poz_max);
			}
		}
		if (strcmp(operatie, "insert_card") == 0) 
		{
			fscanf(input_file, "%llu", &card_number);
			fscanf(input_file, "%s", pin);
			InserareCard(L, card_number, pin, output_file);
			for (aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
				for (aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc)
					if(aux2->info_lsc->card_number == card_number)
					{
						if(strcmp(aux2->info_lsc->pin,pin) != 0)
							/*---daca pinul a fost introdus gresit, numarul de incercari creste---*/
							aux2->info_lsc->wrong++;
					}
			for (aux1 = L; aux1 != NULL; aux1 = aux1->urm_lc)
				for (aux2 = aux1->info_lc; aux2 != NULL; aux2 = aux2->urm_lsc)
					if(aux2->info_lsc->card_number == card_number)
					{
						/*---verific daca numarul de incercari de introducere a PIN-ului gresit---*/
						if(aux2->info_lsc->wrong==3)
						{	
							fprintf(output_file,"The card is blocked. Please contact the administrator.\n");
							strcpy(aux2->info_lsc->status,"LOCKED");
						}
						else

							if(aux2->info_lsc->wrong>3)
								fprintf(output_file,"The card is blocked. Please contact the administrator.\n");
					}
		}
		if (strcmp(operatie, "recharge") == 0) 
		{
			fscanf(input_file, "%llu", &card_number);
			fscanf(input_file, "%llu", &sum);
			DepunereNumerar(L, card_number, sum, output_file);
		}
		if (strcmp(operatie, "cash_withdrawal") == 0) 
		{
			fscanf(input_file, "%llu", &card_number);
			fscanf(input_file, "%llu", &sum);
			RetragereNumerar(L, card_number, sum, output_file);
		}
		if (strcmp(operatie, "balance_inquiry") == 0) 
		{
			fscanf(input_file, "%llu", &card_number);
			VizualizareSoldCurent(L, card_number, output_file);

		}
		if (strcmp(operatie, "transfer_funds") == 0) 
		{
			fscanf(input_file, "%llu", &card_number_source);
			fscanf(input_file, "%llu", &card_number_dest);
			fscanf(input_file, "%llu", &sum);
			Tranzactie(L, card_number_source, card_number_dest, sum,
				   output_file);
		}
		if (strcmp(operatie, "pin_change") == 0) 
		{
			fscanf(input_file, "%llu", &card_number);
			fscanf(input_file, "%s", new_pin);
			SchimbarePin(L, card_number, new_pin, output_file);
		}
		if (strcmp(operatie, "cancel") == 0) 
		{
			fscanf(input_file, "%llu", &card_number);
			RetragereCard(L, card_number);
		}
	}
	DistrugeLista(&L);
	fclose(input_file);
	fclose(output_file);
	return 0;
}
