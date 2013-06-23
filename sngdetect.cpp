/*sngdetect
  Version 1.0
  (Simple and Glider with escaped-glider Detection)
  March 2000
  Gabriel Nivasch
*/

#include <stdio.h>
#include <stdlib.h>
//#include <profiler.h>
//#include <DateTimeUtils.h>
#include "sngdetect.h"

/*Note: if MAXPATWIDTH or MAXPATHEIGHT are set to less than the size of one of the
  objects, the program might incorrectly produce patterns that include that object.
  There's no point in setting these constants to such values, so just don't do it.
*/

void printAllObjects(void);
void printStBoard(FILE *);
int placeNext();
void backup(void);

typedef int objCoord[2], //{x, y}
	nbrsCoord[3]; //{x, y, # on neighbors from itself - always 1 or 2}

#define NUMTOTALOBJECTS 14
#define NUMOBJECTS 10 //Number of used objects

objCoord block[4]={{0, 0}, {0, 1}, {1, 0}, {1, 1}},
	beehive1[6]={{1, 0}, {2, 0}, {3, 1}, {2, 2}, {1, 2}, {0, 1}},
	beehive2[6]={{0, 1}, {0, 2}, {1, 3}, {2, 2}, {2, 1}, {1, 0}},
	pond[8]={{1, 0}, {2, 0}, {3, 1}, {3, 2}, {2, 3}, {1, 3}, {0, 2}, {0, 1}},
	loaf1[7]={{1, 0}, {2, 0}, {3, 1}, {2, 2}, {1, 3}, {0, 2}, {0, 1}},
	loaf2[7]={{2, 0}, {1, 0}, {0, 1}, {1, 2}, {2, 3}, {3, 2}, {3, 1}},
	loaf3[7]={{2, 3}, {1, 3}, {0, 2}, {1, 1}, {2, 0}, {3, 1}, {3, 2}},
	loaf4[7]={{1, 3}, {2, 3}, {3, 2}, {2, 1}, {1, 0}, {0, 1}, {0, 2}},
	blinker1[5]={{1, 0}, {1, 1}, {1, 2}, {0, 1}, {2, 1}}, //The latter 2 cells are "dummy".
		//They are needed so that all the blinker's neighbor cells be adjacent.
	blinker2[5]={{0, 1}, {1, 1}, {2, 1}, {1, 0}, {1, 2}}, //Same here.
	boat1[5]={{1, 0}, {2, 1}, {1, 2}, {0, 2}, {0, 1}},
	boat2[5]={{1, 0}, {0, 1}, {1, 2}, {2, 2}, {2, 1}},
	boat3[5]={{1, 2}, {0, 1}, {1, 0}, {2, 0}, {2, 1}},
	boat4[5]={{1, 2}, {2, 1}, {1, 0}, {0, 0}, {0, 1}},
	
	*object[NUMTOTALOBJECTS]={block, beehive1, beehive2, pond, loaf1, loaf2,
		loaf3, loaf4, blinker1, blinker2, boat1, boat2, boat3, boat4};

