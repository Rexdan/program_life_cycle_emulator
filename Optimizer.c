/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Fall 2016                              *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

void collapse(Instruction *input)
{	
	Instruction *toFree = NULL;
	int count = 0;
	int freeCount = 0;
	int op;

	while(input)
	{	
		op = input->opcode;

		if(input->critical != 'y')
		{
			toFree = input;
			switch(op)
			{
				case LOAD:
					WARNING("REMOVING LOAD INSTRUCTION\n");
					break;
				case LOADI:
					WARNING("REMOVING LOADI INSTRUCTION\n");
					break;
				case STORE:
					WARNING("REMOVING STORE INSTRUCTION\n");
					break;
				case ADD:
					WARNING("REMOVING ADD INSTRUCTION\n");
					break;
				case SUB:
					WARNING("REMOVING SUB INSTRUCTION\n");
					break;
				case MUL:
					WARNING("REMOVING MUL INSTRUCTION\n");
					break;
				case READ:
					WARNING("REMOVING READ INSTRUCTION\n");
					break;
				case WRITE:
					WARNING("REMOVING WRITE INSTRUCTION\n");
					break;
			}
			input->next->prev = input->prev;
			input->prev->next = input->next;
			input = input->next;
			free(toFree);
			freeCount++;
			continue;
		}
		if(input->critical == 'y') count++;
		
		input = input->next;
	}
	WARNING("NUMBER OF TIMES POINTER WAS FREED: %d\n", freeCount);
	DEBUG("TOTAL NUMBER OF INSTRUCTIONS AFTER OPTIMIZATION: %d\n", count);		
}

