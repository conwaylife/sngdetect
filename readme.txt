sngdetect v1.0
(Simple and Glider with escaped-glider Detection)
by Gabriel Nivasch
nivasch@attglobal.net
March 2000

	About sngdetect

This program generates patterns that are combinations of common objects, and
for each one, it determines what happens when a glider hits it. In particular,
it checks whether the original pattern re-appears, possibly translated but at
its original orientation, after the reaction settles down. While the pattern is
running, the program detects and eliminates escaped gliders in order to
increase the speed at which the pattern is run.

The program consists of the following files:
 - sngdetect.cpp: Contains the main() function and the pattern-generating code.
 - sngdetect2.cpp: Contains the code that runs the patterns and checks for
re-appearance.
 - sngdetect3.cpp: Contains the code for detecting escaped gliders.
 - sngdetect.h: Contains some adjustable parameters.

The reason I made this program was to try to build either an arbitrarily-slow
spaceship/puffer, or a stable glider reflector.

One way an arbitrarily-slow spaceship could work is having two stable or
oscillating patterns and a glider that shuttles back and forth between them.
When the glider hits one of the patterns, the reaction produces the same
pattern moved a certain distance and a glider travelling in the opposite
direction. When the glider reaches the second pattern, the latter moves by the
same amount and emits a glider in the original direction and in the appropriate
path. If we had such a configuration, then increasing the distance between the
patterns would increase the period and reduce the speed of the spaceship.

If the original pattern happened to re-appear at its original location and
produced a glider and nothing else, then it would constitute a stable glider
reflector (if the starting pattern had only still lifes). If it produced some
extra objects, one could try to control the reaction with eaters or other
"catalysts".

One only needs to test the glider coming in one path, because shifting the
glider one way is the same as shifting the pattern the other.

In Life, the 5 most common objects that arise from random soups are: the
blinker, block, beehive, pond, and loaf. All of these are still lifes except
for the blinker. These are the objects that the program uses to make up the
initial patterns.

	Implementation of the program

The program generates the initial patterns using a depth-first search. It
enforces on them several user-adjustable constraints.

Each of the basic objects is represented with two arrays. The first one
contains the coordinates of each live cell of the object. The second one
contains the coordinates of each dead neighbor that has to be checked. The
second array also contains the number of live neighbors the cell already has
from the object itself. In order to make sure that the combination is stable,
the program adds up these neighbor counts and makes sure they don't add up to
3. This strategy was originally designed assuming that the patterns are still
lifes; when I added the blinker I had to make some minor ad hoc modifications.

Some of the objects have more than one orientation or phase, so the program has
a separate copy for each of them. Thus, the program has 10 objects: 2 blinkers,
1 block, 2 beehives, 1 pond, and 4 loaves.

In order to generate each pattern only once, the objects can only be placed in
order of their positions on the board. The program stores the current position
and object number in the global variables x, y, and objnum. The array pmtList[]
functions as a stack that keeps track of the position and object number of all
the objects placed so far.

The program keeps a running track of the top, left, bottom, and right of the
pattern, in order to compare its width and height to the specified maxima and
minima.

	A few notes about pattern generation

The program doesn't allow two objects to touch each other, like in the bi-pond:

 .**....
 *..*...
 *..*...
 .**.**.
 ...*..*
 ...*..*
 ....**.

Also, it only generates combinations such that the objects can be placed one at
a time. That excludes combinations like a block of 4 blocks. If the program
included the eater, these kind of objects would also be excluded:

 .......**
 ......*.*
 ......*..
 .....**..
 ..**.....
 ..**.**..
 .....**..
 ..**.....
 ..*......
 *.*......
 **.......

Furthermore, there are objects that can be assembled one by one only in a
particular order. The program would generate them only if it attempts the
correct order:

 ..**.**
 ..**.*.
 ......*
 ..**.**
 ..*....
 *.*....
 **.....

	Testing the patterns
 
For each pattern generated, the program calls the tryGlider() function.

When tryGlider() is called, it places the pattern and the initial glider on the
run board, and then runs the pattern generation by generation. At each
generation, the program calls checkGlider(), which checks if there are any
gliders that escaped, and ig so, deletes them. The program also checks at each
generation whether the pattern settled to period 1 or 2.