nbrsCoord blocknbrs[12]={{-1, -1, 1}, {-1, 0, 2}, {-1, 1, 2}, {-1, 2, 1}, {0, 2, 2}, {1, 2, 2},
		{2, 2, 1}, {2, 1, 2}, {2, 0, 2}, {2, -1, 1}, {1, -1, 2}, {0, -1, 2}},
	beehive1nbrs[18]={{0, -1, 1}, {1, -1, 2}, {2, -1, 2}, {3, -1, 1}, {3, 0, 2}, {4, 0, 1},
		{4, 1, 1}, {4, 2, 1}, {3, 2, 2}, {3, 3, 1}, {2, 3, 2}, {1, 3, 2}, {0, 3, 1}, {0, 2, 2},
		{-1, 2, 1}, {-1, 1, 1}, {-1, 0, 1}, {0, 0, 2}},
	beehive2nbrs[18]={{-1, 0, 1}, {-1, 1, 2}, {-1, 2, 2}, {-1, 3, 1}, {0, 3, 2}, {0, 4, 1},
		{1, 4, 1}, {2, 4, 1}, {2, 3, 2}, {3, 3, 1}, {3, 2, 2}, {3, 1, 2}, {3, 0, 1}, {2, 0, 2},
		{2, -1, 1}, {1, -1, 1}, {0, -1, 1}, {0, 0, 2}},
	pondnbrs[20]={{0, 0, 2}, {0, -1, 1}, {1, -1, 2}, {2, -1, 2}, {3, -1, 1}, {3, 0, 2},
		{4, 0, 1}, {4, 1, 2}, {4, 2, 2}, {4, 3, 1}, {3, 3, 2}, {3, 4, 1}, {2, 4, 2}, {1, 4, 2},
		{0, 4, 1}, {0, 3, 2}, {-1, 3, 1}, {-1, 2, 2}, {-1, 1, 2}, {-1, 0, 1}},
	loaf1nbrs[20]={{0, 0, 2}, {0, -1, 1}, {1, -1, 2}, {2, -1, 2}, {3, -1, 1}, {3, 0, 2},
		{4, 0, 1}, {4, 1, 1}, {4, 2, 1}, {3, 2, 2}, {3, 3, 1}, {2, 3, 2}, {2, 4, 1}, {1, 4, 1},
		{0, 4, 1}, {0, 3, 2}, {-1, 3, 1}, {-1, 2, 2}, {-1, 1, 2}, {-1, 0, 1}},
	loaf2nbrs[20]={{3, 0, 2}, {3, -1, 1}, {2, -1, 2}, {1, -1, 2}, {0, -1, 1}, {0, 0, 2},
		{-1, 0, 1}, {-1, 1, 1}, {-1, 2, 1}, {0, 2, 2}, {0, 3, 1}, {1, 3, 2}, {1, 4, 1}, {2, 4, 1},
		{3, 4, 1}, {3, 3, 2}, {4, 3, 1}, {4, 2, 2}, {4, 1, 2}, {4, 0, 1}},
	loaf3nbrs[20]={{3, 3, 2}, {3, 4, 1}, {2, 4, 2}, {1, 4, 2}, {0, 4, 1}, {0, 3, 2},
		{-1, 3, 1}, {-1, 2, 1}, {-1, 1, 1}, {0, 1, 2}, {0, 0, 1}, {1, 0, 2}, {1, -1, 1}, {2, -1, 1},
		{3, -1, 1}, {3, 0, 2}, {4, 0, 1}, {4, 1, 2}, {4, 2, 2}, {4, 3, 1}},
	loaf4nbrs[20]={{0, 3, 2}, {0, 4, 1}, {1, 4, 2}, {2, 4, 2}, {3, 4, 1}, {3, 3, 2},
		{4, 3, 1}, {4, 2, 1}, {4, 1, 1}, {3, 1, 2}, {3, 0, 1}, {2, 0, 2}, {2, -1, 1}, {1, -1, 1},
		{0, -1, 1}, {0, 0, 2}, {-1, 0, 1}, {-1, 1, 2}, {-1, 2, 2}, {-1, 3, 1}},
	blinker1nbrs[16]={{0, 0, 2}, {0, -1, 1}, {1, -1, 1}, {2, -1, 1}, {2, 0, 2}, {3, 0, 1}, {3, 1, 1},
		{3, 2, 1}, {2, 2, 2}, {2, 3, 1}, {1, 3, 1}, {0, 3, 1}, {0, 2, 2}, {-1, 2, 1}, {-1, 1, 1},
		{-1, 0, 1}},
	blinker2nbrs[16]={{0, 0, 2}, {0, -1, 1}, {1, -1, 1}, {2, -1, 1}, {2, 0, 2}, {3, 0, 1}, {3, 1, 1},
		{3, 2, 1}, {2, 2, 2}, {2, 3, 1}, {1, 3, 1}, {0, 3, 1}, {0, 2, 2}, {-1, 2, 1}, {-1, 1, 1},
		{-1, 0, 1}},
	boat1nbrs[16]={{0, 0, 2}, {0, -1, 1}, {1, -1, 1}, {2, -1, 1}, {2, 0, 2}, {3, 0, 1},
		{3, 1, 1}, {3, 2, 1}, {2, 2, 2}, {2, 3, 1}, {1, 3, 2}, {0, 3, 2}, {-1, 3, 1}, {-1, 2, 2},
		{-1, 1, 2}, {-1, 0, 1}},
	boat2nbrs[16]={{2, 0, 2}, {2, -1, 1}, {1, -1, 1}, {0, -1, 1}, {0, 0, 2}, {-1, 0, 1},
		{-1, 1, 1}, {-1, 2, 1}, {0, 2, 2}, {0, 3, 1}, {1, 3, 2}, {2, 3, 2}, {3, 3, 1}, {3, 2, 2},
		{3, 1, 2}, {3, 0, 1}},
	boat3nbrs[16]={{2, 2, 2}, {2, 3, 1}, {1, 3, 1}, {0, 3, 1}, {0, 2, 2}, {-1, 2, 1},
		{-1, 1, 1}, {-1, 0, 1}, {0, 0, 2}, {0, -1, 1}, {1, -1, 2}, {2, -1, 2}, {3, -1, 1}, {3, 0, 2},
		{3, 1, 2}, {3, 2, 1}},
	boat4nbrs[16]={{0, 2, 2}, {0, 3, 1}, {1, 3, 1}, {2, 3, 1}, {2, 2, 2}, {3, 2, 1},
		{3, 1, 1}, {3, 0, 1}, {2, 0, 2}, {2, -1, 1}, {1, -1, 2}, {0, -1, 2}, {-1, -1, 1}, {-1, 0, 2},
		{-1, 1, 2}, {-1, 2, 1}},
		
	*nbrs[NUMTOTALOBJECTS]={blocknbrs, beehive1nbrs, beehive2nbrs, pondnbrs, loaf1nbrs, loaf2nbrs,
		loaf3nbrs, loaf4nbrs, blinker1nbrs, blinker2nbrs, boat1nbrs, boat2nbrs, boat3nbrs, boat4nbrs};

