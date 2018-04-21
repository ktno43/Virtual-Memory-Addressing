/*****************************************
Kyle Nguyen
COMP 222
Spring 2018
Prof. Robert McIlhenny
T/R 3:30 P.M - 4:45 P.M

Assignment #4: Virtual Memory
Source.c

Version 6.0

The program works as expected and follows
program specifications.  It translates
the virtual address to a physical
address which involves a custom-sized
fully associative page table.

The program also has the added feature
of using the policy LFU.  By using this
policy it correctly rearranges the table
accordingly based on the frequency.  In
the case of tie breakers, LRU will be
used.
*****************************************/
#include <stdio.h>
#include <stdlib.h>

// Function Declarations
void mapVirtualAddr(void);
void displayMenu(void);
void setParamaters(void);
void freeStuff(void);

// Global Variables
int g_nMainMemSize = 0; // Main memory size
int g_nPageSize = 0; // Page size
int g_nPolicy = 0; // Replacemnt policy 
int g_nNumEntries = 0; // Number of lines in page table

// Structure for page table node 
struct pageTableNode
{ // A page table node consists of a virtual page and a page frame
	int m_VirtualPage; // Variable for virtual page
	int m_PageFrame; // Variable for page frame
	int m_Hit; // Variable for # of hits
} *pageTable = NULL;

typedef struct pageTableNode entry; // Define variable entry for page tablenode

int main()
{
	displayMenu(); // Function call to Display Menu of Options

	printf("\n\nProgram Ending . . .\n");  //Program Ending
	printf("Press ENTER to close --->  ");
	getchar();
	getchar();

	return EXIT_SUCCESS;
}

// Display menu of options
void displayMenu()
{
	char cInput = '\0'; // Variable for input data

	while (cInput != 'c')
	{
		printf("Virtual memory to Main memory mapping:\n");
		printf("--------------------------------------\n");
		printf("a) Enter parameters\n");
		printf("b) Map virtual address\n");
		printf("c) Quit\n\n");

		printf("Enter selection:  ");
		scanf(" %c", &cInput); // Get the character pressed from user

		switch (cInput)
		{
			case 'a':
				setParamaters(); // Function call to assign values to parameters
				break;

			case 'b':
				mapVirtualAddr(); // Function call to map virtual address
				break;

			case 'c':
				freeStuff(); // Function call to free dynamically allocated memory and exit the program
				return;

			default:
				printf("Invalid input. \n"); // Invalid character read
		}
		printf("\n\n");
		getchar();
	}
	return;
}

// Function to set parameters
void setParamaters()
{
	int bInputFlag = 1;

	// Input value for size of main memory
	printf("\nEnter main memory size (words): ");
	scanf("%d", &g_nMainMemSize);

	// Input value for page size
	printf("Enter page size (words/page):  ");
	scanf("%d", &g_nPageSize);

	// Input value for replacement policy
	while (bInputFlag == 1)
	{
		// What policy is it? LRU, FIFO, LFU
		printf("Enter replacement policy (0=LRU, 1=FIFO, 2=LFU):  ");
		scanf("%d", &g_nPolicy);

		// If the policy is valid, continue
		if (g_nPolicy == 1 || g_nPolicy == 0 || g_nPolicy == 2)
		{
			bInputFlag = 0;
		}

		else // Else invalid number
			printf("Invalid input. \n\n");
	}

	// Check to see if page table is free
	freeStuff();

	// Build page table
	g_nNumEntries = g_nMainMemSize / g_nPageSize; // Function to get number of pages 
	pageTable = (entry *)malloc(g_nNumEntries * sizeof(entry)); // Allocate the cache according to the number of entries

	// Fill page table
	int i = 0;
	for (i = 0; i < g_nNumEntries; i++) { // For ever entry in the page table, the virtual page will be -1
		pageTable[i].m_VirtualPage = -1; // Assign to -1 B.Y.S.S (because you said so)
		pageTable[i].m_Hit = 0; // Assign hit counter to 0 for LFU
	}
	return;
}

