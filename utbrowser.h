#ifndef __UTBROWSER_H__
#define __UTBROWSER_H__

#define PORT "80"
#define MAX_LEN 8192

void  remove_string (char text[], int index, int rm_length);
const char *cleantext(GumboNode* node);

#endif
