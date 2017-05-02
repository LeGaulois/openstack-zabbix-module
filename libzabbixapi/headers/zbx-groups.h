#ifndef ZBX_GROUPS_H_INCLUDED
#define ZBX_GROUPS_H_INCLUDED

json_object* getAllGroups(zbxinstance *zbx);
int checkGroupHostByName(zbxinstance *zbx, char *groupname);

#endif