int main()
{
	Instruction *head;
	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}
	/* YOUR CODE GOES HERE */
	Instruction *curr;
	curr = LastInstruction(head);
	int critReg = -1;
	int i;

	/*
		NOTE: field1 will always be a register for opcodes SUB, MUL, ADD, LOAD, and LOADI
	*/

	int numOfRegs = 0;
	int *regs;
	int total = 0;
	int writes = 0;
	int *writeVars;

	/*First we loop to see how many registers we will potentially be working with and the number of WRITE instructions.*/
	while(curr != NULL)
	{
		int op = curr->opcode;
		switch(op)
		{
			case LOAD:
				numOfRegs++;
				break;
			case LOADI:
				numOfRegs++;
				break;
			case STORE:
				numOfRegs++;
				break;
			case ADD:
				numOfRegs++;;
				break;
			case SUB:
				numOfRegs++;
				break;
			case MUL:
				numOfRegs++;
				break;
			case WRITE:
				writes++;
				break;
		}
		if(curr->opcode == WRITE) writes++;
		total++;
		curr = curr->prev;
	}
	DEBUG("TOTAL NUMBER OF INSTRUCTIONS: %d\n", total);
	if(writes > 0)
	{
		writeVars = (int*)malloc(sizeof(int)*5);
	}
	else
	{
		WARNING("NO WRITE INSTRUCTIONS FOUND!\n");
		exit(EXIT_FAILURE);
	}

	for(i = 0; i < 5; i++) writeVars[i] = -1;

	if(numOfRegs > 0)
	{
		regs = (int*)malloc(sizeof(int)*numOfRegs);
	}
	else
	{
		WARNING("NO REGISTERS WERE FOUND!\n");
		exit(EXIT_FAILURE);
	}

	for(i = 0; i < numOfRegs; i++) regs[i] = 0;
	
	curr = LastInstruction(head);
	
	while(curr != NULL)
	{
		if(curr->opcode == READ || curr->opcode == WRITE || curr->opcode == STORE)
		{
			if(curr->opcode == READ) curr->critical = 'y';
			if(curr->opcode == WRITE)
			{
				/*The register whose instructions should be marked as critical.*/
				int spot;
				for(i = 0; i < 5; i++)
				{
					if(writeVars[i] == curr->field1) break;
					if(writeVars[i] < 0) spot = i;
				}
				if(i == 4)
				{
					WARNING("REUSING A VARIABLE IN WRITE INSTRUCTION\n.");
				}
				else
				{
					writeVars[spot] = curr->field1;
				}
				//WARNING("CURRENT WRITE VARIABLE: %c\n", writeVars[spot]);
				curr->critical = 'y';
				/*So our encounter with STORE can check to see if its register's value is equal to the WRITE's value*/
			}
			if(curr->opcode == STORE)
			{
				/*This is our reference to the critical register. Secure for WRITE instruction.*/
				critReg = curr->field2;
				//WARNING("IN STORE. STORE'S REGISTER AND VARIABLE: %d %c\n", curr->field2, curr->field1);
				for(i = 0; i < numOfRegs; i++)
				{
					if(critReg == regs[i])
					{
						curr->critical = 'y';
						break;
					}
				}
				/*Case for when variable is critical.*/
				if(curr->critical != 'y')
				{
					//WARNING("STORE'S REGISTER NOT FOUND TO BE CRITICAL. CHECKING VARIABLE...\n");
					for(i = 0; i < 5;i++)
					{
						if(curr->field1 == writeVars[i])
						{
							//WARNING("FOUND CRITICAL VARIABLE. STORE INSTRUCTION'S CRITICAL REGISTER: %d\n", regs[regC]);
							curr->critical = 'y';
							break;
						}
					}
					/*Have to add the register to the list now.*/
					if(curr->critical == 'y')
					{
						for(i = 0; i < numOfRegs; i++)
						{
							if(regs[i] == 0)
							{
								regs[i] = critReg;
								break;
							}
						}
					}
				}
			}
		}
		else
		{
			int op = curr->opcode;
			/*Looking for instructions other than READ, WRITE, and STORE.*/
			if(op == ADD || op == SUB || op == MUL)
			{	
				/*if(curr->opcode == ADD)
					WARNING("CURRENT OPCODE IS: ADD\n");
				if(curr->opcode == SUB)
					WARNING("CURRENT OPCODE IS: SUB\n");
				if(curr->opcode == MUL)
					WARNING("CURRENT OPCODE IS: MUL\n");*/
				int reg1a = -1, reg2a = -1, reg3a = -1;
				reg1a = curr->field1;
				reg2a = curr->field2;
				reg3a = curr->field3;
					
				int add1 = -1, add2 = -1, add3 = -1;

				for(i = 0; i < numOfRegs; i++)
				{
					if(regs[i] == reg1a || regs[i] == reg2a || regs[i] == reg3a)
					{
						if(regs[i] == reg1a)
						{
							//WARNING("REGISTER1a EXISTS: %d\n", reg1a);
							add1 = 1;
						}
						if(regs[i] == reg2a)
						{
							//WARNING("REGISTER1b EXISTS: %d\n", reg2a);
							add2 = 1;
						}
						if(regs[i] == reg3a)
						{
							//WARNING("REGISTER1c EXISTS: %d\n", reg3a);
							add3 = 1;
						}
					}
				}
				if(add1 == 1 || add2 == 1 || add3 == 1)
				{
					curr->critical = 'y';
					
					if(add1 == -1)
					{
						//WARNING("ADDED REGISTER: %d\n", reg1a);
						for(i = 0; i < numOfRegs; i++)
						{
							if(regs[i] == 0)
							{
								regs[i] = reg1a;
								break;
							}
						}
					}
					if(add2 == -1)
					{
						//WARNING("ADDED REGISTER: %d\n", reg2a);
						for(i = 0; i < numOfRegs; i++)
						{
							if(regs[i] == 0)
							{
								regs[i] = reg2a;
								break;
							}
						}
					}

					if(add3 == -1)
					{
						//WARNING("ADDED REGISTER: %d\n", reg3a);
						for(i = 0; i < numOfRegs; i++)
						{
							if(regs[i] == 0)
							{
								regs[i] = reg3a;
								break;
							}
						}
					}
				}
			}
			else if(op == LOAD || op == LOADI)
			{
				/*Case when LOAD or LOADI has a critical register.*/
				for(i = 0; i < numOfRegs; i++)
				{
					if(regs[i] == curr->field1)
					{
						/*if(op == LOAD)
						{
							WARNING("LOAD HAS A CRITICAL REGISTER\n");
							WARNING("LOAD's REGISTER AND VARIABLE: %d %c \n", curr->field1, curr->field2);
						}
						if(op == LOADI)
						{
							WARNING("LOADI HAS A CRITICAL REGISTER\n");
							WARNING("LOADI's REGISTER AND CONSTANT: %d %d \n", curr->field1, curr->field2);
						}*/
						curr->critical = 'y';
						critReg = curr->field1;
						break;
					}
				}
				/*Case where critical register was not found, but may have critical variable (LOAD only).*/
				if(op == LOAD && curr->critical != 'y')
				{
					for(i = 0; i < 5; i++)
					{
						if(writeVars[i] == critReg)
						{
							curr->critical = 'y';
							//WARNING("LOAD HAS CRITICAL VARIABLE.\n");
							break;
						}
					}
				}
				/*Case for when we are on a critical LOAD instruction and need to add its variable to our list.*/
				if(op == LOAD && curr->critical == 'y')
				{
					//WARNING("LOAD VARIABLE: %c\n", curr->field2);
					int spot;
					int found = -1;

					for(i = 0; i < 5; i++)
					{
						if(writeVars[i] == curr->field2) found = 1;
						if(writeVars[i] < 0) spot = i;
					}
					if(found == -1)
						writeVars[spot] = curr->field2;
				}
			}
		}
		curr = curr->prev;
		total++;
	}
	
	curr = head;
	collapse(curr);
	head = curr;
	free(writeVars);
	free(regs);	
	if (head) {
		PrintInstructionList(stdout, head);
		DestroyInstructionList(head);
	}
	return EXIT_SUCCESS;
}

