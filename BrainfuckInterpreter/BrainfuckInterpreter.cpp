// BrainfuckInterpreter.cpp
// Brainfuck interpreter v1.0A
// Code (C) Dylan Knutson 2011
// Desc: 
//		Contains 3 classes, a Turing Tape complete data object, a Stack object, and the BF interpreter object. Via this, multiple programs can run concurrently. 
//		call stepProgram() to run the next instruction in the program
//		BF interperter object will print chars to the screen
//		Does not support get char command, although very few BF programs currently support it
//
//	Stack object: Generic stack (dynamicaly allocated). Can theoreticaly hold up to 2^32 items
//	Turing tape object: Turing compleate, dynamically allcoated tape object. Can theoretically hold up to 2^32 bytes of data before index exaustion 
//

#include "stdafx.h"
#include <string>
#include <stdlib.h>

/* Begin Stack class */
class Stack
{
	//TODO: Implement Stack
	int* m_ipStack; //The main stack data heap
	int m_iStackAllocSize; //The currently allocated size of the stack/sizeof(int) (number of elems it can currently hold)
	int m_iStackTopLoc; //The location of the top of the stack (how many elems are on the stack at the time). Is the next free space in the stack. 
public:
	Stack() :
	m_ipStack(NULL),
	m_iStackAllocSize(0),
	m_iStackTopLoc(0)
	{
		m_ipStack = (int*)malloc(sizeof(int)); //Create a data heap for the stack, and init the first data item to 0
		*m_ipStack = 0;

		return; //Just init our values
	}
	
	int push(int); //Push a value onto the stack, return number of elems on the stack
	int pop(void); //Pop a value off the stack and return it
	int top(void); //The value on the top of the stack
};
int	Stack::push(int data)
{
	if(m_iStackAllocSize < m_iStackTopLoc+1) // Check if our allocated size is less than 1 plus the current stack size
	{
		m_iStackAllocSize++; //Increment the stack allocation size if not to accomidate the new data
		int* stackRealloc = (int*)realloc(m_ipStack, m_iStackAllocSize * sizeof(int)); //And reallocate the stack heap
		if(stackRealloc == NULL)
			return NULL; //There was an error reallocating the stack. Return null. 
		m_ipStack = stackRealloc; //Succesfull reallocation, set m_ipStack to new data location
	}
	*(m_ipStack+m_iStackTopLoc) = data; //Assign the passed data to the top of the stack
	m_iStackTopLoc++; //And inrement the top of the stack
	return m_iStackTopLoc;
}
int	Stack::pop(void)
{
	if(m_iStackTopLoc < 1) //make sure there is something on the stack
		return NULL; //If not, then return null

	m_iStackTopLoc--; //Decrement the top of the stack
	int data = *(m_ipStack + m_iStackTopLoc); //Return the data at that location
	return data;
}
int Stack::top(void)
{
	if(!m_iStackTopLoc) // Nothing is on the stack
		return 0;

	return *(m_ipStack + m_iStackTopLoc-1); //Return the top of the stack -1 (due to 1 based indexes)
}
/* End Stack class */

/* Begin TuringTape class */
class TuringTape
{
	/* Begin private member vars */
	unsigned char * m_pcTape; //Pointer to the data heap that holds the turing tape data
	unsigned int m_iTapeAllocSize; //Current allocated size of the turing tape / sizeof(unsigned char)
	unsigned int m_iHeadLocation; //Location of the head on the turing tape
	/* End private member vars */
	/* Begin private member functions */
	int incTapeSize(); //Increment the tape size, init the value of the new place to 0, return 0 on success, !0 on error
	/* End private member functions */
public:
	/* Begin public member functions */
	TuringTape() :
	m_pcTape(NULL),
	m_iTapeAllocSize(0),
	m_iHeadLocation(0)
	{
		incTapeSize(); //Allocate and init the first space on the tape
		return; //Nothing to do here but init vars
	}

