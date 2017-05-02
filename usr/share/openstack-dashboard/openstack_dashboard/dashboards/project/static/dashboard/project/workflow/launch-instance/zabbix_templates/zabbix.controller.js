(function () {
  'use strict';

  angular
    .module('horizon.dashboard.project.workflow.launch-instance')
    .controller('LaunchInstanceZabbixTemplatesController', LaunchInstanceZabbixTemplatesController);

  LaunchInstanceZabbixTemplatesController.$inject = [
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
  function LaunchInstanceZabbixTemplatesController(launchInstanceModel, basePath) {
    var ctrl = this;

    ctrl.tableData = {
      available: launchInstanceModel.zabbixTemplates,
      allocated: launchInstanceModel.newInstanceSpec.zabbix_templates,
      displayedAvailable: [],
      displayedAllocated: []
    };

    //ctrl.tableDetails = basePath + 'zabbix/zabbix-details.html';

    ctrl.tableHelp = {
      /*eslint-disable max-len */
      noneAllocText: gettext('Select a zabbix template from the available groups below.'),
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
