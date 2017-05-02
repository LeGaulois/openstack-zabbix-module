#ifndef ZBX_TEMPLATES_H_INCLUDED
#define ZBX_TEMPLATES_H_INCLUDED

json_object* getAllTemplates(zbxinstance *zbx);
int checkTemplatesByName(zbxinstance *zbx, char *templatename);
int checkTemplatesByID(zbxinstance *zbx, int templateid);

#endif
