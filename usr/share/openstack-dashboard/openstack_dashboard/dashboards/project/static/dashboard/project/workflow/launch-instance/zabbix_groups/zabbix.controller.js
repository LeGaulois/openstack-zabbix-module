    function getNetworks() {
      return neutronAPI.getNetworks().then(onGetNetworks, noop).then(getPorts, noop);
    }

    function onGetNetworks(data) {
      model.neutronEnabled = true;
      model.networks.length = 0;
      if (data.data.items.length === 1) {
        model.newInstanceSpec.networks.push(data.data.items[0]);
      }
      push.apply(model.networks,
        data.data.items.filter(function(net) {
          return net.subnets.length > 0;
        }));
      return data;
    }

    function setFinalSpecNetworks(finalSpec) {
      finalSpec.nics = [];
      finalSpec.networks.forEach(function (network) {
        finalSpec.nics.push(
          {
            "net-id": network.id,
            "v4-fixed-ip": ""
          });
      });
      delete finalSpec.networks;
    }
/*
 * Copyright 2016 Symantec Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
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
    .module('horizon.dashboard.project.workflow.launch-instance')
    .controller('LaunchInstanceZabbixController', LaunchInstanceZabbixController);

  LaunchInstanceZabbixController.$inject = [
    'launchInstanceModel',
    'horizon.dashboard.project.workflow.launch-instance.basePath'
  ];

  /**
   * @ngdoc controller
   * @name LaunchInstanceZabbixController
   * @param {Object} launchInstanceModel
   * @param {string} basePath
   * @description
   * Allows selection of zabbix groups.
   * @returns {undefined} No return value
   */
  function LaunchInstanceZabbixController(launchInstanceModel, basePath) {
    var ctrl = this;

    ctrl.tableData = {
      available: launchInstanceModel.zabbixGroups,
      allocated: launchInstanceModel.newInstanceSpec.zabbix_groups,
      displayedAvailable: [],
      displayedAllocated: []
    };

    //ctrl.tableDetails = basePath + 'zabbix/zabbix-details.html';

    ctrl.tableHelp = {
      /*eslint-disable max-len */
      noneAllocText: gettext('Select a zabbix group from the available groups below.'),
      /*eslint-enable max-len */
      availHelpText: gettext('Select one')
    };

    ctrl.tableLimits = {
      maxAllocation: -1
    };

    ctrl.filterFacets = [
      {
        label: gettext('Name'),
        name: 'name',
        singleton: true
      }
    ];
  }
})();