int objectlen[NUMTOTALOBJECTS]={4, 6, 6, 8, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5},
	nbrslen[NUMTOTALOBJECTS]={12, 18, 18, 20, 20, 20, 20, 20, 16, 16, 16, 16, 16, 16};

int stboard[HSIZE][VSIZE]; //The cells at the edges should remain off

int numplaced=0, x=1, y=1, objnum=0, foundcount=0;

//Variables used for keeping track of the pattern's size.
//(The pattern's size includes only the live cells themselves)
int patLeft=HSIZE-2, patRight=1, patTop=VSIZE-2, patBottom=1;

typedef int placement[3]; //{x, y, object #}
placement pmtList[MAXPLACED];

extern int numReapp, numStable, numDied, numP2, numEdge, numLong, numThru;

int main()
{
	int i, j, wasPlaced;
	
	/*if (ProfilerInit(collectDetailed, bestTimeBase, 20, 10)!=noErr)
	{ printf("ProfilerInit() failed.\n"); return 0; }*/
	
	//Check that each line of the pattern can fit in an integer:
	if (HSIZE+2>8*sizeof(int))
	{
		printf("Pattern too wide.\n");
		exit(1);
	}
	
	printf("HSIZE=%d, VSIZE=%d,\nMINPLACED=%d, MAXPLACED=%d,\nMINPATWIDTH=%d, "
		"MINPATHEIGHT=%d,\nMAXPATWIDTH=%d, MAXPATHEIGHT=%d,\n\n"
		"BSIZE=%d, STL=%d, STT=%d, MAXGENS=%d\n\n", HSIZE, VSIZE, MINPLACED,
		MAXPLACED, MINPATWIDTH, MINPATHEIGHT, MAXPATWIDTH, MAXPATHEIGHT,
		BSIZE, STL, STT, MAXGENS);
	
	/*unsigned long startTime, endTime;
	GetDateTime(&startTime);*/
	
	for (i=0; i<HSIZE; i++)
		for (j=0; j<VSIZE; j++)
			stboard[i][j]=0;
	
	/*//Test Elkies' ship reaction:
	stboard[3][1]=stboard[4][1]=stboard[3][2]=stboard[5][2]=stboard[4][3]=stboard[5][3]=9;
	patLeft=3; patRight=5; patTop=1; patBottom=3; tryGlider(); return 0;*/
	
	//FILE *f=fopen("patterns", "w");
	
	//Main loop -- depth-first generation of stable combinations of objects:
	while(1)
	{
		//Try to place another object:
		wasPlaced=placeNext();
		if (numplaced==0) //End of the search.
			break;
		if (wasPlaced && numplaced>=MINPLACED && patRight-patLeft+1>=MINPATWIDTH
			&& patBottom-patTop+1>=MINPATHEIGHT)
		{
			foundcount++;
			//printStBoard(stdout);
			tryGlider();
		}
		if (numplaced==MAXPLACED || !wasPlaced) //Can't place more objects for either reason.
			backup(); //backup() will change the values of x, y, and objnum.
	}
	
	//fclose(f);
	
	//GetDateTime(&endTime);
	
	printf("\nDone.\n%d patterns tested:\n%d re-appeared.\n%d became stable.\n"
		"%d died out.\n%d became period 2.\n%d reached the edge.\n%d exceeded"
		" the generation limit.\n%d let the glider through.\n\n",
		foundcount, numReapp, numStable, numDied,
		numP2, numEdge, numLong, numThru);
	printf("Test completed in %d seconds.\n", endTime-startTime);
	printf("   ----------\n");
	
	/*if(ProfilerDump("\pprofile")!=noErr)
		printf("ProfilerDump() failed.");
	ProfilerTerm();*/
	
	return 0;
}

