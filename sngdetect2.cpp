/*sngdetect
  Version 1.0
  (Simple and Glider with escaped-glider Detection)
  March 2000
  Gabriel Nivasch
*/

#include <stdio.h>
#include <stdlib.h>
#include "sngdetect.h"

void print(int, int, int, int);
void run(void);
void createMask(void);
void printMask(void);
int checkReappearance(void);
void printinitialboard(void);

//Global variables:
int runboard[3][BSIZE][BSIZE], initialboard[BSIZE][BSIZE],
	gen, pop, initialpop, numEscGliders;
//We keep the previous generation also to in order to be able to test
//if the pattern became period 2.
//The initial board is used to print out the pattern.

int runLeft[3], runRight[3], runTop[3], runBottom[3],
	runMinFwDiag, runMaxFwDiag, runMinBkDiag, runMaxBkDiag;

int prv, cur, nxt; //Have values from 0 to 2, to index runboard and the run limits.

//Variables for keeping statistics:
int numReapp=0, numStable=0, numDied=0, numP2=0, numEdge=0,
	numLong=0, numThru=0;

extern int stboard[HSIZE][VSIZE], foundcount;
extern int patLeft, patRight, patTop, patBottom;

int stmask[HSIZE+2][VSIZE+2]; //Used temporarily by createMask()
//These two arrays are initialized by createMask() and used by checkReappearance():
unsigned int stbin[VSIZE+2], stbinmask[VSIZE+2];

void tryGlider(void)
{
	int i, j, k, m;
	
	gen=0;
	numEscGliders=0;
	//Erase all run boards:
	for (i=0; i<BSIZE; i++)
		for (j=0; j<BSIZE; j++)
		{
			for (k=0; k<3; k++)
				runboard[k][i][j]=0;
			initialboard[i][j]=0;
		}
	prv=0;
	cur=1;
	nxt=2;
	//Set the glider:
	runboard[cur][STL-2][STT-1]=1;
	runboard[cur][STL-3][STT-1]=1;
	runboard[cur][STL-2][STT-2]=1;
	runboard[cur][STL-4][STT-2]=1;
	runboard[cur][STL-2][STT-3]=1;
	initialboard[STL-2][STT-1]=1;
	initialboard[STL-3][STT-1]=1;
	initialboard[STL-2][STT-2]=1;
	initialboard[STL-4][STT-2]=1;
	initialboard[STL-2][STT-3]=1;
	initialpop=5;
	//Copy the stable pattern next to the glider:
	for (k=patLeft; k<=patRight; k++)
		for (m=patTop; m<=patBottom; m++)
			if (stboard[k][m]==9)
			/*(Blinker dummy cells are marked 10 and should be counted as off)*/
			{
				runboard[cur][STL+k-1][STT+m-1]=1;
				initialboard[STL+k-1][STT+m-1]=1;
				initialpop++;
			}
	createMask();
	//Initialize the run board's limits:
	runLeft[cur]=STL-4;
	runTop[cur]=STT-3;
	runRight[cur]=STL+patRight;
	runBottom[cur]=STT+patBottom;
	runLeft[prv]=runLeft[nxt]=runTop[prv]=runTop[nxt]=BSIZE;
	runRight[prv]=runRight[nxt]=runBottom[prv]=runBottom[nxt]=0;
	
	//Run this pattern and see what happens:
	run();
	
	if (foundcount % ANNOUNCEX == 0)
		printf("%d patterns so far...\n", foundcount);
}