	int incPointer(void); //Increment the "head" of the tape, return current position
	int decPointer(void); //Decrement the "head" of the tape, return current position
	int incData(void); //Increment the data at the current head location, return data at head location
	int decData(void); //Decrement the data at the current head location, return data at head location
	unsigned int getHeadLoc(void); //Get the location of the head on the tape
	unsigned char getDataAtHeadLoc(unsigned int); //Get the data located at header location 'x'
	unsigned char getData(); //Get the data at the current header location
	void printTapeContents(void);
	/* End private member functions */
};
int	TuringTape::incTapeSize()
{
	m_iTapeAllocSize++; //Incremnt the allocation size
	unsigned char* pReallocTape = (unsigned char*)realloc(m_pcTape, sizeof(unsigned char*)*m_iTapeAllocSize); //Attempt to reallocate the size of the data heap

	if(pReallocTape == NULL)
		return 1; //Error reallocating data
	m_pcTape = pReallocTape; //Assign the reallocated memory location
	//Init the newly allocated data to 0
	*(m_pcTape+m_iTapeAllocSize-1) = 0; //Subtract 1 due to 1 based index

	return 0; // Success, return 0
}
int	TuringTape::incPointer(void)
{
	m_iHeadLocation++;
	if(m_iHeadLocation+1 > m_iTapeAllocSize) // If the new head location (plus 1, because the head at 0 is not greater than an allocation size of 0)
		if(incTapeSize() != 0) //Then increase tape allocation size
			return NULL; //There was an error reallocating the tape
	
	return m_iHeadLocation;
}
int	TuringTape::decPointer(void)
{
	if(m_iHeadLocation < 1)
		return NULL; //We cant go below a 0 index on the tape head. Implement it later. 

	m_iHeadLocation--;
	return m_iHeadLocation;
}
int	TuringTape::incData(void)
{
	*(m_pcTape+m_iHeadLocation) = *(m_pcTape+m_iHeadLocation)+1; //Increment the data in the memory location
	return *(m_pcTape+m_iHeadLocation);
}
int TuringTape::decData(void)
{
	*(m_pcTape+m_iHeadLocation) = *(m_pcTape+m_iHeadLocation)-1; //Dec data at mem location
	return *(m_pcTape+m_iHeadLocation);
}
unsigned int TuringTape::getHeadLoc(void)
{
	return m_iHeadLocation;
}
unsigned char TuringTape::getData()
{
	return *(m_pcTape+m_iHeadLocation);
}
unsigned char TuringTape::getDataAtHeadLoc(unsigned int index)
{
	if(index > m_iTapeAllocSize || index < 0)
		return 0;

	return *(m_pcTape + m_iHeadLocation);
}
void TuringTape::printTapeContents(void)
{
	for(unsigned int i=0; i < m_iTapeAllocSize; i++)
	{
		printf("Tape at index: '%u' is '0x%X'\n", i, *(m_pcTape+i));
	}
}
/* End TuringTape class */

/* Begin InterpretBrainfuck class */
class InterpretBrainfuck
{
	/* Begin member variables */
	unsigned char* m_pcBFProg; //Hold the parsed and formatted instructions for our program
	unsigned int m_iBFProgLen; //The total number of instructions in the program (maximum iBFISP value - 1)
	/*
	BFISP: 0 1 2 3
	Prog:  + + + \0

	length of program: 3
	BFISP at nul step: 3
	When BFISP+1 is > len, we are at end of program
	*/
	unsigned int m_iBFISP; //Program instruction pointer
	Stack *m_SBFStack; //Stack for the BF program
	TuringTape *m_TBFTape; //Tape to hold program data in

	/* End member variables */
	/* Begin private functions */

	/* End private functions */
public:
	/* Begin public functions */
	//Constructor. Init variables to 0 and null, construct objects
	InterpretBrainfuck() : 
		m_pcBFProg(NULL),
		m_iBFISP(0),
		m_iBFProgLen(0),
		m_SBFStack(new Stack()),
		m_TBFTape(new TuringTape())
	{
		return;
	}

