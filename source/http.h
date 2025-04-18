#ifndef HTTP_H
#define HTTP_H

// Effettua una GET HTTP su URL e restituisce body (alloca *outData)
int http_get(const char *url, char **outData, int *outLen);

#endif // HTTP_H
