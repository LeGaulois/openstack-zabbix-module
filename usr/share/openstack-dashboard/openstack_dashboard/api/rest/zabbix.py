from __future__ import absolute_import

#Dependances Flo
import pika
import random
import json
import sys

#Dependances Openstack

import logging
from collections import OrderedDict

from django.http import HttpResponse
from django.template.defaultfilters import slugify
from django.utils import http as utils_http
from django.utils.translation import ugettext_lazy as _
from django.views import generic

from horizon import exceptions as hz_exceptions

from novaclient import exceptions

from openstack_dashboard import api
from openstack_dashboard.api.rest import json_encoder
from openstack_dashboard.api.rest import urls
from openstack_dashboard.api.rest import utils as rest_utils
from openstack_dashboard.usage import quotas


#Types de requetes
ZABBIX_LIST_GROUPS    = 1
ZABBIX_LIST_TEMPLATES = 2
ZABBIX_CREATE_HOST    = 3

#Types de message
ZABBIX_REQUEST        = 0
ZABBIX_RESPONSE       = 1

RESPONSE = None


LOG = logging.getLogger(__name__)


def get_message_type(x):
    """
    Alternative au switch/case
    Renvoie le type de demande
    sous forme de string
    """
    return {
        1:    "zabbix.list.groups",
        2:    "zabbix.list.templates",
        3:    "zabbix.create.host"
    }.get(x)
    
def callback(ch, method, properties, body):
    """
    Cette fonction sera appele pour tout message
    AMQP capte
    On verifie:
        * le type de message (request/response)
        * ID du message (couple request/response)
    """
    response = json.loads(body)
    msg_id = int(method.consumer_tag)
    
    if((response["message_type"]=="zabbix.response") and (response["id"]==msg_id)):
        global RESPONSE 
        RESPONSE = response["payload"]
        ch.basic_cancel(consumer_tag=str(msg_id))


def requestToZabbixInfos(host="localhost", exchange="amq.fanout",
        routingkey="zabbix", reqtype=1, args={}):
    """
    Fonction pour envoyer des requetes au module zabbix
    Elle renvoie -1 le payload de la reponse
    """
    
    connection = pika.BlockingConnection(pika.ConnectionParameters(host))
    channel    = connection.channel()
    msg_id     = long(random.getrandbits(50))
    
    tab = exchange.split(".")
    if (len(tab)==2):
        exch = tab[0]
        typ  = tab[1]
    else:
        exch = exchange
        typ  = "fanout"
    
    channel.exchange_declare(exchange=exch, type=typ)

    #Zone envoie
    msg = {
        "message_type" : "zabbix.request",
        "event_type"   : get_message_type(reqtype),
        "id"           : msg_id
    }
    
    if args:
        msg["payload"] = args
    
    channel.basic_publish(exchange=exchange,
                          routing_key=routingkey,
                          body=json.dumps(msg))

    
    #Zone reception
    result = channel.queue_declare(exclusive=False)
    queue_name = result.method.queue
    channel.queue_bind(exchange=exchange,
                   queue=queue_name)
                   
    channel.basic_consume(callback, consumer_tag=str(msg_id),queue=queue_name,no_ack=True,arguments={"id":msg_id})
    channel.start_consuming()
    global RESPONSE    
    connection.close()
    return RESPONSE
    
    
@urls.register
class ZabbixHost(generic.View):
    """
    API over ZABBIX Hosts
    """
    url_regex = r'zabbix/host/$'

    _zbx_groups_args={'name','groupid','templateid'}
    
    
    @rest_utils.ajax(data_required=True)
    def post(self, request):
        """
        try:
            args = (
                request,
                request.DATA['hostname'],
                request.DATA['groupsname']
            )
        except KeyError as e:
            raise rest_utils.AjaxError(400, 'missing required parameter '
                                            "'%s'" % e.args[0])
        """
        kw = {}
        LOG.error("ADD host (POST): %s", request.DATA)
        for name in self._zbx_groups_args:
            if name in request.DATA:
                kw[name] = request.DATA[name]

        requestToZabbixInfos(reqtype=3,args=kw)
        
        return rest_utils.CreatedResponse('/api/zabbix/host/')


@urls.register
class ZabbixGroups(generic.View):
    """
    API over ZABBIX Groups
    """
    url_regex = r'zabbix/groups/$'

    _zbx_groups_args={'groupid','name'}
    
    @rest_utils.ajax()
    def get(self, request):
        """Get a list of groups.

        The listing result is an object with property "items". Each item is
        a group.

        Example GET:
        http://localhost/api/zabbix/groups
        """
        groups= requestToZabbixInfos()
        return {'items':RESPONSE}
        
@urls.register
class ZabbixTemplates(generic.View):
    """
    API over ZABBIX Templates
    """
    url_regex = r'zabbix/templates/$'

    _zbx_groups_args={'templateid','name'}
    
    @rest_utils.ajax()
    def get(self, request):
        """Get a list of templates.

        The listing result is an object with property "items". Each item is
        a template.

        Example GET:
        http://localhost/api/zabbix/templates
        """
        templates= requestToZabbixInfos(reqtype=2)
        return {'items':templates}
