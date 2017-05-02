# Presentation
Module Zabbix développé pour Openstack permettant l'ajout/suppresion automatique des instances sur ZABBIX en fonction des événements NOVA.

Le module est constitué:
    - d'une API ZABBIX (C)
    - d'un module d'écoute des files RabbitMQ (C)
    - des pages de modifications du formulaire de création d'instance sur HORIZON
    - des API pour horizon
    
# Prérequis
Le module a été testé sur la version OCATA sous CentOS.

Les librairies suivantes sont nécessaires pour pouvoir utiliser le module:
  - libconfig-devel.x86_64
  - libcurl-devel.x86_64
  - json-c-devel.x86_64
  - la librairie [rabbitMQ](https://github.com/alanxz/rabbitmq-c)
    
# Code annexe
Les ressources suivantes ont été utilisées:
  - [gestion des erreurs en C](https://openclassrooms.com/courses/la-gestion-des-erreurs-en-c)
  - [liste en C](https://github.com/clibs/list)
    
    
# Exemples
![section zabbix groups](https://github.com/LeGaulois/openstack-zabbix-module/blob/master/horizon_zabbixgroups.png)
![section zabbix templates](https://github.com/LeGaulois/openstack-zabbix-module/blob/master/horizon_zabbixtemplates.png)
