/*sngdetect
  Version 1.0
  (Simple and Glider with escaped-glider Detection)
  March 2000
  Gabriel Nivasch
*/

//Constants:
#define HSIZE 9
#define VSIZE 8

#define MINPLACED 1
#define MAXPLACED 10
#define MINPATWIDTH 6
#define MINPATHEIGHT 4
#define MAXPATWIDTH 7
#define MAXPATHEIGHT 5

#define BSIZE 300
#define STL 150
#define STT 150
#define MAXGENS 1000
#define ANNOUNCEX 200

#define CHKGLIDERX 1 //Should be odd. Leave it at 1.

void tryGlider(void);
void checkGlider(void);