	int loadProgram(const char*);  //Load a program and parse it. Return 0 on success
	unsigned char* getProgram(void); //Get the program in a null terminated string
	int endOfProgram(); //Return 0 if we are running, 1 if we are at the end of the program
	double stepProgram(void); //Return: [7, 6, 5, 4, 3, 2, 1, 0] <- Bytes
							// Byte 0-3: unsigned int of the current instruction pointer
							// Byte 4: unsigned char* of the data at the current pointer location
							// unsigned char* = (stepProgram() >> sizeof(int)*4);
							// unsigned int = stepProgram();
	void printTape(void)
	{
		m_TBFTape->printTapeContents();
	}
	/* End public functions */
};
int	InterpretBrainfuck::loadProgram(const char* rawProgram) //We give the program a null terminated pointer array
{
	m_iBFProgLen = 0; //Reset program length counter

	//Count the [ and ] instruction to make sure we have an equal number of both
	int insJmpIfZeroCount = 0;
	int insJmpIfNotZeroCount = 0;

	char * pProg;
	pProg = (char*)rawProgram; //Set our temporary pProg to the location of rawProgram

	//First find the total number of valid instructions in the program (ignore comments and other chars)
	while(*pProg != '\0')
	{
		//Is the current char a valid command?
		if(	*pProg == '>' ||
			*pProg == '<' ||
			*pProg == '+' ||
			*pProg == '-' ||
			*pProg == '.' ||
			*pProg == ',')
		{
			m_iBFProgLen++; //We ran across a valid command, increase instruction count
		}
		if(	*pProg == '[') //We have a jump if zero command
		{
			m_iBFProgLen++;
			insJmpIfZeroCount++; //Increase numbr of [ commands
		}
		if(	*pProg == ']') //We have a jump if zero command
		{
			m_iBFProgLen++;
			insJmpIfNotZeroCount++; //Increase count of ] commands
		}

		pProg++; //Increment to next byte in string
	}

	if(insJmpIfZeroCount != insJmpIfNotZeroCount)
		return -1; //Mismatched number of '[' and ']' commands

	//And reiterate thru commands again to parse valid commands into single command string
	pProg = (char*) rawProgram;

	m_pcBFProg = (unsigned char*) malloc(m_iBFProgLen); //Allocate a new array the size of the number of valid instructions, plus 1 for a null terminator

	if(m_pcBFProg == NULL) //We were not able to allocate the new data block correctly
		return -2; //Not enough memory/error allocating memory for program
		
	unsigned char* pBFProg = m_pcBFProg; //Back up the current address of the program

	//Now append the valid commands onto the internal BFProg variable, skipping non program commands
	while(*pProg != '\0')
	{
		if(	*pProg == ']' ||
			*pProg == '[' ||
			*pProg == '>' ||
			*pProg == '<' ||
			*pProg == '+' ||
			*pProg == '-' ||
			*pProg == '.' ||
			*pProg == ',')
		{
			*m_pcBFProg = *pProg; //Append the command onto m_pcBFProg
			m_pcBFProg++; //Only go to the next instruction if we have a valid instruction
		}
		//Increment thru the entire input program
		pProg++;
	}
	*(m_pcBFProg) = '\0'; //Null terminate the instruction list
	m_pcBFProg = pBFProg; //Restore the pointer to the original location
	return 0;
}
unsigned char* InterpretBrainfuck::getProgram(void)
{
	return m_pcBFProg;
}
int	InterpretBrainfuck::endOfProgram()
{
	//If the instruction pointer (plus 1, 1 based index) is greater than the total program length, then return 1 (prog end)
	//Else, return 0 (not end)
	return !((m_iBFISP+1) > m_iBFProgLen);
}
double InterpretBrainfuck::stepProgram(void)
{
	//Quick state refrence for debugging
	
	char cInstruction = *(m_pcBFProg+m_iBFISP);
	unsigned int BFISP = m_iBFISP;
	unsigned char tapeData = m_TBFTape->getData();
	unsigned int tapeHead = m_TBFTape->getHeadLoc();
	unsigned int StackTop = m_SBFStack->top();
	
	//printf("Running command: %c, BFISP is: %u\n", *(m_pcBFProg+m_iBFISP), m_iBFISP);
	switch(*(m_pcBFProg+m_iBFISP))
	{
	case '+':
		m_TBFTape->incData();
		break;

	case '-':
		m_TBFTape->decData();
		break;

	case '>':
		m_TBFTape->incPointer();
		break;

	case '<':
		m_TBFTape->decPointer();
		break;

	case '[':
		{
		int i=0;
		int o=0;
		if(m_TBFTape->getData() == 0) //If the data at the head is zero, skip to the corrosponding ']' command
		{
			//Find the next matching ']' instruction
			while(1) //Keep looping till one is found or we hit program end
			{
				i++; //Increment the instruction offset

				//If we are a the end of the program
				if(m_iBFISP+i+1 > m_iBFProgLen)
					return 0; //We got to the end of the program and have not found our closing loop. There was an error, return 0

				if(*(m_pcBFProg+m_iBFISP+i) == '[') //Another loop opens here, so increment the loop count
					o++;
				else if(*(m_pcBFProg+m_iBFISP+i) == ']') //A close loop instruction is found, and o is zero.
				{
					if(o!=0) //We are not at the corrosponding ']' instruction
						o--; //Decrement the open instruction counter
					else //o is 0, we are at the correct ']' instruction
					{
						m_iBFISP += i-1; //Jump forward this offset, minus 1 (1 based index)
						break;
					}
				}
			}
		}
		else
		{
			m_SBFStack->push(m_iBFISP); //Push the current instruction pointer on the stack, so we can jump to it if needed in a jump if not zero instruction
		}
		break;
		}
	case ']':
		{
		if(m_TBFTape->getData() != 0)
		{
			m_iBFISP = m_SBFStack->top(); //Set the ISP to the top of the stack (eg, the matching open loop address)
		}
		else
		{
			//data was 0
			m_SBFStack->pop(); //Pop the value and continue on
		}
		break;
		}
	case '.':
		{
			printf("%c", m_TBFTape->getData());
			break;
		}
	default:
		printf("Unknown command encountered: %c\n", *(m_pcBFProg+m_iBFISP)); 
		break;
	}
	m_iBFISP++; //Increment the instruction pointer
	return 0; //Implement later...
}
/* End InterpretBrainfuck class */

