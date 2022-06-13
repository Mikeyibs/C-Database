/********************************************
* 265inventory.c -- This program functions as an interactive database utility, which maintains
* a simple database from a text file called inv.dat that is located within the CWD.
* Author -- Michael Ibrahim
* Date   -- June 2nd, 2022
*
* gcc version 9.4.0 (Ubuntu 9.4.0-1ubuntu1~20.04.1) 
* 5.4.0-105-generic x86_64 GNU/Linux
*******************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct itemNode itemNode ;

struct itemNode {
	char id[16] ;
	unsigned short qty ;
	char desc[31] ;
	itemNode *next ;
} ;



/* printData - Prints out the data nicely in a table format.
 * input     - itemNode *data (Pointer to the linked list data)
 * output    - N/A */
void printData(itemNode *data)
{
	itemNode *temp;
	char ID[16] = "ID";
	char DESC[31] = "DESC";
	char QTY[5] = "QTY";

	printf("%6s %30s %10s\n", DESC, QTY, ID);
	printf("------------------------------  -------  -----------\n");

	for( temp=data; temp != NULL; temp = temp->next)
	{
		printf("%-30.30s %7d %12.16s\n", temp->desc, temp->qty, temp->id);
	}
}



/* listRev - Reverses the provided linked list. 
 * input   - itemNode *list (Pointer to the linked list data)
 * output  - list (Reversed list)
 */
itemNode *listRev(itemNode *list)
{
	itemNode *p = list;
	itemNode *prev = NULL;
	itemNode *nxt = list;

	if( list == NULL )
	{
		return list;
	}

	while( p != NULL )
	{
		nxt = nxt->next;

		p->next = prev;
		prev = p;
		p = nxt;
	}

	list = prev;
	return list;
}



/* push  - Adds a node to the linked list with the provided data
 * input - itemNode *list (Pointe to the linked list data), id (pointer to string ID),
 * 		  qty (quantity), descr (pointer to string description)
 * output - list (If the list is NULL it will return the given list), temp (New node in list)
 */
itemNode *push(itemNode *list, char *id, int qty, char *descr)
{
	itemNode *temp;
	
	temp = malloc(sizeof(itemNode));
	if(temp == NULL)
	{
		return list;
	}

	strcpy(temp->id, id);
	temp->qty = qty;
	strcpy(temp->desc, descr);
	temp->next = list;
	return temp;
}


/* findItem - Searches the linked list for the correct item based on the ID
 * input    - itemNode *list (Pointer to the linked list), *id (pointer to the string ID)
 * output   - item (The node in the list if it is found), NULL (If the node is not found it will return NULL)
 */
itemNode *findItem(itemNode *list, char *id)
{
	itemNode *item;

	for( item = list; item != NULL; item = item-> next )
	{
		if(strcmp(item->id, id) == 0)
		{
			return item;
		} 
	}
	return NULL;
}


/* add - Simply adds the given quantity to the given nodes quantity;
 * input - itemNode *list (Pointer to the node in the list), qty (The amount to be added)
 * output - N/A
 * Side Effects - Updates the quantity within the given node
 */
void add(itemNode *list, int qty)
{
	list->qty = list->qty + qty;
}


/* checkQty - This function checks to see if the quantity given is less then the amount in the linked list.
 * input    - itemNode *list (Pointer to the node in the list), qty (The amount to be removed)
 * output   - 0 (If the quantity in the list is less then that to be removed), 1 (If the quantity in the list is greater then the amount being removed.
 */
int checkQty(itemNode *list, int qty)
{
	int testQty;

	testQty = list->qty;
	if(testQty < qty)
	{
		return 0;
	} else
	{
		return 1;
	}
}


/* writeOut - This function creates a temporary file and writes out the data present within each node of the list to said temporary file then renames the
 *				  file to inv.dat to match our existing database.
 * input    - itemNode *list (Pointer to the head of the list)
 * output   - N/A
 * Side effects - Creates a new file in the current working directory then overwrites the existing inv.dat file by renaming itself. 
 */
