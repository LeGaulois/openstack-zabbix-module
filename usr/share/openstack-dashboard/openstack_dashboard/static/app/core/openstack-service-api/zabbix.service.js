/**
 * Copyright 2014, Rackspace, US, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

(function () {
  'use strict';

  angular
    .module('horizon.app.core.openstack-service-api')
    .factory('horizon.app.core.openstack-service-api.zabbix', zabbixAPI);


    zabbixAPI.$inject = [
    'horizon.framework.util.http.service',
    'horizon.framework.widgets.toast.service'
  ];
  /**
   * @ngdoc service
   * @param {Object} apiService
   * @param {Object} toastService
   * @param {Object} $window
   * @name novaApi
   * @description Provides access to Nova APIs.
   * @returns {Object} The service
   */
  function zabbixAPI(apiService, toastService) {

    var service = {
      getZabbixGroups: getZabbixGroups,
      getZabbixTemplates: getZabbixTemplates,
      setZabbixHost: setZabbixHost
    };

    return service;


    function getZabbixGroups() {
        return apiService.get('/api/zabbix/groups/')
        .error(function () {
          toastService.add('error', gettext('Unable to retrieve the zabbix groups'));
        });
    }
    
    function getZabbixTemplates() {
        return apiService.get('/api/zabbix/templates/')
        .error(function () {
          toastService.add('error', gettext('Unable to retrieve the zabbix templates'));
        });
    }
    
    function setZabbixHost(data) {
        return apiService.post('/api/zabbix/host/', data);
    }

  }
}());