Once the pattern stabilized, the program checks whether the original pattern
re-appeared, by calling checkReappearance(). checkReappearance() checks if the
original pattern is present anywhere on the board, and it uses a "mask" to make
sure the pattern is surrounded by a border 2 cells thick of dead cells all
around. (Now that I'm thinking, there's no need to make sure the pattern is
surrounded by dead cells, since we know the board stabilized. This is a vestige
of earlier versions of the program...) checkReappearance() does this by passing
through the board only once, using a boolean array to keep track of partial
matches.

If a pattern exceeds the size or generation limits, it is rejected.

When the program finishes, it prints out the total number of patterns tested,
and how many of them re-appeared, became stable, died out completely, became
period 2, reached the edge, exceeded the generation limit, and let the initial
glider pass through without reacting.

   Speed considerations

The pattern-generating part of the program is much, much faster than the
testing part. This means that the overhead of generating patterns that are not
going to be tested is negligible.

This also means that you can first run the program with the tryGlider()
function call commented out, and the program will just generate the patterns
and count them. That way you will have an idea of how long a particular run
will take.

   Adjustable parameters

The adjustable parameters are macros located in "sngdetect.h".

MINPLACED and MAXPLACED are the minimum are maximum number of simple objects
the pattern should have.

BSIZE is the size of the board where the pattern is run. The board is always
square.

STL and STT are the coordinates of the top-left corner of the initial pattern
in the running board. Usually their values are BSIZE/2.

MAXGENS is the maximum number of generations the pattern can run.

ANNOUNCEX: Every ANNOUNCEX patterns the program will print a status message to
the screen.

CHKGLIDERX is the frequency with which the program calls checkGlider(). It
should be an odd number. It turns out that the extra time checkGlider() takes
is insignificant, so this parameter should be left at 1.

HSIZE and VSIZE specify the size of the board where patterns are generated. The
border cells of the board have to remain off, so the maximum size a pattern can
have is (HSIZE-2)x(VSIZE-2).

MINPATWIDTH, MINPATHEIGHT, MAXPATWIDTH, and MAXPATHEIGHT specify the minimum
and maximum size the pattern should have.

Here are some calculations to illustrate the relationship between all these
size parameters:

There are 4 patters exactly 6x4; this can be verified by setting MINPATWIDTH=
MAXPATWIDTH=6, MINPATHEIGHT=MAXPATHEIGHT=4, HSIZE=8, VSIZE=6, and running the
program.

There are 19 patterns exactly 6x5.

There are 31 patterns exactly 7x4.

There are 55 patterns exactly 7x5.

This implies that there should be 4*4 + 2*19 + 2*31 + 55 = 171 patterns between
6x4 and 7x5. This is indeed the count produced by the program.

This also implies that in a 8x5 board (setting HSIZE=10 and VSIZE=7), there
should be the following number of patterns between 6x4 and 7x5: 6*4 + 3*19 +
4*31 + 2*55 = 315. This is also verified running the program.

In a 7x6 board, the number of patterns between 6x4 and 7x5 should be: 6*4 +
4*19 + 3*31 + 2*55 = 303. This also coincides with the program.

	Version History

sngdetect 1.0:
- Added escaped-glider detection.
- Now the program checks for re-appearance only after the reaction settles
down.
- Increased the default board size to 300x300 and the default generation limit
to 1000.

Stableshift 1.11:
- The blinker dummy cells were not handled completely correctly. Now they are
marked in the stable board with a special number, so that they be considered as
part of the pattern for size purposes.
- Changed checkReappearance() to check one more cell out of the active region
of the board, because now with the blinkers, the stable pattern can have a
completely OFF line of cells at the edge. This made it necessary to decrease
the usable part of the run board by one cell on all sides.

Stableshift 1.1:
- The boat was replaced by the pond and the blinker was added. As Stephen
Silver pointed out, if you take symmetry into consideration, the pond is more
common than the boat. Also, the blinker is very common: more than any other
pattern, even the block. Note that I was so lazy that I didn't even change the
program's name, which still is "stableshift".
- I wrote a more efficient algorithm to check if the pattern re-appears, and I
dropped the MAXSHIFT constraint. Now the program checks whether the pattern
appeared anywhere on the board.

Stableshift 1.0:
- First version of the program.

If anything is not clear or you have any questions, you can contact me at:
nivasch@attglobal.net
