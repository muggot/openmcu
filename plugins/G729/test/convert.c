
#include <stdio.h>

int main () {
    
    FILE *infd, *outfd;
    short int val;
    short int zero = 0;
    char word;
    int i;
    
    infd = fopen ("encoded.bit", "r+");
    outfd = fopen ("encoded.test", "w+");
    
    while (1) {
	    if (read (fileno (infd), &val, 2) < 2) break;
	    read (fileno (infd), &val, 2);	
	    
	    word = 0;
	    for (i=0; i < 80; i++) {
		read (fileno (infd), &val, 2);	

		word = word << 1;	    

		if (val != 0x7F) {
		    word ++;
	        }
		
		if (i % 8 == 7) {
			write (fileno (outfd), &word, 1);
			word = 0;	     
		}

	    }
    }
    fclose (infd);
    fclose (outfd);
}