int placeNext()
{
	int k, ax, ay;
	while(x<patLeft+MAXPATWIDTH && x<HSIZE-1)
	{
		//See if the object objnum can fit in position (x, y):
		for (k=0; k<objectlen[objnum]; k++)
		{
			ax=x+object[objnum][k][0];
			ay=y+object[objnum][k][1];
			
			if (ax<1 || ax-patLeft>=MAXPATWIDTH  || ax>=HSIZE-1 || patRight -ax>=MAXPATWIDTH  ||
				ay<1 || ay-patTop >=MAXPATHEIGHT || ay>=VSIZE-1 || patBottom-ay>=MAXPATHEIGHT ||
				stboard[ax][ay]!=0)
				break;
		}
		if (k<objectlen[objnum]) //We breaked
		{
			//Increment & retry:
			objnum++;
			if (objnum==NUMOBJECTS)
			{
				objnum=0;
				y++;
				if (y==patTop+MAXPATHEIGHT || y==VSIZE-1)
				{
					y=patBottom-MAXPATHEIGHT+1;
					if (y<1) y=1;
					x++;
				}
			}
			continue;
		}
		for (k=0; k<nbrslen[objnum]; k++)
		{
			ax=x+nbrs[objnum][k][0];
			ay=y+nbrs[objnum][k][1];
			
			if (ax<0 || ax>=HSIZE || ay<0 || ay>=VSIZE || stboard[ax][ay]>=9)
			{ printf("Bad error in placeNext(). x=%d, y=%d, objnum=%d.\n", x, y, objnum); exit(0); }
			if (stboard[ax][ay] + nbrs[objnum][k][2] == 3)
				break;
		}
		if (k<nbrslen[objnum]) //We breaked
		{
			//Increment & retry:
			objnum++;
			if (objnum==NUMOBJECTS)
			{
				objnum=0;
				y++;
				if (y==patTop+MAXPATHEIGHT || y==VSIZE-1)
				{
					y=patBottom-MAXPATHEIGHT+1;
					if (y<1) y=1;
					x++;
				}
			}
			continue;
		}
		//The object passed all the tests, so let's go ahead and place it,
		//adjusting the globals that keep track of the boundaries:
		for (k=0; k<objectlen[objnum]; k++)
		{
			ax=x+object[objnum][k][0];
			ay=y+object[objnum][k][1];
			
			//In the blinker, the last 2 cells are dummy, so they are marked
			//in a special way:
			if ((objnum==8 || objnum==9) && k>=3)
				stboard[ax][ay]=10;
			else
				stboard[ax][ay]=9;
			if (ax<patLeft) patLeft=ax;
			if (ax>patRight) patRight=ax;
			if (ay<patTop) patTop=ay;
			if (ay>patBottom) patBottom=ay;
		}
		for (k=0; k<nbrslen[objnum]; k++)
			stboard[x+nbrs[objnum][k][0]][y+nbrs[objnum][k][1]] += nbrs[objnum][k][2];
		
		//Record this placement:
		pmtList[numplaced][0]=x;
		pmtList[numplaced][1]=y;
		pmtList[numplaced][2]=objnum;
		numplaced++;
		return 1;
	}
	return 0;
}