int _tmain(int argc, _TCHAR* argv[])
{
	InterpretBrainfuck* BFProg1 = new InterpretBrainfuck();  

	//Some sample programs
	char helloWorld[] = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>."; //Hello world
	char simple1[] = "++[-]"; 
	char printBFCode[] = ">>++++++++++[<++++++<++++++>>-]<<>++>>>>>++++++++++[<++++<++++<++++<++++>>>>-]<<<<+++>+++++>++++++>++++>>>++++++++++[<+++++++++<+++++++++>>-]<<+>+++>++++++++++<<<<<<<.>..........>>>>.<<<<<<.>>..........<.>>.>>>>.<<<<<<<.>>>>>>>>.<<<<<..>.>>>>.<<<<<<................>>.>>>>.<<<<<.................>.>>>>.<<<<<<........>>.>>>>.<<<<<<.....>>.>>>>.<<<<<........>.>>>>.<<<<<<...............>>.>>>>.<<<<<..................>.>>>>.<<<<<<........>>.>>>>.<<.<<<.>>>>.>[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]";
	char bfCodeOutput[] = ">++++++++++[<++++++++++>-]<--.++++++++++++++++.-----------------.++++++++.+++++.--------.+++++++++++++++.------------------.++++++++.[-]";
	char happyBdayLauren[] = "[-]>[-]<>++++++++[<++++++++>-]<++++++++.>+++++[<+++++>-]<.>++++[<++++>-]<-..>+++[<+++>-]<.>+++++++++[<--------->-]<--------.>++++++++[<++++++++>-]<++.>+++[<+++>-]<--.>+++[<+++>-]<.++.>+++[<--->-]<---.----.---.>+++++[<+++++>-]<-.>+++++++++[<--------->-]<++++.>+++[<--->-]<---.>+++++++[<+++++++>-]<-----.>+++++[<+++++>-]<----.>++++[<++++>-]<++++.---.>++++[<---->-]<+++.>+++[<+++>-]<.>+++++++++[<--------->-]<++++.";
	//Load program here -------------------v
	if(int ret = BFProg1->loadProgram(happyBdayLauren))
	{
		printf("There was an error loading the program: %d\n", ret);
		system("pause"); //Yes, it's ugly, but system("pause") is just temporary
		return 1;
	}
	printf("Loaded the program\n");

	printf("\n-------------START OF PROGRAM OUTPUT-------------\n");
	while(BFProg1->endOfProgram() != 0)
	{
		BFProg1->stepProgram();
		//BFProg1->printTape();
	}
	printf("\n--------------END OF PROGRAM OUTPUT--------------\n");
	system("pause");
	return 0;
}