#ifndef SENDER_H_INCLUDED
#define SENDER_H_INCLUDED

void prepareJsonData(json_object *param,
    const char *methode, zbxinstance *zbx, curlrequest **cr);
int sendPost(zbxinstance *zbx, curlrequest *r);

#endif
