// GS-2019-05-29-16:16 This  is a  minimalistic Board  Support Package
// for the msp430 simulator offered my "mspdebug sim"

int putchar(int c)
{
    // we write into the simulated 
    *((char*)0x00FF)=(char)c;
    
    return c;
}