// Function to map the virtual address
void mapVirtualAddr()
{
	int nVirtualAddr = 0; // Variable for virtual address

	printf("\nEnter virtual memory address to access:  ");
	scanf("%d", &nVirtualAddr);

	int nVirtualPage = nVirtualAddr / g_nPageSize; // From the slides... VP = VA / PS
	int nOffset = nVirtualAddr % g_nPageSize; // From the slides... Offset = VA MOD PS
	int nPhysicalAddr = 0; // Variable for physical address
	int nPageFrame = -1; // Variable for page frame

	/* Check for end of table, unallocated entry, or matched entry in table
	while none of three cases, keep looping */
	int i = 0;
	while ((i < g_nNumEntries) && (pageTable[i].m_VirtualPage != -1) && (pageTable[i].m_VirtualPage != nVirtualPage))
	{
		i++;
	}

	/* In case of end of table, replace either LRU or FIFO entry (top entry in page table), print message */
	if (i >= g_nNumEntries)
	{
		nPageFrame = pageTable[0].m_PageFrame;  // Previous reference to PF

		int j = 0;
		for (j = 0; j < g_nNumEntries - 1; j++)
		{
			pageTable[j] = pageTable[j + 1]; // Shift all entries up
		}
		pageTable[g_nNumEntries - 1].m_VirtualPage = nVirtualPage; // Last entry will be the new VP
		pageTable[g_nNumEntries - 1].m_PageFrame = nPageFrame; // Last entry's PF will be the previous PF

		// If it's LFU rearrange the table
		if (g_nPolicy == 2)
		{
			pageTable[g_nNumEntries - 1].m_Hit = 0; // Set the last entry's counter

			int k = 1;
			while (pageTable[g_nNumEntries - 1].m_Hit < pageTable[g_nNumEntries - 1 - k].m_Hit) // While the current's hit count is < the previous hit count (table full at this point)
			{
				k++; // Get the number of iterations until you get to the right spot
			}

			int nUpMoves = k - 1; // Number of moves is off by 1 
			int l = g_nNumEntries - 1; // Variable for starting position for page table

			int m = 0;
			for (m = 0; m < nUpMoves; m++, l--) // For the number of up moves needed, move the necessary entries up
			{
				pageTable[l] = pageTable[l - 1]; // Move each entry to the one above
			}

			pageTable[(g_nNumEntries - 1) - nUpMoves].m_VirtualPage = nVirtualPage; // Update the VP at the new location
			pageTable[(g_nNumEntries - 1) - nUpMoves].m_PageFrame = nPageFrame; // Update the PF at the new location
			pageTable[(g_nNumEntries - 1) - nUpMoves].m_Hit = 0; // Hit count will be 0 since it just entered to page table
		}
		printf("\nPage fault!\n");
	}

	/* In case of unallocated entry, set entry according to virtal page and page frame, print message */
	else if (pageTable[i].m_VirtualPage == -1)
	{
		pageTable[i].m_VirtualPage = nVirtualPage; // Set current index to new VP
		pageTable[i].m_PageFrame = i; // Set current PF to index because index = PF
		int nPrevHit = pageTable[i].m_Hit; // Previous reference to number of hits

		if (g_nPolicy == 2) // If LFU policy
		{
			int j = 1;
			while (!((i - j) <= -1) && pageTable[i].m_Hit < pageTable[i - j].m_Hit) // While the index is a valid index and the current hit count is less than the previous
			{
				j++; // Get the number of iterations until you get to the right spot
			}

			int nUpMoves = j - 1; // Number of moves is off by 1
			int k = i; // Variable for starting position for page table

			int l = 0;
			for (l = 0; l < nUpMoves; l++, k--) // For the number of up moves needed, move the necessary entries up
			{
				pageTable[k] = pageTable[k - 1]; // Move each entry to the one above
			}
			pageTable[i - nUpMoves].m_VirtualPage = nVirtualPage; // Update the VP at the new location
			pageTable[i - nUpMoves].m_PageFrame = i; // Update the PF at the new location
			pageTable[i - nUpMoves].m_Hit = nPrevHit; // Update the hit count to the previous hit
		}
		printf("\nPage fault!\n");
	}

	/* In case of hit in page table, calculate physical address and print message, update page table if LRU policy */
	else // (pageTable[i].m_VirtualPage == nVirtualPage)
	{
		nPageFrame = pageTable[i].m_PageFrame; // Get PF from current index
		nPhysicalAddr = nPageFrame * g_nPageSize + nOffset; // From slides... PA = Base + Offset = PF * PS + (VA MOD PS)

		// If LRU policy
		if (g_nPolicy == 0)
		{
			int j;
			for (j = i; j < g_nNumEntries - 1; j++) // For the number of entries starting where the hit was found
			{
				if (pageTable[j + 1].m_VirtualPage == -1) // Check to see if there is a next entry or not
				{
					break;
				}
				pageTable[j] = pageTable[j + 1]; // Shift everything down
			}
			pageTable[j].m_VirtualPage = nVirtualPage; // Update the VP of the entry at position j, will be the bottom of the page table to the new VP calculated
			pageTable[j].m_PageFrame = nPageFrame; // Update the PF of the entry at position j, will be the bottom of the page table to the previously saved PF earlier
		}

		// Else if LFU policy
		else if (g_nPolicy == 2)
		{
			pageTable[i].m_Hit += 1; // Increase the hit count
			int nCurrentHit = pageTable[i].m_Hit; // Reference to the current hit count

			int k = i;
			while ((k < g_nNumEntries - 1) && (pageTable[i].m_Hit >= pageTable[k + 1].m_Hit)) // While a valid entry and the current hit count is greater than the next, swap them
			{
				if (pageTable[k + 1].m_VirtualPage == -1) // Check to see if there is an entry or not
				{
					break;
				}
				pageTable[k] = pageTable[k + 1]; // Current reference will be the next reference
				pageTable[k + 1].m_VirtualPage = nVirtualPage; // Next reference's VP will be the current VP
				pageTable[k + 1].m_PageFrame = nPageFrame; // Next reference's PF will be the current PF
				pageTable[k + 1].m_Hit = nCurrentHit; // Next reference's hit count will be the current hit count

				k++; // Keep going until the end of the table or an empty entry
			}
		}
		printf("\nVirtual address %d maps to physical address %d\n", nVirtualAddr, nPhysicalAddr);
	}

	// Display not so beautiful ASCII art
	printf("-----------------\n");
	printf("| VP\t| PF\t|\n");
	printf("-----------------\n");

	int j = 0;
	for (j = 0; j < g_nNumEntries; j++)
	{
		if (pageTable[j].m_VirtualPage != -1)
		{
			printf("| %d\t| %d\t|\n", pageTable[j].m_VirtualPage, pageTable[j].m_PageFrame);
			printf("-----------------\n");
		}
	}
	return;
}

void freeStuff() // Deallocate memory
{
	if (pageTable != NULL) // Is the page table null?
	{
		free(pageTable); // So free the page table now
		pageTable = NULL;
	}
	return;
}