void backup()
{
	int k, m, ax, ay;
	
	if (numplaced<1)
	{ printf("Bad error 1 in backup().\n"); exit(0); }
	
	numplaced--;
	x=pmtList[numplaced][0];
	y=pmtList[numplaced][1];
	objnum=pmtList[numplaced][2];
	//Take out the object from the board:
	for (k=0; k<objectlen[objnum]; k++)
	{
		ax=x+object[objnum][k][0];
		ay=y+object[objnum][k][1];
		
		if (stboard[ax][ay]!=9 && stboard[ax][ay]!=10)
		{ printf("Bad error 2 in backup().\n"); exit(0); }
		stboard[ax][ay]=0;
	}
	for (k=0; k<nbrslen[objnum]; k++)
	{
		stboard[x+nbrs[objnum][k][0]][y+nbrs[objnum][k][1]] -= nbrs[objnum][k][2];
		if (stboard[x+nbrs[objnum][k][0]][y+nbrs[objnum][k][1]] < 0)
		{ printf("Bad error 3 in backup().\n"); exit(0); }
	}
	//printf("Backup. Now numplaced==%d\n", numplaced);
	//Readjust the globals that keep track of the boundaries:
	if (numplaced==0) //The board is (supposedly) empty.
	{
		patLeft=HSIZE-2;
		patRight=1;
		patTop=VSIZE-2;
		patBottom=1;
	}
	else
	{
		for (k=patLeft; k<=patRight; k++)
		{
			patLeft=k;
			for (m=patTop; m<=patBottom; m++)
				if (stboard[k][m]==9 || stboard[k][m]==10) //Get out of this double for-loop
				{ m=VSIZE; k=HSIZE; }
		}
		for (k=patRight; k>=patLeft; k--)
		{
			patRight=k;
			for (m=patTop; m<=patBottom; m++)
				if (stboard[k][m]==9 || stboard[k][m]==10)
				{ m=VSIZE; k=0; }
		}
		for (m=patTop; m<=patBottom; m++)
		{
			patTop=m;
			for (k=patLeft; k<=patRight; k++)
				if (stboard[k][m]==9 || stboard[k][m]==10)
				{ k=HSIZE; m=VSIZE; }
		}
		for (m=patBottom; m>=patTop; m--)
		{
			patBottom=m;
			for (k=patLeft; k<=patRight; k++)
				if (stboard[k][m]==9 || stboard[k][m]==10)
				{ k=HSIZE; m=0; }
		}
	}
	//Increment x, y, and objnum:
	objnum++;
	if (objnum==NUMOBJECTS)
	{
		objnum=0;
		y++;
		if (y==patTop+MAXPATHEIGHT || y==VSIZE-1)
		{
			y=patBottom-MAXPATHEIGHT+1;
			if (y<1) y=1;
			x++;
		}
	}
}

//This funtion is not used:
void printStBoard(FILE *f)
{
	int i, j;
	fprintf(f, "Pattern #%d. Size: %dx%d:\n", foundcount,
		patRight-patLeft+1, patBottom-patTop+1);
	for (j=0; j<VSIZE; j++)
	{
		for (i=0; i<HSIZE; i++)
			switch(stboard[i][j])
			{
				case 0:
					fputc('.', f); break;
				case 1: case 2: case 4:
					fputc(stboard[i][j]+'0', f); break;
				case 3:
					fputc('?', f); exit(0);
				case 5: case 6: case 7: case 8:
					fputc('!', f); exit(0);
				case 9:
					fputc('*', f); break;
				case 10: //Blinker dummy cell
					fputc('k', f); break;
				default:
					fputc('X', f); exit(0);
			}
		fputc('\n', f);
	}
}

//Function for debugging:
void printAllObjects()
{
	int brd[9][9], i, j, k, m;
	
	for (k=0; k<NUMOBJECTS; k++)
	{
		//Clear the board:
		for (i=0; i<9; i++)
			for (j=0; j<9; j++)
				brd[i][j]=0;
		//Mark the object and its neighbors on the board:
		for (m=0; m<objectlen[k]; m++)
		{
			if (brd[1+object[k][m][0]][1+object[k][m][1]] != 0)
			{ printf("Problem with object #%d, coord #%d.\n", k, m); return; }
			brd[1+object[k][m][0]][1+object[k][m][1]]=9;
		}
		for (m=0; m<nbrslen[k]; m++)
		{
			if (brd[1+nbrs[k][m][0]][1+nbrs[k][m][1]] != 0)
			{ printf("Problem with nbrs #%d, coord #%d.\n", k, m); return; }
			brd[1+nbrs[k][m][0]][1+nbrs[k][m][1]]=nbrs[k][m][2];
		}
		//Print out:
		for (j=0; j<9; j++)
		{
			for (i=0; i<9; i++)
				switch(brd[i][j])
				{
					case 0: putchar('-'); break;
					case 1: putchar('.'); break;
					case 2: putchar(':'); break;
					case 9: putchar('*'); break;
					default: putchar('X'); break;
				}
			putchar('\n');
		}
		putchar('\n');
	}
}