void writeOut(itemNode *list)
{
	char tempFileName[30] = "./myTempFile-XXXXXX";
	char newFileName[30] = "./inv.dat";
	int fileDes = -1;
	int reName;
	itemNode *temp;
	FILE* oFile;

	fileDes = mkstemp(tempFileName);
	oFile = fdopen(fileDes, "w");

	for( temp=list; temp != NULL; temp=temp->next )
	{
		fprintf(oFile, "%s:%d:%s\n", temp->id, temp->qty, temp->desc);
	}

	reName = rename(tempFileName, newFileName);
	fclose(oFile);
}


/* removeItem - This function removes quantity from a given node in the linked list.
 * input      - itemNode* list (Pointer to a specific node within the list), qty (The amount to be removed)
 * output     - N/A
 * Side effects - Updates the quantity within the given node. 
 */
void removeItem(itemNode *list, int qty)
{
	list->qty = list->qty - qty;
}


/* main - Main control function for our program. 
 * 	    Handles data collection from the intiial file and user interaction.
 */
int main()
{
	char *resp = NULL;
	FILE *fp;
	char *buff = NULL;
	size_t len = 0;
	char ID[16];
	char cmd[7];
   int qty;
   int cmds;

	char tempID[16];
	char tempDesc[31];
	int tempQty;
	
	fp = fopen("./inv.dat", "r");
	if (fp == NULL)
	{
		printf("Unable to find Data File");
		return 0;
	}
	itemNode *data = NULL;
	
	// While loop to collect the data from the file inv.dat.
	while(getline(&buff, &len, fp) != -1)
	{
		itemNode *temp = (itemNode*) malloc(sizeof(itemNode));
		buff[ strlen(buff)-1 ] = '\0';
		sscanf(buff, "%16[^:]:%d:%31[^\n]", tempID, &tempQty, tempDesc);	
		
		data = push(data, tempID, tempQty, tempDesc);
		free( temp );
	}

	// Call to listRev to properly organize our newly allocated data.
	data = listRev(data);

	// Main user input loop.
	while(1)
	{
		printf("\n%% ");
		getline(&resp, &len, stdin);
		sscanf(resp, "%s", cmd);
		
		if((strncmp(resp, "add", 3) == 0) && ((strlen(cmd) == 3) || (strlen(cmd) == 4)))
		{

			cmds = sscanf(resp, "%*s %16[^:]:%d", ID, &qty);
			if (cmds == 2)
			{
				itemNode *item = findItem(data, ID);

				if (item != NULL)
				{
					add(item, qty);
					printf("ACK");
				} 

				else
				{
					printf("NAK Unknown Item");
				}
			} 

			else 
			{
				printf("NAK Incorrect syntax.");
			}

		} 

		else if ((strncmp(resp, "remove", 6) == 0) && ((strlen(cmd) == 6) || (strlen(cmd) == 7)))
		{

			cmds = sscanf(resp, "%*s %16[^:]:%d", ID, &qty);
			if (cmds == 2)
			{
				itemNode *rmvItem = findItem(data, ID);

				if (rmvItem != NULL && checkQty(rmvItem, qty) != 0)
				{
					removeItem(rmvItem, qty);
					printf("ACK");
				} 

				else 
				{
					printf("NAK Insufficient Quantity");
				}
			} 

			else
			{
				printf("NAK Incorrect Syntax");
			}
		} 

		else if ((strncmp(resp, "print", 5) == 0) && ((strlen(cmd) == 5) || (strlen(cmd) == 6)))
		{
			printData(data);
			printf("ACK");
		} 
		
		else if ((strncmp(resp, "quit", 4) == 0) && ((strlen(cmd) == 4) || (strlen(cmd) == 5)))
		{
			writeOut(data);
			printf("ACK\n");
			fclose(fp);
			free(resp);
			free(buff);
			free(data);
			break;
		} 

		else
		{
			printf("Unrecognized command");
		}

	}

	return 0;
}