void createMask()
{
	int i, j;
	//Create the mask:
	for (i=0; i<HSIZE+2; i++)
		for (j=0; j<VSIZE+2; j++)
			stmask[i][j]=0;
	for (i=patLeft; i<=patRight; i++)
		for (j=patTop; j<=patBottom; j++)
			if (stboard[i][j]==9)
			{
				stmask[i+1][j+1]=2;
				if (stmask[i  ][j  ]==0) stmask[i  ][j  ]=1;
				if (stmask[i+1][j  ]==0) stmask[i+1][j  ]=1;
				if (stmask[i+2][j  ]==0) stmask[i+2][j  ]=1;
				if (stmask[i+2][j+1]==0) stmask[i+2][j+1]=1;
				if (stmask[i+2][j+2]==0) stmask[i+2][j+2]=1;
				if (stmask[i+1][j+2]==0) stmask[i+1][j+2]=1;
				if (stmask[i  ][j+2]==0) stmask[i  ][j+2]=1;
				if (stmask[i  ][j+1]==0) stmask[i  ][j+1]=1;
				
				//if (stmask[i-1][j-1]==0) stmask[i-1][j-1]=1;
				if (stmask[i  ][j-1]==0) stmask[i  ][j-1]=1;
				if (stmask[i+1][j-1]==0) stmask[i+1][j-1]=1;
				if (stmask[i+2][j-1]==0) stmask[i+2][j-1]=1;
				//if (stmask[i+3][j-1]==0) stmask[i+3][j-1]=1;
				if (stmask[i+3][j  ]==0) stmask[i+3][j  ]=1;
				if (stmask[i+3][j+1]==0) stmask[i+3][j+1]=1;
				if (stmask[i+3][j+2]==0) stmask[i+3][j+2]=1;
				//if (stmask[i+3][j+3]==0) stmask[i+3][j+3]=1;
				if (stmask[i+2][j+3]==0) stmask[i+2][j+3]=1;
				if (stmask[i+1][j+3]==0) stmask[i+1][j+3]=1;
				if (stmask[i  ][j+3]==0) stmask[i  ][j+3]=1;
				//if (stmask[i-1][j+3]==0) stmask[i-1][j+3]=1;
				if (stmask[i-1][j+2]==0) stmask[i-1][j+2]=1;
				if (stmask[i-1][j+1]==0) stmask[i-1][j+1]=1;
				if (stmask[i-1][j  ]==0) stmask[i-1][j  ]=1;
			}
	//Convert the mask into the binary values:
	//(We put the pattern as far right as possible because checkReappearance()
	//only checks inside the edges of the active region of the board. Therefore,
	//if we left empty space at the right, checkReappearance() would not find the
	//pattern if it appeared at the edge.)
	for (j=0; j<VSIZE+2; j++)
	{
		stbin[j]=0;
		stbinmask[j]=0;
		for (i=patLeft-1; i<=patRight+3; i++)
		{
			//Put the new bit at the right and shift the older bits to the left:
			stbin[j]=2*stbin[j] + (stmask[i][j]==2);
			stbinmask[j]=2*stbinmask[j] + (stmask[i][j]!=0);
		}
	}
}

//Debugging function:
void printMask()
{
	for (int j=0; j<VSIZE+2; j++)
	{
		for (int i=0; i<HSIZE+2; i++)
		{
			if ( (stbin[j]&(1<<(HSIZE+1-i))) &&  (stbinmask[j]&(1<<(HSIZE+1-i))))
				printf("*");
			if (!(stbin[j]&(1<<(HSIZE+1-i))) &&  (stbinmask[j]&(1<<(HSIZE+1-i))))
				printf(".");
			if ( (stbin[j]&(1<<(HSIZE+1-i))) && !(stbinmask[j]&(1<<(HSIZE+1-i))))
				printf("X");
			if (!(stbin[j]&(1<<(HSIZE+1-i))) && !(stbinmask[j]&(1<<(HSIZE+1-i))))
				printf("_");
		}
		printf("\n");
	}
	printf("\n");
}

