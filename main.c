#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

struct termios old_settings; // save old settings

void invalid(){
    printf("System call failed.\n");
    exit(1);
}

void disable_raw_mode() {
    // restore original settings
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_settings) == -1) invalid();
    // put settings to struct //std input // thrw away everything user ttyped that havent read yet // which truct
}

void enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &old_settings) == -1) invalid(); // save in old_settingw
    atexit(disable_raw_mode);

    struct termios raw; // local; working copy
    raw = old_settings; // copy attributes

    // local flags
    /*ECHO -  keystokes appearing on the screen
    ICANON - buffer input untill enter
    ISIG - ctr + C , ctr  + Z
    IEXTEN - ctr + V */

    // input flag
    /*ICRNL - carriage return to newline automatic translation off
    IXON - disables ctr + S and ctr + Q
    BRKIN - a ts like a ctr + C break , line goes electrically flat
    INPCK - enables aparity checking sends 8th bit 1 so 1 s are always even
    ISTRIP- stripping 8th bit
     */
    // output flag
    // OPOST - terminal ewrites your output like \n to \r\n
    // control flag
    // CS8 - sets character size to 8 bits per byte

    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_iflag &= ~( BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    // special control characters
    raw.c_cc[VMIN] = 0; // VMIN - minimum bytes befroe read amy return
    raw.c_cc[VTIME] = 1;// time out in tenths of seconds
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) invalid(); // apply parametters

}

int main() {
    enable_raw_mode();
    while(1){
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1) invalid();
        if ( iscntrl(c)){ // is it control character? unprintable
            printf("%d\r\n", c);
        } else {
            printf("%d (%c)\r\n", c, c);
        }

        if (c == 'q') break;
    }
   
    disable_raw_mode();
    return 0;
}