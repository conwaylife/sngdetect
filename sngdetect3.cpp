/*sngdetect
  Version 1.0
  (Simple and Glider with escaped-glider Detection)
  March 2000
  Gabriel Nivasch
*/

#include <stdio.h>
#include "sngdetect.h"

extern int runboard[3][BSIZE][BSIZE];
extern int gen, numEscGliders, runLeft[3], runRight[3], runTop[3], runBottom[3],
  runMinFwDiag, runMaxFwDiag, runMinBkDiag, runMaxBkDiag, cur;

void checkGlider()
{
  /*Detecting an escaped glider:
    - Diagonals 2, 4, 5, and 6 are empty.
    - There's only 1 bit at diagonal 3.
    - At diagonal 0 there are only the 2 bits marked 'o'.
    - At diagonal 1 there is only the 'o' and one of the '?'s.
      6 5 4 3 2 1 0
       \ \ \ \ \ \ \
    . * . . . . . . . . . .
    * * * . . . . . ? o . .
    . . . * . . . . . o o .
    . . * * * . . . o . ? .
    . . . . . * . . . . . .
    . . . . * * * . . . . .
    . . . . . . . * . . . .
    . . . . . . * * * . . .
    . . . . . . . . . * . .
    . . . . . . . . * * * .
    . . . . . . . . . . . *
  */

  int i, j, bitAtDiag3, bitAt3i, bitAt3j,
    diag0count, diag1count, maybeGlider;
  //Check for NE-bound glider:
  bitAtDiag3=0;
  diag0count=0;
  diag1count=0;
  maybeGlider=1;
  for (j=runTop[cur], i=runMaxBkDiag+j; i-6<=runRight[cur] && j<=runBottom[cur]; i++, j++)
  {
    //Check diagonals 2, 4, 5, and 6:
    if ((i-6>=runLeft[cur] && runboard[cur][i-6][j]==1)
      ||(i-5>=runLeft[cur] && i-5<=runRight[cur] && runboard[cur][i-5][j]==1)
      ||(i-4>=runLeft[cur] && i-4<=runRight[cur] && runboard[cur][i-4][j]==1)
      ||(i-2>=runLeft[cur] && i-2<=runRight[cur] && runboard[cur][i-2][j]==1))
    { maybeGlider=0; break; }
    //Check diagonal 3:
    if (i-3>=runLeft[cur] && i-3<=runRight[cur] && runboard[cur][i-3][j]==1)
    {
      if (bitAtDiag3)
      { maybeGlider=0; break; }
      bitAtDiag3=1;
      bitAt3i=i-3;
      bitAt3j=j;
    }
    //Check diagonal 1:
    if (i-1>=runLeft[cur] && i-1<=runRight[cur] && runboard[cur][i-1][j]==1)
    {
      diag1count++;
      if (diag1count>2)
      { maybeGlider=0; break; }
    }
    //Check diagonal 0:
    if (i<=runRight[cur] && runboard[cur][i][j]==1)
    {
      diag0count++;
      if (diag0count>2)
      { maybeGlider=0; break; }
    }
  }
  if (maybeGlider && bitAtDiag3 && diag1count==2 && diag0count==2
    && bitAt3i+2<=runRight[cur] && bitAt3j-2>=runTop[cur]
    && (runboard[cur][bitAt3i  ][bitAt3j-2]==1 || runboard[cur][bitAt3i+2][bitAt3j  ]==1)
    &&  runboard[cur][bitAt3i+1][bitAt3j-2]==1
    &&  runboard[cur][bitAt3i+1][bitAt3j-1]==1
    &&  runboard[cur][bitAt3i+2][bitAt3j-1]==1)
  {
    //printf("Glider escaped to the NE at generation %d.\n", gen);
    //Kill the glider:
    runboard[cur][bitAt3i+1][bitAt3j-1]=0;
    numEscGliders++;
  }

  //Check for SE-bound glider:
  bitAtDiag3=0;
  diag0count=0;
  diag1count=0;
  maybeGlider=1;
  for (j=runBottom[cur], i=runMaxFwDiag-j; i-6<=runRight[cur] && j>=runTop[cur]; i++, j--)
  {
    if ((i-6>=runLeft[cur] && runboard[cur][i-6][j]==1)
      ||(i-5>=runLeft[cur] && i-5<=runRight[cur] && runboard[cur][i-5][j]==1)
      ||(i-4>=runLeft[cur] && i-4<=runRight[cur] && runboard[cur][i-4][j]==1)
      ||(i-2>=runLeft[cur] && i-2<=runRight[cur] && runboard[cur][i-2][j]==1))
    { maybeGlider=0; break; }
    if (i-3>=runLeft[cur] && i-3<=runRight[cur] && runboard[cur][i-3][j]==1)
    {
      if (bitAtDiag3)
      { maybeGlider=0; break; }
      bitAtDiag3=1;
      bitAt3i=i-3;
      bitAt3j=j;
    }
    if (i-1>=runLeft[cur] && i-1<=runRight[cur] && runboard[cur][i-1][j]==1)
    {
      diag1count++;
      if (diag1count>2)
      { maybeGlider=0; break; }
    }
    if (i<=runRight[cur] && runboard[cur][i][j]==1)
    {
      diag0count++;
      if (diag0count>2)
      { maybeGlider=0; break; }
    }
  }
  if (maybeGlider && bitAtDiag3 && diag1count==2 && diag0count==2
    && bitAt3i+2<=runRight[cur] && bitAt3j+2<=runBottom[cur]
    && (runboard[cur][bitAt3i  ][bitAt3j+2]==1 || runboard[cur][bitAt3i+2][bitAt3j  ]==1)
    &&  runboard[cur][bitAt3i+1][bitAt3j+2]==1
    &&  runboard[cur][bitAt3i+1][bitAt3j+1]==1
    &&  runboard[cur][bitAt3i+2][bitAt3j+1]==1)
  {
    //printf("Glider escaped to the SE at generation %d.\n", gen);
    runboard[cur][bitAt3i+1][bitAt3j+1]=0;
    numEscGliders++;
  }

  //Check for NW-bound glider:
  bitAtDiag3=0;
  diag0count=0;
  diag1count=0;
  maybeGlider=1;
  for (j=runTop[cur], i=runMinFwDiag-j; i+6>=runLeft[cur] && j<=runBottom[cur]; i--, j++)
  {
    if ((i+6<=runRight[cur] && runboard[cur][i+6][j]==1)
      ||(i+5<=runRight[cur] && i+5>=runLeft[cur] && runboard[cur][i+5][j]==1)
      ||(i+4<=runRight[cur] && i+4>=runLeft[cur] && runboard[cur][i+4][j]==1)
      ||(i+2<=runRight[cur] && i+2>=runLeft[cur] && runboard[cur][i+2][j]==1))
    { maybeGlider=0; break; }
    if (i+3<=runRight[cur] && i+3>=runLeft[cur] && runboard[cur][i+3][j]==1)
    {
      if (bitAtDiag3)
      { maybeGlider=0; break; }
      bitAtDiag3=1;
      bitAt3i=i+3;
      bitAt3j=j;
    }
    if (i+1<=runRight[cur] && i+1>=runLeft[cur] && runboard[cur][i+1][j]==1)
    {
      diag1count++;
      if (diag1count>2)
      { maybeGlider=0; break; }
    }
    if (i>=runLeft[cur] && runboard[cur][i][j]==1)
    {
      diag0count++;
      if (diag0count>2)
      { maybeGlider=0; break; }
    }
  }
  if (maybeGlider && bitAtDiag3 && diag1count==2 && diag0count==2
    && bitAt3i-2>=runLeft[cur] && bitAt3j-2>=runTop[cur]
    && (runboard[cur][bitAt3i  ][bitAt3j-2]==1 || runboard[cur][bitAt3i-2][bitAt3j  ]==1)
    &&  runboard[cur][bitAt3i-1][bitAt3j-2]==1
    &&  runboard[cur][bitAt3i-1][bitAt3j-1]==1
    &&  runboard[cur][bitAt3i-2][bitAt3j-1]==1)
  {
    //printf("Glider escaped to the NW at generation %d.\n", gen);
    runboard[cur][bitAt3i-1][bitAt3j-1]=0;
    numEscGliders++;
  }

  //Check for SW-bound glider:
  bitAtDiag3=0;
  diag0count=0;
  diag1count=0;
  maybeGlider=1;
  for (j=runBottom[cur], i=runMinBkDiag+j; i+6>=runLeft[cur] && j>=runTop[cur]; i--, j--)
  {
    if ((i+6<=runRight[cur] && runboard[cur][i+6][j]==1)
      ||(i+5<=runRight[cur] && i+5>=runLeft[cur] && runboard[cur][i+5][j]==1)
      ||(i+4<=runRight[cur] && i+4>=runLeft[cur] && runboard[cur][i+4][j]==1)
      ||(i+2<=runRight[cur] && i+2>=runLeft[cur] && runboard[cur][i+2][j]==1))
    { maybeGlider=0; break; }
    if (i+3<=runRight[cur] && i+3>=runLeft[cur] && runboard[cur][i+3][j]==1)
    {
      if (bitAtDiag3)
      { maybeGlider=0; break; }
      bitAtDiag3=1;
      bitAt3i=i+3;
      bitAt3j=j;
    }
    if (i+1<=runRight[cur] && i+1>=runLeft[cur] && runboard[cur][i+1][j]==1)
    {
      diag1count++;
      if (diag1count>2)
      { maybeGlider=0; break; }
    }
    if (i>=runLeft[cur] && runboard[cur][i][j]==1)
    {
      diag0count++;
      if (diag0count>2)
      { maybeGlider=0; break; }
    }
  }
  if (maybeGlider && bitAtDiag3 && diag1count==2 && diag0count==2
    && bitAt3i-2>=runLeft[cur] && bitAt3j+2<=runBottom[cur]
    && (runboard[cur][bitAt3i  ][bitAt3j+2]==1 || runboard[cur][bitAt3i-2][bitAt3j  ]==1)
    &&  runboard[cur][bitAt3i-1][bitAt3j+2]==1
    &&  runboard[cur][bitAt3i-1][bitAt3j+1]==1
    &&  runboard[cur][bitAt3i-2][bitAt3j+1]==1)
  {
    //printf("Glider escaped to the SW at generation %d.\n", gen);
    runboard[cur][bitAt3i-1][bitAt3j+1]=0;
    numEscGliders++;
  }
}