void run()
{
	int i, j, stillinitialpop=1;
	
	while(gen<MAXGENS)
	{
		pop=0;
		//Calculate the next generation:
		runMinFwDiag=runMinBkDiag=2*BSIZE;
		runMaxFwDiag=runMaxBkDiag=-2*BSIZE;
		int change=0, period2=1;
		//Set the limits to the widest between the current board
		//and the next board:
		int theL=(runLeft[cur]<runLeft[nxt])?runLeft[cur]:runLeft[nxt],
			theR=(runRight[cur]>runRight[nxt])?runRight[cur]:runRight[nxt],
			theT=(runTop[cur]<runTop[nxt])?runTop[cur]:runTop[nxt],
			theB=(runBottom[cur]>runBottom[nxt])?runBottom[cur]:runBottom[nxt];
		runLeft[nxt]=runTop[nxt]=BSIZE;
		runRight[nxt]=runBottom[nxt]=0;
		for (i=theL-1; i<=theR+1; i++)
			for (j=theT-1; j<=theB+1; j++)
			{
				int count= runboard[cur][i-1][j-1] + runboard[cur][i  ][j-1]
				         + runboard[cur][i+1][j-1] + runboard[cur][i-1][j  ]
				         + runboard[cur][i+1][j  ] + runboard[cur][i-1][j+1]
				         + runboard[cur][i  ][j+1] + runboard[cur][i+1][j+1];
				runboard[nxt][i][j]= (runboard[cur][i][j]==1 && (count==2 || count==3))
					|| (runboard[cur][i][j]==0 && count==3);
				if (runboard[nxt][i][j])
					pop++;
				if (runboard[cur][i][j]!=runboard[nxt][i][j])
					change=1;
				if (runboard[prv][i][j]!=runboard[nxt][i][j])
					period2=0;
				if (runboard[nxt][i][j]==1)
				{
					if (i<runLeft[nxt]) runLeft[nxt]=i;
					if (i>runRight[nxt]) runRight[nxt]=i;
					if (j<runTop[nxt]) runTop[nxt]=j;
					if (j>runBottom[nxt]) runBottom[nxt]=j;
					
					if (i+j<runMinFwDiag) runMinFwDiag=i+j;
					if (i+j>runMaxFwDiag) runMaxFwDiag=i+j;
					if (i-j<runMinBkDiag) runMinBkDiag=i-j;
					if (i-j>runMaxBkDiag) runMaxBkDiag=i-j;
				}
			}
		prv=cur;
		cur=nxt;
		nxt=(nxt+1)%3;
		
		gen++;
		
		if (runLeft[cur]==2 || runTop[cur]==2 || runRight[cur]==BSIZE-3
			|| runBottom[cur]==BSIZE-3)
		{
			/*printf("Pattern #%d reached the edge at gen. %d\n",
				foundcount, gen, numEscGliders);
			if (numEscGliders>=1)
				printf("%d glider%s escaped.\n", numEscGliders, numEscGliders==1?"":"s");
			printinitialboard();*/
			numEdge++;
			return;
		}
		
		//We detect the start of the reaction by seeing when the initial
		//population first changes. We are using the fact
		//that both the blinker and the glider have constant population.
		if (stillinitialpop && pop!=initialpop)
		{
			stillinitialpop=0;
			//printf("Reaction started at gen. %d.\n", gen);
		}
		
		if (pop==0)
		{
			/*printf("Pattern #%d died at gen. %d.\n", foundcount, gen);
			if (numEscGliders>=1)
				printf("%d glider%s escaped.\n", numEscGliders, numEscGliders==1?"":"s");
			printinitialboard();*/
			numDied++;
			return;
		}
		if (!change)
		{
			//The board stabilized, so check if the original pattern
			//re-appeared:
			if (checkReappearance())
			{
				numReapp++;
				return;
			}
			/*printf("Pattern #%d stabilized at gen. %d.\n", foundcount, gen);
			if (numEscGliders>=1)
				printf("%d glider%s escaped.\n", numEscGliders, numEscGliders==1?"":"s");
			printinitialboard();*/
			numStable++;
			return;
		}
		if (period2)
		{
			//Check if the original pattern re-appeared at either phase:
			if (checkReappearance())
			{
				numReapp++;
				return;
			}
			//Go back to the previous generation:
			cur=prv;
			gen--;
			//Check again:
			if (checkReappearance())
			{
				numReapp++;
				return;
			}
			
			cur=(cur+1)%3;
			gen++;
			/*printf("Pattern #%d became p2 at gen. %d.\n", foundcount, gen);
			if (numEscGliders>=1)
				printf("%d glider%s escaped.\n", numEscGliders, numEscGliders==1?"":"s");
			printinitialboard();*/
			numP2++;
			return;
		}
		
		if (gen%CHKGLIDERX==0)
			checkGlider();
		
		if (numEscGliders==1 && stillinitialpop)
		{
			/*printf("Pattern %d let the glider through.\n", foundcount);
			printinitialboard();*/
			numThru++;
			return;
		}
	}
	/*printf("Pattern #%d exceeded MAXGENS=%d.\n", foundcount, gen);
	if (numEscGliders>=1)
		printf("%d glider%s escaped.\n", numEscGliders, numEscGliders==1?"":"s");
	printinitialboard();*/
	numLong++;
}

