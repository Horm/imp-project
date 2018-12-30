/**
 * JS for Web APP
 * @author Matej Horak (xhorak68) - 100%
 * @date 30.12.2018
 **/

function getDomain() {
  return 'http://' + document.domain;
}

function getTrafficBaseUrl() {
  return getDomain() + "/traffic";
}

function setTrafficGUIOn() {
  document.getElementById('traffic-status').innerText = "Traffic: On"


  document.getElementById('traffic-off').classList.remove('disabled')
  document.getElementById('traffic-on').classList.add('disabled')
}

function setTrafficGUIOff() {
  document.getElementById('traffic-status').innerText = "Traffic: Off"


  document.getElementById('traffic-on').classList.remove('disabled')
  document.getElementById('traffic-off').classList.add('disabled')
}

function trafficGuiCallback(data) {
  data = data.data;
  if (data.status) {
    setTrafficGUIOn()
  }
  else {
    setTrafficGUIOff()
  }
  success();
}

function normalGuiCallback(data) {
  data = data.data;
  if (data.redStatus) {
    document.getElementById('red-switch').classList.remove('grey')
    document.getElementById('red-switch').classList.add('red')
  }
  else {
    document.getElementById('red-switch').classList.remove('red')
    document.getElementById('red-switch').classList.add('grey')
  }
  if (data.orangeStatus) {
    document.getElementById('orange-switch').classList.remove('grey')
    document.getElementById('orange-switch').classList.add('orange')
  }
  else {
    document.getElementById('orange-switch').classList.remove('orange')
    document.getElementById('orange-switch').classList.add('grey')
  }
  if (data.greenStatus) {
    document.getElementById('green-switch').classList.remove('grey')
    document.getElementById('green-switch').classList.add('green')
  }
  else {
    document.getElementById('green-switch').classList.remove('green')
    document.getElementById('green-switch').classList.add('grey')
  }
  success();
}

function initTraffic() {
  axios.get(getTrafficBaseUrl())
      .then(trafficGuiCallback)
      .catch(error)
}

function getEffectsBaseUrl() {
  return getDomain() + '/effects'
}

function initEffects() {
  axios.get(getEffectsBaseUrl())
      .then(success)
      .catch(error)
}

function getNormalBaseUrl() {
  return getDomain() + '/normal'
}

function initNormal() {
  axios.get(getNormalBaseUrl())
      .then(normalGuiCallback)
      .catch(error)
}

(function($){
  $(function(){

    // init materialize tab
    var elem = $('.tabs')


    var options = {duration: 600, onShow: new_tab_show_callback}
    var init_tabs = M.Tabs.init(elem, options);
    var instance_tabs = M.Tabs.getInstance(elem);


    //----- new tab clicked -----
    function new_tab_show_callback (){

      if (instance_tabs.index === 1) {
        initTraffic()
      }
      else if (instance_tabs.index === 2) {
        initEffects()
      }
      else {
        initNormal()
      }
    }

    initNormal();

  }); // end of document ready
})(jQuery); // end of jQuery name space

function success() {
  M.toast({html: 'Success!'})
}

function error() {
  M.toast({html: 'Error!'})
}

function getEffectUrl(effectNumber) {
  return getEffectsBaseUrl() + "/" + effectNumber
}

function runEffect(effectNumber) {
  axios.get(getEffectUrl(effectNumber))
      .then(success)
      .catch(error)
}

function getNormalRedUrl() {
  return getNormalBaseUrl() + '/red'
}

function switchRed() {
  axios.get(getNormalRedUrl())
      .then(normalGuiCallback)
      .catch(error)
}

function getNormalOrangeUrl() {
  return getNormalBaseUrl() + '/orange'
}

function switchOrange() {
  axios.get(getNormalOrangeUrl())
      .then(normalGuiCallback)
      .catch(error)
}

function getNormalGreenUrl() {
  return getNormalBaseUrl() + '/green'
}

function switchGreen() {
  axios.get(getNormalGreenUrl())
      .then(normalGuiCallback)
      .catch(error)
}

function getTrafficOnUrl() {
  return getTrafficBaseUrl() + '/on'
}

function trafficOn() {
  axios.get(getTrafficOnUrl())
      .then(trafficGuiCallback)
      .catch(error)
}

function getTrafficOffUrl() {
  return getTrafficBaseUrl() + '/off'
}

function trafficOff() {
  axios.get(getTrafficOffUrl())
      .then(trafficGuiCallback)
      .catch(error)
}