/* How checkReappearance() works:
   The pattern to search for is represented in the arrays stbin[] and stbinmask[].
   Each entry of stbin[] represents a horizontal line in the pattern. Each cell of the line
   corresponds to a bit of the entry, which is set to either 0 or 1. stbinmask[] identifies
   which cells of the pattern should be checked. They are represented in the same way.
   The main loop of checkReappearance goes through each line of the board and puts the
   values of the cells in the bits of the variable "line". Each time, it shifts the older
   bits to the left and adds the new bit to the far right. The old bits "fall off" at the
   left.
   Each time it wants to compare the board to the pattern, it ANDs the line with the
   appropiate mask and then compares it to the corresponding entry in stbin[].
   The array lineN[][] keeps track of all the partial matches simultaneously.
   The program first checks if the first line of the pattern matches. It sets the corresponding
   entry in lineN[][] to 0 or 1 depending. If it set it to 1, then at the same position
   of the next line it will check the second line of the pattern, and either keep that entry as 1
   or set it back to 0. It keeps going like that. If the last line of the pattern matches,
   it means the pattern re-appeared, so checkReappearance() prints out the pattern and
   returns 1.
*/
int checkReappearance()
{
	static int lineN[BSIZE][BSIZE];
	int i, j, k;
	unsigned int line;
	
	//Pre-calculate often-used values:
	int binTop=patTop-1, vDiff=patBottom-patTop+4, hDiff=patRight-patLeft+3,
		checkTop=runTop[cur]-3, checkBottom=runBottom[cur]+3, checkLeft=runLeft[cur]-3,
		checkRight=runRight[cur]+3;
	/*3 cells of padding on all sides, instead of 2, because if the pattern contains
	  a blinker at the edge, then the pattern boundary variables will indicate one cell bigger
	  than it really is. Therefore, checkReappearance would not find it if it appeared at the
	  edge of the active board. For example, it wouldn't detect that the following pattern
	  re-appears at generation 30:
	  ..o......
	  o.o......
	  .oo......
	  .........
	  ....ooo..
	  .........
	  ........o
	  ........o
	  ........o
	*/

	
	for (i=0; i<BSIZE; i++)
		for (j=0; j<BSIZE; j++)
			lineN[i][j]=0;
	
	//Main loop - go through the active region of runboard[][]:
	for (j=checkTop; j<=checkBottom; j++)
	{
		line=0;
		for (i=checkLeft; i<=checkRight; i++)
		{
			//Put the new bit at the right and shift the older bits to the left:
			line=2*line+runboard[cur][i][j];
			if (i>=hDiff)
			{
				//Check if the first line of the pattern matches:
				if ( (line&stbinmask[binTop]) == stbin[binTop] )
					lineN[i][j]=1;
				else
					lineN[i][j]=0;
				//If we have a partial match of previous lines of the pattern,
				//check if the next line matches:
				for (k=j-vDiff; k<j; k++)
				{
					if (k<checkTop)
						continue;
					if (lineN[i][k]==1)
						if ((line&stbinmask[j-k+binTop]) != stbin[j-k+binTop])
							lineN[i][k]=0;
						//Else leave the entry in lineN[][] at 1
				}
				if (j-vDiff>=checkTop && lineN[i][j-vDiff]==1)
				{
					printf("Pattern #%d re-appeared at generation %d (shift: (%d, %d)).\n",
						foundcount, gen, i-patRight-STL-1, j-patBottom-STT-1);
					if (numEscGliders>=1)
						printf("%d glider%s escaped.\n", numEscGliders,
							numEscGliders==1?"":"s");
					printinitialboard();
					return 1;
				}
			}
		}
	}
	return 0;
}

void printinitialboard()
{
	for (int m=STT-4; m<STT+VSIZE; m++)
	{
		for (int k=STL-5; k<STL+HSIZE; k++)
			putchar(initialboard[k][m]?'*':'.');
		putchar('\n');
	}
}

//Debugging function:
void print(int l, int t, int r, int b)
{
	int i, j;
	FILE *f=stdout;
	
	if (l<0) l=0;
	if (t<0) t=0;
	if (r>=BSIZE) r=BSIZE-1;
	if (b>=BSIZE) b=BSIZE-1;
	
	fprintf(f, "Generation %d:\n", gen);
	int btp=cur; //Board to print
	for (j=t; j<=b; j++)
	{
		for (i=l; i<=r; i++)
			if (i<runLeft[btp] || i>runRight[btp] || j<runTop[btp] || j>runBottom[btp])
				fprintf(f, "#");
			else
				fprintf(f, "%c", runboard[btp][i][j]==0?'.':runboard[btp][i][j]==1?'*':'X');
		fprintf(f, "\n");
	}
}