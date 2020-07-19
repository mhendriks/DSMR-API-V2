/*
***************************************************************************  
**  Program  : DSMRindex.js, part of DSMRfirmwareAPI
**  Version  : v1.2.2
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
//development//  const APIGW='http://192.168.2.229/api/';
	const APIGW='http://'+window.location.host+'/api/';

  "use strict";

  let needBootsTrapMain     = true;
  let needBootsTrapSettings = true;
  let activePage            = "mainPage";
  let activeTab             = "none";
  let presentationType      = "TAB";
  let tabTimer              = 0;
  let actualTimer           = 0;
  let timeTimer             = 0;
  var GitHubVersion         = 0;
  var GitHubVersion_dspl    = "-";
  var firmwareVersion       = 0;
  var firmwareVersion_dspl  = "-";
  var newVersionMsg         = "";
  
  var tlgrmInterval         = 10;
  var ed_tariff1            = 0;
  var ed_tariff2            = 0;
  var er_tariff1            = 0;
  var er_tariff2            = 0;
  var gd_tariff             = 0;
  var electr_netw_costs     = 0;
  var gas_netw_costs        = 0;
  var hostName            	=  "-";  
  var data       = [];
                  
  let monthType        = "ED";
  let settingBgColor   = 'deepskyblue';
  let settingFontColor = 'white'
                    
  var monthNames = [ "indxNul","Januari","Februari","Maart","April","Mei","Juni"
                    ,"Juli","Augustus","September","Oktober","November","December"
                    ,"\0"
                   ];
  
  window.onload=bootsTrapMain;
  /*
  window.onfocus = function() {
    if (needBootsTrapMain) {
      window.location.reload(true);
    }
  };
  */
    
  //============================================================================  
  function bootsTrapMain() {
    console.log("bootsTrapMain()");
    needBootsTrapMain = false;
    
    document.getElementById('bActualTab').addEventListener('click',function()
                                                {openTab('ActualTab');});
    document.getElementById('bHoursTab').addEventListener('click',function() 
                                                {openTab('HoursTab');});
    document.getElementById('bDaysTab').addEventListener('click',function() 
                                                {openTab('DaysTab');});
    document.getElementById('bMonthsTab').addEventListener('click',function() 
                                                {openTab('MonthsTab');});
    document.getElementById('bFieldsTab').addEventListener('click',function() 
                                                {openTab('FieldsTab');});
    document.getElementById('bTelegramTab').addEventListener('click',function() 
                                                {openTab('TelegramTab');});
    document.getElementById('bSysInfoTab').addEventListener('click',function() 
                                                {openTab('SysInfoTab');});
    document.getElementById('bAPIdocTab').addEventListener('click',function() 
                                                {openTab('APIdocTab');});
    document.getElementById('FSexplorer').addEventListener('click',function() 
                                                { console.log("newTab: goFSexplorer");
                                                  location.href = "/FSexplorer";
                                                });
    document.getElementById('Settings').addEventListener('click',function() 
                                                {openPage('settingsPage');});
    
    document.getElementById('mCOST').checked = false;
    setMonthTableType();
    refreshDevTime();
    getDevSettings();
    refreshDevInfo();
    
    clearInterval(timeTimer);  
    timeTimer = setInterval(refreshDevTime, 10 * 1000); // repeat every 10s

    openPage("mainPage");
    openTab("ActualTab");
    initActualGraph();
    setPresentationType('TAB');
    readGitHubVersion();
      
  } // bootsTrapMain()
  
    
  function bootsTrapSettings() {
    console.log("bootsTrapSettings()");
    needBootsTrapSettings = false;
    
    document.getElementById('bTerug').addEventListener('click',function()
                                                {openPage('mainPage');});
    document.getElementById('bEditMonths').addEventListener('click',function()
                                                {openTab('tabEditMonths');});
    document.getElementById('bEditSettings').addEventListener('click',function()
                                                {openTab('tabEditSettings');});
    document.getElementById('bUndo').addEventListener('click',function() 
                                                {undoReload();});
    document.getElementById('bSave').addEventListener('click',function() 
                                                {saveData();});
    refreshDevTime();
    refreshDevInfo();
    
    openPage("settingsPage");

    //openTab("tabEditSettings");
    
    //---- update buttons in navigation bar ---
    let x = document.getElementsByClassName("editButton");
    for (var i = 0; i < x.length; i++) {
      x[i].style.background     = settingBgColor;
      x[i].style.border         = 'none';
      x[i].style.textDecoration = 'none';  
      x[i].style.outline        = 'none';  
      x[i].style.boxShadow      = 'none';
    }

  } // bootsTrapSettings()
  

  //============================================================================  
  function openTab(tabName) {
        
    activeTab = tabName;

    clearInterval(tabTimer);  
    clearInterval(actualTimer);  
    
    let bID = "b" + tabName;
    let i;
    //---- update buttons in navigation bar ---
    let x = document.getElementsByClassName("tabButton");
    for (i = 0; i < x.length; i++) {
      x[i].style.background     = 'deepskyblue';
      x[i].style.border         = 'none';
      x[i].style.textDecoration = 'none';  
      x[i].style.outline        = 'none';  
      x[i].style.boxShadow      = 'none';
    }
    //--- hide canvas -------
    document.getElementById("dataChart").style.display = "none";
    document.getElementById("gasChart").style.display  = "none";
    //--- hide all tab's -------
    x = document.getElementsByClassName("tabName");
    for (i = 0; i < x.length; i++) {
      x[i].style.display    = "none";  
    }
    //--- and set active tab to 'block'
    console.log("now set ["+bID+"] to block ..");
    //document.getElementById(bID).style.background='lightgray';
    document.getElementById(tabName).style.display = "block";  
    if (tabName != "ActualTab") {
      clearInterval(actualTimer);
      actualTimer = setInterval(refreshSmActual, 60 * 1000);                  // repeat every 60s
    }
    
    if (tabName == "ActualTab") {
      console.log("newTab: ActualTab");
      refreshSmActual();
      clearInterval(actualTimer);
      if (tlgrmInterval < 10)
            actualTimer = setInterval(refreshSmActual, 10 * 1000);            // repeat every 10s
      else  actualTimer = setInterval(refreshSmActual, tlgrmInterval * 1000); // repeat every tlgrmInterval seconds

    } else if (tabName == "HoursTab") {
      console.log("newTab: HoursTab");
      refreshHours();
      clearInterval(tabTimer);
      tabTimer = setInterval(refreshHours, 58 * 1000); // repeat every 58s

    } else if (tabName == "DaysTab") {
      console.log("newTab: DaysTab");
      refreshDays();
      clearInterval(tabTimer);
      tabTimer = setInterval(refreshDays, 58 * 1000); // repeat every 58s

    } else if (tabName == "MonthsTab") {
      console.log("newTab: MonthsTab");
      refreshMonths();
      clearInterval(tabTimer);
      tabTimer = setInterval(refreshMonths, 118 * 1000); // repeat every 118s
    
    } else if (tabName == "SysInfoTab") {
      console.log("newTab: SysInfoTab");
      refreshDevInfo();
      clearInterval(tabTimer);
      tabTimer = setInterval(refreshDevInfo, 58 * 1000); // repeat every 58s

    } else if (tabName == "FieldsTab") {
      console.log("newTab: FieldsTab");
      refreshSmFields();
      clearInterval(tabTimer);
      tabTimer = setInterval(refreshSmFields, 58 * 1000); // repeat every 58s

    } else if (tabName == "TelegramTab") {
      console.log("newTab: TelegramTab");
      refreshSmTelegram();
      clearInterval(tabTimer); // do not repeat!

    } else if (tabName == "APIdocTab") {
      console.log("newTab: APIdocTab");
      showAPIdoc();
      
    } else if (tabName == "tabEditMonths") {
      console.log("newTab: tabEditMonths");
      document.getElementById('tabMaanden').style.display = 'block';
      getMonths();

    } else if (tabName == "tabEditSettings") {
      console.log("newTab: tabEditSettings");
      document.getElementById('tabEditSettings').style.display = 'block';
      refreshSettings();
    
    }

  } // openTab()
  
  
  //============================================================================  
  function openPage(pageName) {
        
    console.log("openPage("+pageName+")");
    activePage = pageName;
    if (pageName == "mainPage") {
      document.getElementById("settingsPage").style.display = "none";
      data = {};
      needBootsTrapSettings = true;
      openTab("ActualTab");
      if (needBootsTrapMain)       bootsTrapMain();
    }
    else if (pageName == "settingsPage") {
      document.getElementById("mainPage").style.display = "none";  
      data = {};
      needBootsTrapMain = true;
      openTab('tabEditSettings');
      if (needBootsTrapSettings)   bootsTrapSettings();
    }
    document.getElementById(pageName).style.display = "block";  

  } // openPage()
    
  
  //============================================================================  
  function refreshDevInfo()
  {
    fetch(APIGW+"v2/dev/info")
      .then(response => response.json())
      .then(json => {
        //console.log("parsed .., data is ["+ JSON.stringify(json)+"]");
        obj = json;
        var tableRef = document.getElementById('tb_info');
        //clear table
		while (tableRef.hasChildNodes()) { tableRef.removeChild(tableRef.lastChild);}
		//fill table
        for( let k in obj ) {
			var newRow=tableRef.insertRow(-1);
			var MyCell1 = newRow.insertCell(0);
			var MyCell2 = newRow.insertCell(1);
			var MyCell3 = newRow.insertCell(2);
			MyCell1.innerHTML=translateToHuman(k);
            if(obj[k] instanceof Object) {
				MyCell2.innerHTML=obj[k].value;
				MyCell3.innerHTML=obj[k].unit;
				MyCell2.style.textAlign = "right";
             
            } else { 
            	MyCell2.innerHTML=obj[k]; 
            };
        } //for loop
      
	  //new fwversion detection
	  document.getElementById('devVersion').innerHTML = obj.fwversion;
	  var tmpFW = obj.fwversion;
	  firmwareVersion_dspl = tmpFW;
	  tmpX = tmpFW.substring(1, tmpFW.indexOf(' '));
	  tmpN = tmpX.split(".");
	  firmwareVersion = tmpN[0]*10000 + tmpN[1]*1;
	  console.log("firmwareVersion["+firmwareVersion+"] >= GitHubVersion["+GitHubVersion+"]");
	  if (GitHubVersion == 0 || firmwareVersion >= GitHubVersion)
			newVersionMsg = "";
	  else  newVersionMsg = firmwareVersion_dspl + " nieuwere versie ("+GitHubVersion_dspl+") beschikbaar";
	  document.getElementById('message').innerHTML = newVersionMsg;
	  console.log(newVersionMsg);

	  tlgrmInterval = obj.telegraminterval;
  
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      });     
  } // refreshDevInfo()



  //============================================================================  
  function refreshDevTime()
  {
    //console.log("Refresh api/v2/dev/time ..");
    fetch(APIGW+"v2/dev/time")
      .then(response => response.json())
      .then(json => {
              document.getElementById('theTime').innerHTML = json.time;
              //console.log("parsed .., data is ["+ JSON.stringify(json)+"]");
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      });     
      
    document.getElementById('message').innerHTML = newVersionMsg;

  } // refreshDevTime()
  
  
  //============================================================================  
  function refreshSmActual()
  {
    fetch(APIGW+"v2/sm/actual")
      .then(response => response.json())
      .then(json => {
          //console.log("parsed .., fields is ["+ JSON.stringify(json)+"]");
          data = json;
          copyActualToChart(data);
          if (presentationType == "TAB")
                showActualTable(data);
          else  showActualGraph(data);
          //console.log("-->done..");
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      }); 
  };  // refreshSmActual()
  
  
  //============================================================================  
  function refreshSmFields()
  {
    fetch(APIGW+"v2/sm/fields")
      .then(response => response.json())
      .then(json => {
          console.log("parsed .., fields is ["+ JSON.stringify(json)+"]");
          data = json;
          for (var item in data) 
          {
          	console.log("fields item: " +item);
          	console.log("fields data[item].value: " +data[item].value);
            data[item].humanName = translateToHuman(item);
            var tableRef = document.getElementById('fieldsTable').getElementsByTagName('tbody')[0];
            if( ( document.getElementById("fieldsTable_"+item)) == null )
            {
              var newRow   = tableRef.insertRow();
              newRow.setAttribute("id", "fieldsTable_"+item, 0);
              // Insert a cell in the row at index 0
              var newCell  = newRow.insertCell(0);                  // name
              var newText  = document.createTextNode('');
              newCell.appendChild(newText);
              newCell  = newRow.insertCell(1);                      // humanName
              newCell.appendChild(newText);
              newCell  = newRow.insertCell(2);                      // value
              newCell.appendChild(newText);
              newCell  = newRow.insertCell(3);                      // unit
              newCell.appendChild(newText);
            }
            tableCells = document.getElementById("fieldsTable_"+item).cells;
            tableCells[0].innerHTML = item;
            tableCells[1].innerHTML = data[item].humanName;
            if (item == "electricity_failure_log" && data[item].value.length > 50) 
            {
              tableCells[2].innerHTML = data[item].value.substring(0,50);
              var lLine = data[item].value.substring(50);
              while (lLine.length > 50)
              {
                tableCells[2].innerHTML += "<br>" + lLine.substring(0,50);
                lLine = lLine.substring(50);
              }
              tableCells[2].innerHTML += "<br>" + lLine;
              tableCells[0].setAttribute("style", "vertical-align: top");
              tableCells[1].setAttribute("style", "vertical-align: top");
            }
            else
            {
              tableCells[2].innerHTML = data[item].value;
            }
            if (data[item].hasOwnProperty('unit'))
            {
              tableCells[2].style.textAlign = "right";              // value
              tableCells[3].style.textAlign = "center";             // unit
              tableCells[3].innerHTML = data[item].unit;
            }
          }
          //console.log("-->done..");
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      }); 
  };  // refreshSmFields()
  
  //============================================================================  
  function expandData(data)
  {
	//console.log("expandData2: data length:"+ data.data.length );
	//console.log("expandData2: actslot:"+ data.actSlot );
    var i;
    var slotbefore;
    	
    //--- first check op volgordelijkheid ------    
    if (activeTab == "HoursTab") {  
    
    }
    for (let x=data.data.length + data.actSlot; x > data.actSlot; x--)
    {	i = x % data.data.length;
        slotbefore = math.mod(i-1, data.data.length);
		//console.log("ExpandData2 - x: "+x); 
		//console.log("ExpandData2 - i: "+i);
		//console.log("ExpandData2 - slotbefore: "+slotbefore); 
		//console.log("gd_tariff: "+gd_tariff); 
		//console.log("ed_tariff: "+ed_tariff1); 
      var     costs     = 0;
      if (x != data.actSlot 	)
      { 
        data.data[i].p_ed  = ((data.data[i].values[0] + data.data[i].values[1])-(data.data[slotbefore].values[0] +data.data[slotbefore].values[1])).toFixed(3);
        data.data[i].p_edw = (data.data[i].p_ed * 1000).toFixed(0);
        data.data[i].p_er  = ((data.data[i].values[2] + data.data[i].values[3])-(data.data[slotbefore].values[2] +data.data[slotbefore].values[3])).toFixed(3);
        data.data[i].p_erw = (data.data[i].p_er * 1000).toFixed(0);
        data.data[i].p_gd  = (data.data[i].values[4]  - data.data[slotbefore].values[4]).toFixed(3);
        //-- calculate Energy Delivered costs
        costs = ( (data.data[i].values[0] - data.data[slotbefore].values[0]) * ed_tariff1 );
        costs = costs + ( (data.data[i].values[1] - data.data[slotbefore].values[1]) * ed_tariff2 );
        //-- subtract Energy Returned costs
        costs = costs - ( (data.data[i].values[2] - data.data[slotbefore].values[2]) * er_tariff1 );
        costs = costs - ( (data.data[i].values[3] - data.data[slotbefore].values[3]) * er_tariff2 );
        data.data[i].costs_e = costs;
        //-- add Gas Delivered costs
        data.data[i].costs_g = ( (data.data[i].values[4]  - data.data[slotbefore].values[4])  * gd_tariff );
        
        //-- compute network costs
        data.data[i].costs_nw = (electr_netw_costs + gas_netw_costs);
        //-- compute total costs
        data.data[i].costs_tt = ( (data.data[i].costs_e + data.data[i].costs_g + data.data[i].costs_nw) * 1.0);
      }
      else
      {
        costs             = 0;
        data.data[i].p_ed      = (data.data[i].values[0] +data.data[i].values[1]).toFixed(3);
        data.data[i].p_edw     = (data.data[i].p_ed * 1000).toFixed(0);
        data.data[i].p_er      = (data.data[i].values[2] +data.data[i].values[3]).toFixed(3);
        data.data[i].p_erw     = (data.data[i].p_er * 1000).toFixed(0);
        data.data[i].p_gd      = (data.data[i].values[4]).toFixed(3);
        data.data[i].costs_e   = 0.0;
        data.data[i].costs_g   = 0.0;
        data.data[i].costs_nw  = 0.0;
        data.data[i].costs_tt  = 0.0;
      }
    } // for i ..
    //console.log("leaving expandData() ..");
	//console.log("expandData2: eind data "+ JSON.stringify(data));
  } // expandData()
  
  //============================================================================  
  function refreshHours()
  {
    console.log("fetch("+APIGW+"v2/hist/hours)");

    fetch(APIGW+"v2/hist/hours", {"setTimeout": 2000})
      .then(response => response.json())
      .then(json => {
        //console.log(json);
        data = json;
        expandData(data);
        if (presentationType == "TAB")
              showHistTable(data, "Hours");
        else  showHistGraph(data, "Hours");
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      }); 
  } // resfreshHours()
  
  
  //============================================================================  
  function refreshDays()
  {
    console.log("fetch("+APIGW+"v2/hist/days)");
    fetch(APIGW+"v2/hist/days", {"setTimeout": 2000})
      .then(response => response.json())
      .then(json => {
        data = json;
        expandData(data);
        if (presentationType == "TAB")
              showHistTable(data, "Days");
        else  showHistGraph(data, "Days");
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      });
  } // resfreshDays()
  
  
  //============================================================================  
  function refreshMonths()
  {
    console.log("fetch("+APIGW+"v2/hist/months)");
    fetch(APIGW+"v2/hist/months", {"setTimeout": 2000})
      .then(response => response.json())
      .then(json => {
        //console.log(response);
        data = json;
        expandData(data);
        if (presentationType == "TAB")
        {
          if (document.getElementById('mCOST').checked)
                showMonthsCosts(data);
          else  showMonthsHist(data);
        }
        else  showMonthsGraph(data);
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      });
  } // resfreshMonths()

    
  //============================================================================  
  function refreshSmTelegram()
  {
    fetch(APIGW+"v2/sm/telegram")
      .then(response => response.text())
      .then(response => {
        //console.log("parsed .., data is ["+ response+"]");
    	//console.log('-------------------');
        var divT = document.getElementById('rawTelegram');
        if ( document.getElementById("TelData") == null )
        {
            console.log("CreateElement(pre)..");
            var preT = document.createElement('pre');
            preT.setAttribute("id", "TelData", 0);
            preT.setAttribute('class', 'telegram');
            preT.textContent = response;
            divT.appendChild(preT);
        }
        preT = document.getElementById("TelData");
        preT.textContent = response;
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      });     
  } // refreshSmTelegram()

  //============================================================================  
  function showActualTable(data)
  { 
    if (activeTab != "ActualTab") return;

    console.log("showActual()");

    for (var item in data) 
    {
       	//console.log("showActualTableV2 i: "+item);
    	//console.log("showActualTableV2 data[i]: "+data[item]);
    	//console.log("showActualTableV2 data[i].value: "+data[item].value);
    	
      data[item].humanName = translateToHuman(item);
      var tableRef = document.getElementById('actualTable').getElementsByTagName('tbody')[0];
      if( ( document.getElementById("actualTable_"+item)) == null )
      {
        var newRow   = tableRef.insertRow();
        newRow.setAttribute("id", "actualTable_"+item, 0);
        // Insert a cell in the row at index 0
        var newCell  = newRow.insertCell(0);            // (short)name
        var newText  = document.createTextNode('');
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(1);                // value
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(2);                // unit
        newCell.appendChild(newText);
      }
      tableCells = document.getElementById("actualTable_"+item).cells;
      tableCells[0].innerHTML = data[item].humanName;
      tableCells[1].innerHTML = data[item].value;
      if (data[item].hasOwnProperty('unit')) tableCells[2].innerHTML = data[item].unit;
      
    }

    //--- hide canvas
    document.getElementById("dataChart").style.display = "none";
    document.getElementById("gasChart").style.display  = "none";
    //--- show table
    document.getElementById("actual").style.display    = "block";

  } // showActualTable()
    
      
  //============================================================================  
  function showHistTable(data, type)
  { 
    console.log("showHistTable("+type+")");
    // the last element has the metervalue, so skip it
    var stop = data.actSlot + 1;
    var start = data.data.length + data.actSlot ;
    var index;
    	//console.log("showHistTable start: "+start);
    	//console.log("showHistTable stop: "+stop);
    
    for (let i=start; i>stop; i--)
    {  index = i % data.data.length;
		//console.log("showHistTable index: "+index);
		//console.log("showHistTable("+type+"): data["+i+"] => data["+i+"]name["+data[i].recid+"]");

      var tableRef = document.getElementById('last'+type+'Table').getElementsByTagName('tbody')[0];
      if( ( document.getElementById(type +"Table_"+type+"_R"+index)) == null )
      {
        var newRow   = tableRef.insertRow();
        //newRow.setAttribute("id", type+"Table_"+data[i].recid, 0);
        newRow.setAttribute("id", type+"Table_"+type+"_R"+index, 0);
        // Insert a cell in the row at index 0
        var newCell  = newRow.insertCell(0);
        var newText  = document.createTextNode('-');
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(1);
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(2);
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(3);
        newCell.appendChild(newText);
        if (type == "Days")
        {
          newCell  = newRow.insertCell(4);
          newCell.appendChild(newText);
        }
      }
      	
      tableCells = document.getElementById(type+"Table_"+type+"_R"+index).cells;
      tableCells[0].innerHTML = formatDate(type, data.data[index].date);
      if (data.data[index].p_edw >= 0) tableCells[1].innerHTML = data.data[index].p_edw;
      else tableCells[1].innerHTML = "-";

      if (data.data[index].p_erw >= 0) tableCells[2].innerHTML = data.data[index].p_erw;
      else tableCells[2].innerHTML = "-";

      if (data.data[index].p_gd >= 0) tableCells[3].innerHTML = data.data[index].p_gd;

      if (type == "Days")
      {
        tableCells[4].innerHTML = ( (data.data[index].costs_e + data.data[index].costs_g) * 1.0).toFixed(2);
      }
    };

    //--- hide canvas
    document.getElementById("dataChart").style.display = "none";
    document.getElementById("gasChart").style.display  = "none";
    //--- show table
    document.getElementById("lastHours").style.display = "block";
    document.getElementById("lastDays").style.display  = "block";

  } // showHistTable()

//============================================================================  
  function showMonthsHist(data)
  { 
    //console.log("now in showMonthsHist() ..");
    var start = data.data.length + data.actSlot ; //  maar 1 jaar ivm berekening jaar verschil
    var stop = start - 12;
    var i;
    var slotyearbefore = 0;
  
    for (let index=start; index>stop; index--)
    {  i = index % data.data.length;
      	slotyearbefore = math.mod(i-12,24);
      var tableRef = document.getElementById('lastMonthsTable').getElementsByTagName('tbody')[0];
      if( ( document.getElementById("lastMonthsTable_R"+i)) == null )
      {
        var newRow   = tableRef.insertRow();
        newRow.setAttribute("id", "lastMonthsTable_R"+i, 0);
        // Insert a cell in the row at index 0
        var newCell  = newRow.insertCell(0);          // maand
        var newText  = document.createTextNode('-');
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(1);              // jaar
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(2);              // verbruik
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(3);              // jaar
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(4);              // verbruik
        newCell.appendChild(newText);

        newCell  = newRow.insertCell(5);              // jaar
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(6);              // opgewekt
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(7);              // jaar
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(8);             // opgewekt
        newCell.appendChild(newText);
        
        newCell  = newRow.insertCell(9);             // jaar
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(10);             // gas
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(11);             // jaar
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(12);             // gas
        newCell.appendChild(newText);
      }
      var mmNr = parseInt(data.data[i].date.substring(2,4), 10);

      tableCells = document.getElementById("lastMonthsTable_R"+i).cells;
      //fill with default values
      for (y=2; y <= 12; y=y+2){ tableCells[y].innerHTML = "-"; }
      
      tableCells[0].innerHTML = monthNames[mmNr];                           // maand
      
      tableCells[1].innerHTML = "20"+data.data[i].date.substring(0,2);          // jaar
      if (data.data[i].p_ed >= 0) tableCells[2].innerHTML = data.data[i].p_ed;                         // verbruik
      
      tableCells[3].innerHTML = "20"+data.data[slotyearbefore].date.substring(0,2);       // jaar
      if (data.data[slotyearbefore].p_ed >= 0) tableCells[4].innerHTML = data.data[slotyearbefore].p_ed;                      // verbruik
      
      tableCells[5].innerHTML = "20"+data.data[i].date.substring(0,2);          // jaar
      if (data.data[i].p_er >= 0) tableCells[6].innerHTML = data.data[i].p_er;                         // opgewekt
      
      tableCells[7].innerHTML = "20"+data.data[slotyearbefore].date.substring(0,2);       // jaar
      if (data.data[slotyearbefore].p_er >= 0) tableCells[8].innerHTML = data.data[slotyearbefore].p_er;                      // opgewekt
      
      tableCells[9].innerHTML = "20"+data.data[i].date.substring(0,2);          // jaar
      if (data.data[i].p_gd >= 0) tableCells[10].innerHTML = data.data[i].p_gd;                        // gas
      
      tableCells[11].innerHTML = "20"+data.data[slotyearbefore].date.substring(0,2);      // jaar
      if (data.data[slotyearbefore].p_gd >= 0) tableCells[12].innerHTML = data.data[slotyearbefore].p_gd;                     // gas
      
    };
    
    //--- hide canvas
    document.getElementById("dataChart").style.display  = "none";
    document.getElementById("gasChart").style.display   = "none";
    //--- show table
    document.getElementById("lastMonths").style.display = "block";

  } // showMonthsHist()
  
  //============================================================================  
  function showMonthsCosts(data)
  { 
    console.log("now in showMonthsCosts() ..");
    var totalCost   = 0;
    var totalCost_1 = 0;
    var start = data.data.length + data.actSlot ; //  maar 1 jaar ivm berekening jaar verschil
    var stop = start - 12;
    var i;
    var slotyearbefore = 0;
  
    for (let index=start; index>stop; index--)
    {  i = index % data.data.length;
      	slotyearbefore = math.mod(i-12,24);
      //console.log("showMonthsHist(): data["+i+"] => data["+i+"].name["+data[i].recid+"]");
      var tableRef = document.getElementById('lastMonthsTableCosts').getElementsByTagName('tbody')[0];
      if( ( document.getElementById("lastMonthsTableCosts_R"+i)) == null )
      {
        var newRow   = tableRef.insertRow();
        newRow.setAttribute("id", "lastMonthsTableCosts_R"+i, 0);
        // Insert a cell in the row at index 0
        var newCell  = newRow.insertCell(0);          // maand
        var newText  = document.createTextNode('-');
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(1);              // jaar
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(2);              // kosten electra
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(3);              // kosten gas
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(4);              // vast recht
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(5);              // kosten totaal
        newCell.appendChild(newText);

        newCell  = newRow.insertCell(6);              // jaar
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(7);              // kosten electra
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(8);              // kosten gas
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(9);              // vast recht
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(10);              // kosten totaal
        newCell.appendChild(newText);
      }
      var mmNr = parseInt(data.data[i].date.substring(2,4), 10);

      tableCells = document.getElementById("lastMonthsTableCosts_R"+i).cells;
      tableCells[0].style.textAlign = "right";
      tableCells[0].innerHTML = monthNames[mmNr];                           // maand
      
      tableCells[1].style.textAlign = "center";
      tableCells[1].innerHTML = "20"+data.data[i].date.substring(0,2);          // jaar
      tableCells[2].style.textAlign = "right";
      tableCells[2].innerHTML = (data.data[i].costs_e *1).toFixed(2);            // kosten electra
      tableCells[3].style.textAlign = "right";
      tableCells[3].innerHTML = (data.data[i].costs_g *1).toFixed(2);            // kosten gas
      tableCells[4].style.textAlign = "right";
      tableCells[4].innerHTML = (data.data[i].costs_nw *1).toFixed(2);           // netw kosten
      tableCells[5].style.textAlign = "right";
      tableCells[5].style.fontWeight = 'bold';
      tableCells[5].innerHTML = "€ " + (data.data[i].costs_tt *1).toFixed(2);    // kosten totaal
      //--- omdat de actuele maand net begonnen kan zijn tellen we deze
      //--- niet mee, maar tellen we de laatste maand van de voorgaand periode
      if (i > 0)
            totalCost += data.data[i].costs_tt;
      else  totalCost += data.data[slotyearbefore].costs_tt;

      tableCells[6].style.textAlign = "center";
      tableCells[6].innerHTML = "20"+data.data[slotyearbefore].date.substring(0,2);         // jaar
      tableCells[7].style.textAlign = "right";
      tableCells[7].innerHTML = (data.data[slotyearbefore].costs_e *1).toFixed(2);           // kosten electra
      tableCells[8].style.textAlign = "right";
      tableCells[8].innerHTML = (data.data[slotyearbefore].costs_g *1).toFixed(2);           // kosten gas
      tableCells[9].style.textAlign = "right";
      tableCells[9].innerHTML = (data.data[slotyearbefore].costs_nw *1).toFixed(2);          // netw kosten
      tableCells[10].style.textAlign = "right";
      tableCells[10].style.fontWeight = 'bold';
      tableCells[10].innerHTML = "€ " + (data.data[slotyearbefore].costs_tt *1).toFixed(2);  // kosten totaal
      totalCost_1 += data.data[slotyearbefore].costs_tt;

    };

    if( ( document.getElementById("periodicCosts")) == null )
    {
      var newRow   = tableRef.insertRow();                                // voorschot regel
      newRow.setAttribute("id", "periodicCosts", 0);
      // Insert a cell in the row at index 0
      var newCell  = newRow.insertCell(0);                                // maand
      var newText  = document.createTextNode('-');
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(1);              // description
      newCell.setAttribute("colSpan", "4");
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(2);              // voorschot
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(3);              // description
      newCell.setAttribute("colSpan", "4");
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(4);              // voorschot
      newCell.appendChild(newText);
    }
    tableCells = document.getElementById("periodicCosts").cells;
    tableCells[1].style.textAlign = "right";
    tableCells[1].innerHTML = "Voorschot Bedrag"
    tableCells[2].style.textAlign = "right";
    tableCells[2].innerHTML = "€ " + (totalCost / 12).toFixed(2);
    tableCells[3].style.textAlign = "right";
    tableCells[3].innerHTML = "Voorschot Bedrag"
    tableCells[4].style.textAlign = "right";
    tableCells[4].innerHTML = "€ " + (totalCost_1 / 12).toFixed(2);

    
    //--- hide canvas
    document.getElementById("dataChart").style.display  = "none";
    document.getElementById("gasChart").style.display   = "none";
    //--- show table
    if (document.getElementById('mCOST').checked)
    {
      document.getElementById("lastMonthsTableCosts").style.display = "block";
      document.getElementById("lastMonthsTable").style.display = "none";
    }
    else
    {
      document.getElementById("lastMonthsTable").style.display = "block";
      document.getElementById("lastMonthsTableCosts").style.display = "none";
    }
    document.getElementById("lastMonths").style.display = "block";

  } // showMonthsCosts()

  
  //============================================================================  
  function getDevSettings()
  {
    fetch(APIGW+"v2/dev/settings")
      .then(response => response.json())
      .then(json => {
        console.log("getDevSettings: parsed .., data is ["+ JSON.stringify(json)+"]");
        ed_tariff1 = json.ed_tariff1.value;
        ed_tariff2 = json.ed_tariff2.value;
        er_tariff1 = json.er_tariff1.value;
        er_tariff2 = json.er_tariff2.value;
        gd_tariff = json.gd_tariff.value;

        electr_netw_costs = json.electr_netw_costs.value;
        gas_netw_costs = json.gas_netw_costs.value;
        hostName = json.hostName.value;
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      });     
  } // getDevSettings()
  
    
  //============================================================================  
  function setPresentationType(pType) {
    if (pType == "GRAPH") {
      console.log("Set presentationType to GRAPHICS mode!");
      presentationType = pType;
      document.getElementById('aGRAPH').checked = true;
      document.getElementById('aTAB').checked   = false;
      initActualGraph();
      document.getElementById('hGRAPH').checked = true;
      document.getElementById('hTAB').checked   = false;
      document.getElementById('dGRAPH').checked = true;
      document.getElementById('dTAB').checked   = false;
      document.getElementById('mGRAPH').checked = true;
      document.getElementById('mTAB').checked   = false;
      document.getElementById('mCOST').checked  = false;
      document.getElementById("lastMonthsTable").style.display      = "block";
      document.getElementById("lastMonthsTableCosts").style.display = "none";

    } else if (pType == "TAB") {
      console.log("Set presentationType to Tabular mode!");
      presentationType = pType;
      document.getElementById('aTAB').checked   = true;
      document.getElementById('aGRAPH').checked = false;
      document.getElementById('hTAB').checked   = true;
      document.getElementById('hGRAPH').checked = false;
      document.getElementById('dTAB').checked   = true;
      document.getElementById('dGRAPH').checked = false;
      document.getElementById('mTAB').checked   = true;
      document.getElementById('mGRAPH').checked = false;
      document.getElementById('mCOST').checked  = false;

    } else {
      console.log("setPresentationType to ["+pType+"] is quite shitty! Set to TAB");
      presentationType = "TAB";
    }

    document.getElementById("APIdocTab").style.display = "none";

    if (activeTab == "ActualTab")  refreshSmActual();
    if (activeTab == "HoursTab")   refreshHours();
    if (activeTab == "DaysTab")    refreshDays();
    if (activeTab == "MonthsTab")  refreshMonths();

  } // setPresenationType()
  
    
  //============================================================================  
  function setMonthTableType() {
    console.log("Set Month Table Type");
    if (presentationType == 'GRAPH') 
    {
      document.getElementById('mCOST').checked = false;
      return;
    }
    if (document.getElementById('mCOST').checked)
    {
      document.getElementById("lastMonthsTableCosts").style.display = "block";
      document.getElementById("lastMonthsTable").style.display      = "none";
    }
    else
    {
      document.getElementById("lastMonthsTable").style.display      = "block";
      document.getElementById("lastMonthsTableCosts").style.display = "none";
    }
    document.getElementById('lastMonthsTableCosts').getElementsByTagName('tbody').innerHTML = "";
    refreshMonths();
      
  } // setMonthTableType()

    
  //============================================================================  
  function showAPIdoc() {
    console.log("Show API doc ..@["+location.host+"]");
    document.getElementById("APIdocTab").style.display = "block";
    addAPIdoc("v2/dev/info",      "Device info in JSON format", true);
    addAPIdoc("v2/dev/time",      "Device time (epoch) in JSON format", true);
    addAPIdoc("v2/dev/settings",  "Device settings in JSON format", true);
    addAPIdoc("v2/dev/settings{jsonObj}", "[POST] update Device settings in JSON format\
        <br>test with:\
        <pre>curl -X POST -H \"Content-Type: application/json\" --data '{\"name\":\"mqtt_broker\",\"value\":\"hassio.local\"}' \
http://DSMR-API.local/api/v2/dev/settings</pre>", false);
    
    addAPIdoc("v2/sm/info",       "Smart Meter info in JSON format", true);
    addAPIdoc("v2/sm/actual",     "Smart Meter Actual data in JSON format", true);
    addAPIdoc("v2/sm/fields",     "Smart Meter all fields data in JSON format\
        <br>JSON format: {\"name\":[{\"value\":&lt;value&gt;,\"unit\":\"&lt;unit&gt;\"}]} ", true);
    addAPIdoc("v2/sm/fields/{fieldName}", "Smart Meter one field data in JSON format", false);

    addAPIdoc("v2/sm/telegram",   "raw telegram as send by the Smart Meter including all \"\\r\\n\" line endings", false);

    addAPIdoc("v2/hist/hours",    "History data per hour in JSON format", true);
    addAPIdoc("v2/hist/days",     "History data per day in JSON format", true);
    addAPIdoc("v2/hist/months",   "History data per month in JSON format", true);

  } // showAPIdoc()

    
  //============================================================================  
  function addAPIdoc(restAPI, description, linkURL) {
    if (document.getElementById(restAPI) == null)
    {
      var topDiv = document.getElementById("APIdocTab");
      var br = document.createElement("BR"); 
      br.setAttribute("id", restAPI, 0);
      br.setAttribute("style", "clear: left;");
      
      var div1 = document.createElement("DIV"); 
      div1.setAttribute("class", "div1", 0);
      var aTag = document.createElement('a');
      if (linkURL)
      {
        aTag.setAttribute('href',APIGW +restAPI);
        aTag.target = '_blank';
      }
      else
      {
        aTag.setAttribute('href',"#");
      }
      aTag.innerText = "/api/"+restAPI;
      aTag.style.fontWeight = 'bold';
      div1.appendChild(aTag);

      // <div class='div2'>Device time (epoch) in JSON format</div>
      var div2 = document.createElement("DIV"); 
      div2.setAttribute("class", "div2", 0);
      //var t2 = document.createTextNode(description);                   // Create a text node
      var t2 = document.createElement("p");
      t2.innerHTML = description;                   // Create a text node
      div2.appendChild(t2);     

      topDiv.appendChild(br);    // Append <br> to <div> with id="myDIV"
      topDiv.appendChild(div1);  // Append <div1> to <topDiv> with id="myDIV"
      topDiv.appendChild(div2);  // Append <div2> to <topDiviv> with id="myDIV"
    }
    
    
  } // addAPIdoc()
    
  //============================================================================  
  function refreshSettings()
  {
    console.log("refreshSettings() ..");
    data = {};
    fetch(APIGW+"v2/dev/settings")
      .then(response => response.json())
      .then(json => {
        console.log("then(json => ..)");
        data = json;
        for( let i in data )
        {
          console.log("["+i+"]=>["+data[i].value+"]");
          var settings = document.getElementById('settings');
          if( ( document.getElementById("settingR_"+i)) == null )
          {
            var rowDiv = document.createElement("div");
            rowDiv.setAttribute("class", "settingDiv");
            rowDiv.setAttribute("id", "settingR_"+i);
            //--- field Name ---
              var fldDiv = document.createElement("div");
                  fldDiv.textContent = translateToHuman(i);
                  rowDiv.appendChild(fldDiv);
            //--- input ---
              var inputDiv = document.createElement("div");
                  inputDiv.setAttribute("class", "settings-right");

                    var sInput = document.createElement("INPUT");
                    sInput.setAttribute("id", "setFld_"+i);

					switch(data[i].type){
					case "s":
						sInput.setAttribute("type", "text");
						sInput.setAttribute("maxlength", data[i].maxlen);
						break;
					case "f":
						sInput.setAttribute("type", "number");
						sInput.max = data[i].max;
						sInput.min = data[i].min;
						sInput.step = (data[i].min + data[i].max) / 1000;
						break;
					case "i":
						sInput.setAttribute("type", "number");
						sInput.max = data[i].max;
						sInput.min = data[i].min;
						sInput.step = (data[i].min + data[i].max) / 1000;
						sInput.step = 1;
						break;
					}
                    sInput.setAttribute("value", data[i].value);
                    sInput.addEventListener('change',
                                function() { setBackGround("setFld_"+i, "lightgray"); },
                                            false
                                );
                  inputDiv.appendChild(sInput);
                  
            rowDiv.appendChild(inputDiv);
            settings.appendChild(rowDiv);
          }
          else
          {
            document.getElementById("setFld_"+i).style.background = "white";
            document.getElementById("setFld_"+i).value = data[i].value;
          }
        }
        //console.log("-->done..");
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      });     

      document.getElementById('message').innerHTML = newVersionMsg;

  } // refreshSettings()
  
  
  //============================================================================  
  function getMonths()
  {
    console.log("fetch("+APIGW+"v2/hist/months)");
    fetch(APIGW+"v2/hist/months", {"setTimeout": 2000})
      .then(response => response.json())
      .then(json => {
        //console.log(response);
        data = json;
        expandDataSettings(data);
        showMonths(data, monthType);
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      });

      document.getElementById('message').innerHTML = newVersionMsg;
      
  } // getMonths()

  
  //============================================================================  
  function showMonths(data, type)
  { 
    console.log("showMonths("+type+")");
    //--- first remove all Children ----
    var allChildren = document.getElementById('editMonths');
    while (allChildren.firstChild) {
      allChildren.removeChild(allChildren.firstChild);
    }
    
    console.log("Now fill the DOM!");    
    console.log("data.data.length: "+data.data.length);

    var dlength = data.data.length;
  
    for (let index=data.actSlot + dlength; index>data.actSlot; index--)
    {  let i = index % dlength;

      //console.log("["+i+"] >>>["+data.data[i].EEYY+"-"+data.data[i].MM+"]");
      
      var em = document.getElementById('editMonths');

      if( ( document.getElementById("em_R"+i)) == null )
      {
        var div1 = document.createElement("div");
            div1.setAttribute("class", "settingDiv");
            div1.setAttribute("id", "em_R"+i);
            div1.style.borderTop = "thick solid lightblue";
            if (i == (data.data.length -1))  // last row
            {
              div1.style.borderBottom = "thick solid lightblue";
            }
            var span2 = document.createElement("span");
            span2.style.borderTop = "thick solid lightblue";
              //--- create input for EEYY
              var sInput = document.createElement("INPUT");
              sInput.setAttribute("id", "em_YY_"+i);
              sInput.setAttribute("type", "number");
              sInput.setAttribute("min", 2000);
              sInput.setAttribute("max", 2099);
              sInput.size              = 5;
              sInput.style.marginLeft  = '10px';
              sInput.style.marginRight = '20px';
              sInput.addEventListener('change',
                      function() { setNewValue(i, "EEYY", "em_YY_"+i); }, false);
              span2.appendChild(sInput);
              //--- create input for months
              var sInput = document.createElement("INPUT");
              sInput.setAttribute("id", "em_MM_"+i);
              sInput.setAttribute("type", "number");
              sInput.setAttribute("min", 1);
              sInput.setAttribute("max", 12);
              sInput.size              = 3;
              sInput.style.marginRight = '20px';
              sInput.addEventListener('change',
                      function() { setNewValue(i, "MM", "em_MM_"+i); }, false);
              span2.appendChild(sInput);
              //--- create input for data column 1
              sInput = document.createElement("INPUT");
              sInput.setAttribute("id", "em_in1_"+i);
              sInput.setAttribute("type", "number");
              sInput.setAttribute("step", 0.001);
              sInput.style.marginRight = '20px';
              
              if (type == "ED")
              {
                sInput.addEventListener('change',
                    function() { setNewValue(i, "edt1", "em_in1_"+i); }, false );
              }
              else if (type == "ER")
              {
                sInput.addEventListener('change',
                    function() { setNewValue(i, "ert1", "em_in1_"+i); }, false);
              }
              else if (type == "GD")
              {
                sInput.addEventListener('change',
                    function() { setNewValue(i, "gdt", "em_in1_"+i); }, false);
              }
              
              span2.appendChild(sInput);
              //--- if not GD create input for data column 2
              if (type == "ED")
              {
                //console.log("add input for edt2..");
                var sInput = document.createElement("INPUT");
                sInput.setAttribute("id", "em_in2_"+i);
                sInput.setAttribute("type", "number");
                sInput.setAttribute("step", 0.001);
                sInput.style.marginRight = '20px';
                sInput.addEventListener('change',
                      function() { setNewValue(i, "edt2", "em_in2_"+i); }, false);
                span2.appendChild(sInput);
              }
              else if (type == "ER")
              {
                //console.log("add input for ert2..");
                var sInput = document.createElement("INPUT");
                sInput.setAttribute("id", "em_in2_"+i);
                sInput.setAttribute("type", "number");
                sInput.setAttribute("step", 0.001);
                sInput.style.marginRight = '20px';
                sInput.addEventListener('change',
                      function() { setNewValue(i, "ert2", "em_in2_"+i); }, false);
                span2.appendChild(sInput);
              }
              div1.appendChild(span2);
              em.appendChild(div1);
      } // document.getElementById("em_R"+i)) == null 

      //--- year
      document.getElementById("em_YY_"+i).value = data.data[i].EEYY;
      document.getElementById("em_YY_"+i).style.background = "white";
      //--- month
      document.getElementById("em_MM_"+i).value = data.data[i].MM;
      document.getElementById("em_MM_"+i).style.background = "white";
      
      if (type == "ED")
      {
        document.getElementById("em_in1_"+i).style.background = "white";
        document.getElementById("em_in1_"+i).value = (data.data[i].values[0] *1).toFixed(3);
        document.getElementById("em_in2_"+i).style.background = "white";
        document.getElementById("em_in2_"+i).value = (data.data[i].values[1] *1).toFixed(3);
      }
      else if (type == "ER")
      {
        document.getElementById("em_in1_"+i).style.background = "white";
        document.getElementById("em_in1_"+i).value = (data.data[i].values[2] *1).toFixed(3);
        document.getElementById("em_in2_"+i).style.background = "white";
        document.getElementById("em_in2_"+i).value = (data.data[i].values[3] *1).toFixed(3);
      }
      else if (type == "GD")
      {
        document.getElementById("em_in1_"+i).style.background = "white";
        document.getElementById("em_in1_"+i).value = (data.data[i].values[4] *1).toFixed(3);
      }
      
    } // for all elements in data
    
    console.log("Now sequence EEYY/MM values ..(data.data.length="+dlength+")");
    //--- sequence EEYY and MM data
    var changed = false;
    for (let index=data.actSlot+dlength; index>data.actSlot; index--)
    {  let i = index % dlength;
       let next = math.mod(i-1,dlength);
    
    //for (let i=0; i<(dlength -1); i++)
    //  {
      //--- month
      if (data.data[next].MM == 0)
      {
        data.data[next].MM    = data.data[i].MM -1;
        changed = true;
        if (data.data[next].MM < 1) {
          data.data[next].MM   = 12;
          if (data.data[next].EEYY == 2000) {
            data.data[next].EEYY = data.data[i].EEYY -1;
            document.getElementById("em_YY_"+(next)).value = data.data[next].EEYY;
            //document.getElementById("em_YY_"+(i+1)).style.background = "lightgray";
          }
        }
        document.getElementById("em_MM_"+(next)).value = data.data[next].MM;
        //document.getElementById("em_MM_"+(next)).style.background = "lightgray";
      }
      if (data.data[next].EEYY == 2000) {
        data.data[next].EEYY = data.data[i].EEYY;
        changed = true;
        document.getElementById("em_YY_"+(next)).value = data.data[next].EEYY;
        //document.getElementById("em_YY_"+(i+1)).style.background = "lightgray";
      }
      if (changed) sendPostReading(next, data.data);

    } // sequence EEYY and MM

  } // showMonths()

  
  //============================================================================  
  function expandDataSettings(data)
  { 
    for (let i=0; i<data.data.length; i++)
    {
      data.data[i].EEYY = {};
      data.data[i].MM   = {};
      data.data[i].EEYY = parseInt("20"+data.data[i].date.substring(0,2));
      data.data[i].MM   = parseInt(data.data[i].date.substring(2,4));
    }
      console.log("expandDataSettings(): "+JSON.stringify(data));


  } // expandDataSettings()
  
      
  //============================================================================  
  function undoReload()
  {
    if (activeTab == "tabEditMonths") {
      console.log("getMonths");
      getMonths();
    } else if (activeTab == "tabEditSettings") {
      console.log("undoReload(): reload Settings..");
      data = {};
      refreshSettings();

    } else {
      console.log("undoReload(): I don't knwo what to do ..");
    }

  } // undoReload()
  
  
  //============================================================================  
  function saveData() 
  {
    document.getElementById('message').innerHTML = "Gegevens worden opgeslagen ..";

    if (activeTab == "tabEditSettings")
    {
      saveSettings();
    } 
    else if (activeTab == "tabEditMonths")
    {
      saveMeterReadings();
    }
    
  } // saveData()
  
  
  //============================================================================  
  function saveSettings() 
  {
    for(var i in data)
    {
      var fldId  = i;
      var newVal = document.getElementById("setFld_"+fldId).value;
      if (data[i].value != newVal)
      {
        console.log("save data ["+i+"] => from["+data[i].value+"] to["+newVal+"]");
        sendPostSetting(fldId, newVal);
      }
    }    
    // delay refresh as all fetch functions are asynchroon!!
    setTimeout(function() {
      refreshSettings();
    }, 1000);
    
  } // saveSettings()
  
  
  //============================================================================  
  function saveMeterReadings() 
  {
    console.log("Saving months-data ..");
    let changes = false;
    
    /** skip this for now **
    if (!validateReadings(monthType))
    {
      return;
    }
    **/
    
    //--- has anything changed?
    for (i in data.data)
    {
      console.log("saveMeterReadings["+i+"] ..");
      changes = false;

      if (getBackGround("em_YY_"+i) == "lightgray")
      {
        setBackGround("em_YY_"+i, "white");
        changes = true;
      }
      if (getBackGround("em_MM_"+i) == "lightgray")
      {
        setBackGround("em_MM_"+i, "white");
        changes = true;
      }

      if (document.getElementById("em_in1_"+i).style.background == 'lightgray')
      {
        changes = true;
        document.getElementById("em_in1_"+i).style.background = 'white';
      }
      if (monthType != "GD")
      {
        if (document.getElementById("em_in2_"+i).style.background == 'lightgray')
        {
          changes = true;
          document.getElementById("em_in2_"+i).style.background = 'white';
        }
      }
      if (changes) {
        console.log("Changes where made in ["+i+"]["+data.data[i].EEYY+"-"+data.data[i].MM+"]");
        //processWithTimeout([(data.length -1), 0], 2, data, sendPostReading);
        sendPostReading(i, data.data);
      }
    } 

  } // saveMeterReadings()

    
  //============================================================================  
  function sendPostSetting(field, value) 
  {
    const jsonString = {"name" : field, "value" : value};
    //console.log("send JSON:["+JSON.stringify(jsonString)+"]");
    const other_params = {
        headers : { "content-type" : "application/json; charset=UTF-8"},
        body : JSON.stringify(jsonString),
        method : "POST",
        //aangepast cors -> no-cors
        mode : "no-cors"
    };

    fetch(APIGW+"v2/dev/settings", other_params)
      .then(function(response) {
            //console.log(response.status );    //=> number 100–599
            //console.log(response.statusText); //=> String
            //console.log(response.headers);    //=> Headers
            //console.log(response.url);        //=> String
            //console.log(response.text());
            //return response.text()
      }, function(error) {
        console.log("Error["+error.message+"]"); //=> String
      });
      
  } // sendPostSetting()

    
  //============================================================================  
  function validateReadings(type) 
  {
    let withErrors = false;
    let prevMM     = 0;
    let lastBG     = "white";
        
    console.log("validate("+type+")");
    
    for (let i=0; i<(data.length -1); i++)
    {
      //--- reset background for the years
      if (getBackGround("em_YY_"+i) == "red")
      {
        setBackGround("em_YY_"+i, "lightgray");
      }
      //--- zelfde jaar, dan prevMM := (MM -1)
      if ( data[i].EEYY == data[i+1].EEYY )
      {
        prevMM = data[i].MM -1;
        //console.log("["+i+"].EEYY == ["+(i+1)+"].EEYY => ["+data[i].EEYY+"] prevMM["+prevMM+"]");
      }
      //--- jaar == volgend jaar + 1
      else if ( data[i].EEYY == (data[i+1].EEYY +1) )
      {
        prevMM = 12;
        //console.log("["+i+"].EEYY == ["+(i+1)+"].EEYY +1 => ["+data[i].EEYY+"]/["+data[i+1].EEYY+"] ("+prevMM+")");
      }
      else
      {
        setBackGround("em_YY_"+(i+1), "red");
        withErrors = true;
        prevMM = data[i].MM -1;
        //console.log("["+i+"].EEYY == ["+(i+1)+"].EEYY +1 => ["+data[i].EEYY+"]/["+data[i+1].EEYY+"] (error)");
      }
      
      //--- reset background for the months
      if (getBackGround("em_MM_"+(i+1)) == "red")
      {
        setBackGround("em_MM_"+(i+1), "lightgray");
      }
      //--- if next month != prevMM and this MM != next MM
      if (data[i+1].MM != prevMM && data[i].MM != data[i+1].MM)
      {
        setBackGround("em_MM_"+(i+1), "red");
        withErrors = true;
        //console.log("(["+(i+1)+"].MM != ["+prevMM+"].prevMM) && => ["+data[i].MM+"]/["+data[i+1].MM+"] (error)");
      }
      else
      {
        //setBackGround("em_MM_"+i, "lightgreen");
      }
      if (type == "ED")
      {
        if (getBackGround("em_in1_"+(i+1)) == "red")
        {
          setBackGround("em_in1_"+(i+1), "lightgray");
        }
        if (data[i].edt1 < data[i+1].edt1)
        {
          setBackGround("em_in1_"+(i+1), "red");
          withErrors = true;
        }
        if (getBackGround("em_in2_"+(i+1)) == "red")
        {
          setBackGround("em_in2_"+(i+1), "lightgray");
        }
        if (data[i].edt2 < data[i+1].edt2)
        {
          setBackGround("em_in2_"+(i+1), "red");
          withErrors = true;
        }
      }
      else if (type == "ER")
      {
        if (getBackGround("em_in1_"+(i+1)) == "red")
        {
          setBackGround("em_in1_"+(i+1), "lightgray");
        }
        if (data[i].ert1 < data[i+1].ert1)
        {
          setBackGround("em_in1_"+(i+1), "red");
          withErrors = true;
        }
        if (getBackGround("em_in2_"+(i+1)) == "red")
        {
          setBackGround("em_in2_"+(i+1), "lightgray");
        }
        if (data[i].ert2 < data[i+1].ert2)
        {
          setBackGround("em_in2_"+(i+1), "red");
          withErrors = true;
        }
      }
      else if (type == "GD")
      {
        if (getBackGround("em_in1_"+(i+1)) == "red")
        {
          setBackGround("em_in1_"+(i+1), "lightgray");
        }
        if (data[i].gdt < data[i+1].gdt)
        {
          setBackGround("em_in1_"+(i+1), "red");
          withErrors = true;
        }
      }
      
    }
    if (withErrors)  return false;

    return true;
    
  } // validateReadings()
  
    
  //============================================================================  
  function sendPostReading(i, row) 
  {
    console.log("sendPostReadings["+i+"]..");
    let sYY = (row[i].EEYY - 2000).toString();
    let sMM = "00";
    if ((row[i].MM *1) < 1 || (row[i].MM *1) > 12)
    {
      console.log("send: ERROR MM["+row[i].MM+"]");
      return;
    }
    if (row[i].MM < 10)
          sMM = "0"+(row[i].MM).toString();
    else  sMM = ((row[i].MM * 1)).toString();
    let sDDHH = "0101";
    let recId = sYY + sMM + sDDHH;
    console.log("send["+i+"] => ["+recId+"]");
    
    const jsonString = {"recid": recId, "edt1": row[i].values[0], "edt2": row[i].values[1],
                         "ert1": row[i].values[2],  "ert2": row[i].values[3], "gdt":  row[i].values[4] };
//	console.log ("JsonString: "+JSON.stringify(jsonString));
    const other_params = {
        headers : { "content-type" : "application/json; charset=UTF-8"},
        body : JSON.stringify(jsonString),
        method : "POST",
        mode : "cors"
    };
    
    fetch(APIGW+"v2/hist/months", other_params)
      .then(function(response) {
      }, function(error) {
        console.log("Error["+error.message+"]"); //=> String
      });

      
  } // sendPostReading()

  
  //============================================================================  
  function readGitHubVersion()
  {
    if (GitHubVersion != 0) return;
    
    fetch("https://cdn.jsdelivr.net/gh/mhendriks/DSMR-API@master/edge/DSMRversion.dat")
      .then(response => {
        if (response.ok) {
          return response.text();
        } else {
          console.log('Something went wrong');
          return "";
        }
      })
      .then(text => {
        var tmpGHF     = text.replace(/(\r\n|\n|\r)/gm, "");
        GitHubVersion_dspl = tmpGHF;
        console.log("parsed: GitHubVersion is ["+GitHubVersion_dspl+"]");
        tmpX = tmpGHF.substring(1, tmpGHF.indexOf(' '));
        tmpN = tmpX.split(".");
        GitHubVersion = tmpN[0]*10000 + tmpN[1]*1;
        
        console.log("firmwareVersion["+firmwareVersion+"] >= GitHubVersion["+GitHubVersion+"]");
        if (firmwareVersion == 0 || firmwareVersion >= GitHubVersion)
              newVersionMsg = "";
        else  newVersionMsg = firmwareVersion_dspl + " nieuwere versie ("+GitHubVersion_dspl+") beschikbaar";
        document.getElementById('message').innerHTML = newVersionMsg;
        console.log(newVersionMsg);

      })
      .catch(function(error) {
        console.log(error);
        GitHubVersion_dspl   = "";
        GitHubVersion        = 0;
      });     

  } // readGitHubVersion()

    
  //============================================================================  
  function setEditType(eType) {
    if (eType == "ED") {
      console.log("Edit Energy Delivered!");
      monthType = eType;
      getMonths()
      showMonths(data, monthType);
    } else if (eType == "ER") {
      console.log("Edit Energy Returned!");
      monthType = eType;
      getMonths()
      showMonths(data, monthType);
    } else if (eType == "GD") {
      console.log("Edit Gas Delivered!");
      monthType = eType;
      getMonths()
      showMonths(data, monthType);
    } else {
      console.log("setEditType to ["+eType+"] is quit shitty!");
      monthType = "";
    }

  } // setEditType()

   
  //============================================================================  
  function setNewValue(i, dField, field) {
    document.getElementById(field).style.background = "lightgray";
    //--- this is ugly!!!! but don't know how to do it better ---
    if (dField == "EEYY")       data.data[i].EEYY = document.getElementById(field).value;
    else if (dField == "MM")    data.data[i].MM   = document.getElementById(field).value;
    else if (dField == "edt1")  data.data[i].values[0] = document.getElementById(field).value;
    else if (dField == "edt2")  data.data[i].values[1] = document.getElementById(field).value;
    else if (dField == "ert1")  data.data[i].values[2] = document.getElementById(field).value;
    else if (dField == "ert2")  data.data[i].values[3] = document.getElementById(field).value;
    else if (dField == "gdt")   data.data[i].values[4]  = document.getElementById(field).value;
    
  } // setNewValue()

   
  //============================================================================  
  function setBackGround(field, newColor) {
    document.getElementById(field).style.background = newColor;
    
  } // setBackGround()

   
  //============================================================================  
  function getBackGround(field) {
    return document.getElementById(field).style.background;
    
  } // getBackGround()

  
  //============================================================================  
  function validateNumber(field) {
    console.log("validateNumber(): ["+field+"]");
    if (field == "EDT1" || field == "EDT2" || field == "ERT1" || field == "ERT2" || field == "GAS") {
      var pattern = /^\d{1,1}(\.\d{1,5})?$/;
      var max = 1.99999;
    } else {
      var pattern = /^\d{1,2}(\.\d{1,2})?$/;
      var max = 99.99;
    }
    var newVal = document.getElementById(field).value;
    newVal = newVal.replace( /[^0-9.]/g, '' );
    if (!pattern.test(newVal)) {
      document.getElementById(field).style.color = 'orange';
      console.log("wrong format");
    } else {
      document.getElementById(field).style.color = settingFontColor;
      console.log("valid number!");
    }
    if (newVal > max) {
      console.log("Number to big!");
      document.getElementById(field).style.color = 'orange';
      newVal = max;
    }
    document.getElementById(field).value = newVal * 1;
    
  } // validateNumber()

  
  //============================================================================  
  function translateToHuman(longName) {
    //for(var index = 0; index < (translateFields.length -1); index++) 
    for(var index = 0; index < translateFields.length; index++) 
    {
        if (translateFields[index][0] == longName)
        {
          return translateFields[index][1];
        }
    };
    return longName;
    
  } // translateToHuman()

  
  //============================================================================  
  function formatDate(type, dateIn) 
  {
    let dateOut = "";
    if (type == "Hours")
    {
      //date = "20"+dateIn.substring(0,2)+"-"+dateIn.substring(2,4)+"-"+dateIn.substring(4,6)+" ["+dateIn.substring(6,8)+"]";
      dateOut = "("+dateIn.substring(4,6)+") ["+dateIn.substring(6,8)+":00 - "+dateIn.substring(6,8)+":59]";
    }
    else if (type == "Days")
      dateOut = recidToWeekday(dateIn)+" "+dateIn.substring(4,6)+"-"+dateIn.substring(2,4)+"-20"+dateIn.substring(0,2);
    else if (type == "Months")
      dateOut = "20"+dateIn.substring(0,2)+"-["+dateIn.substring(2,4)+"]-"+dateIn.substring(4,6)+":"+dateIn.substring(6,8);
    else
      dateOut = "20"+dateIn.substring(0,2)+"-"+dateIn.substring(2,4)+"-"+dateIn.substring(4,6)+":"+dateIn.substring(6,8);
    return dateOut;
  }

  
  //============================================================================  
  function recidToEpoch(dateIn) 
  {
    var YY = "20"+dateIn.substring(0,2);
    console.log("["+YY+"]["+(dateIn.substring(2,4)-1)+"]["+dateIn.substring(4,6)+"]");
    //-------------------YY-------------------(MM-1)----------------------DD---------------------HH--MM--SS
    var epoch = Date.UTC(YY, (dateIn.substring(2,4)-1), dateIn.substring(4,6), dateIn.substring(6,8), 1, 1);
    //console.log("epoch is ["+epoch+"]");

    return epoch;
    
  } // recidToEpoch()
  
  
  //============================================================================  
  function recidToWeekday(dateIn)
  {
    var YY = "20"+dateIn.substring(0,2);
    //-------------------YY-------------------(MM-1)----------------------DD---------------------HH--MM--SS
    var dt = new Date(Date.UTC(YY, (dateIn.substring(2,4)-1), dateIn.substring(4,6), 1, 1, 1));

    return dt.toLocaleDateString('nl-NL', {weekday: 'long'});
    
  } // epochToWeekday()
  
    
  //============================================================================  
  function round(value, precision) 
  {
    var multiplier = Math.pow(10, precision || 0);
    return Math.round(value * multiplier) / multiplier;
  }
    
  var translateFields = [
           [ "author",                    "Auteur" ]
          ,[ "identification",            "Slimme Meter ID" ]
          ,[ "p1_version",                "P1 Versie" ]
          ,[ "energy_delivered_tariff1",  "Energie Gebruikt tarief 1" ]
          ,[ "energy_delivered_tariff2",  "Energie Gebruikt tarief 2" ]
          ,[ "energy_returned_tariff1",   "Energie Opgewekt tarief 1" ]
          ,[ "energy_returned_tariff2",   "Energie Opgewekt tarief 2" ]
          ,[ "electricity_tariff",        "Electriciteit tarief" ]
          ,[ "power_delivered",           "Vermogen Gebruikt" ]
          ,[ "power_returned",            "Vermogen Opgewekt" ]
          ,[ "electricity_threshold",     "Electricity Threshold" ]
          ,[ "electricity_switch_position","Electricity Switch Position" ]
          ,[ "electricity_failures",      "Electricity Failures" ]
          ,[ "electricity_long_failures", "Electricity Long Failures" ]
          ,[ "electricity_failure_log",   "Electricity Failure log" ]
          ,[ "electricity_sags_l1",       "Electricity Sags l1" ]
          ,[ "electricity_sags_l2",       "Electricity Sags l2" ]
          ,[ "electricity_sags_l3",       "Electricity Sags l3" ]
          ,[ "electricity_swells_l1",     "Electricity Swells l1" ]
          ,[ "electricity_swells_l2",     "Electricity Swells l2" ]
          ,[ "electricity_swells_l3",     "Electricity Swells l3" ]
          ,[ "message_short",             "Korte Boodschap" ]
          ,[ "message_long",              "Lange Boodschap" ]
          ,[ "voltage_l1",                "Voltage l1" ]
          ,[ "voltage_l2",                "Voltage l2" ]
          ,[ "voltage_l3",                "Voltage l3" ]
          ,[ "current_l1",                "Current l1" ]
          ,[ "current_l2",                "Current l2" ]
          ,[ "current_l3",                "Current l3" ]
          ,[ "power_delivered_l1",        "Vermogen Gebruikt l1" ]
          ,[ "power_delivered_l2",        "Vermogen Gebruikt l2" ]
          ,[ "power_delivered_l3",        "Vermogen Gebruikt l3" ]
          ,[ "power_returned_l1",         "Vermogen Opgewekt l1" ]
          ,[ "power_returned_l2",         "Vermogen Opgewekt l2" ]
          ,[ "power_returned_l3",         "Vermogen Opgewekt l3" ]
          ,[ "gas_device_type",           "Gas Device Type" ]
          ,[ "gas_equipment_id",          "Gas Equipment ID" ]
          ,[ "gas_valve_position",        "Gas Klep Positie" ]
          ,[ "gas_delivered",             "Gas Gebruikt" ]
          ,[ "thermal_device_type",       "Thermal Device Type" ]
          ,[ "thermal_equipment_id",      "Thermal Equipment ID" ]
          ,[ "thermal_valve_position",    "Thermal Klep Positie" ]
          ,[ "thermal_delivered",         "Thermal Gebruikt" ]
          ,[ "water_device_type" ,        "Water Device Type" ]
          ,[ "water_equipment_id",        "Water Equipment ID" ]
          ,[ "water_valve_position",      "Water Klep Positie" ]
          ,[ "water_delivered",           "Water Gebruikt" ]
          ,[ "slave_device_type",         "Slave Device Type" ]
          ,[ "slave_equipment_id",        "Slave Equipment ID" ]
          ,[ "slave_valve_position",      "Slave Klep Positie" ]
          ,[ "slave_delivered",           "Slave Gebruikt" ]
          ,[ "ed_tariff1",                "Energy Verbruik Tarief-1/kWh" ]
          ,[ "ed_tariff2",                "Energy Verbruik Tarief-2/kWh" ]
          ,[ "er_tariff1",                "Energy Opgewekt Tarief-1/kWh" ]
          ,[ "er_tariff2",                "Energy Opgewekt Tarief-2/kWh" ]
          ,[ "gd_tariff" ,                "Gas Verbruik Tarief/m3" ]
          ,[ "electr_netw_costs",         "Netwerkkosten Energie/maand" ]
          ,[ "gas_netw_costs",            "Netwerkkosten Gas/maand" ]
          
          ,[ "smhasfaseinfo",             "SM Has Fase Info (0=No, 1=Yes)" ]
          ,[ "sm_has_fase_info",          "SM Has Fase Info (0=No, 1=Yes)" ]
          ,[ "oled_type",                 "OLED type (0=None, 1=SDD1306, 2=SH1106)" ]
          ,[ "oled_flip_screen",          "Flip OLED scherm (0=No, 1=Yes)" ]
          ,[ "tlgrm_interval",            "Telegram Lees Interval (Sec.)" ]
          ,[ "telegraminterval",          "Telegram Lees Interval (Sec.)" ]
          ,[ "index_page",                "Te Gebruiken index.html Pagina" ]
          ,[ "oled_screen_time",          "Oled Screen Time (Min., 0=infinite)" ]
          ,[ "mqttbroker",                "MQTT Broker IP/URL" ]
          ,[ "mqtt_broker",               "MQTT Broker IP/URL" ]
          ,[ "mqttbrokerport",            "MQTT Broker Poort" ]
          ,[ "mqtt_broker_port",          "MQTT Broker Poort" ]
          ,[ "mqttuser",                  "MQTT Gebruiker" ]
          ,[ "mqtt_user",                 "MQTT Gebruiker" ]
          ,[ "mqttpasswd",                "Password MQTT Gebruiker" ]
          ,[ "mqtt_passwd",               "Password MQTT Gebruiker" ]
          ,[ "mqtttoptopic",              "MQTT Top Topic" ]
          ,[ "mqtt_toptopic",             "MQTT Top Topic" ]
          ,[ "mqttinterval",              "Verzend MQTT Berichten (Sec.)" ]
          ,[ "mqtt_interval",             "Verzend MQTT Berichten (Sec.)" ]
          ,[ "mqttbroker_connected",      "MQTT broker connected" ]
          ,[ "mindergas_token",           "Mindergas Token" ]
          ,[ "mindergas_response",        "Mindergas Terugkoppeling" ]
          ,[ "mindergas_status",          "Mindergas Status (@dag | tijd)" ]

          ,[ "telegramcount",             "Telegrammen verwerkt" ]
          ,[ "telegramerrors",            "Telegrammen met fouten" ]          
          ,[ "fwversion",                 "Firmware Versie" ]
          ,[ "compiled",                  "Gecompileerd" ]
          ,[ "hostname",                  "HostName" ]
          ,[ "ipaddress",                 "IP adres" ]
          ,[ "macaddress",                "MAC adres" ]
          ,[ "indexfile",                 "Te Gebruiken index.html Pagina" ]
          ,[ "freeheap",                  "Free Heap Space" ]
          ,[ "maxfreeblock",              "Max. Free Heap Blok" ]
          ,[ "chipid",                    "Chip ID" ]
          ,[ "coreversion",               "ESP8266 Core Versie" ]
          ,[ "sdkversion",                "SDK versie" ]
          ,[ "cpufreq",                   "CPU Frequency" ]
          ,[ "sketchsize",                "Sketch Size" ]
          ,[ "freesketchspace",           "Free Sketch Space" ]
          ,[ "flashchipid",               "Flash Chip ID" ]
          ,[ "flashchipsize",             "Flash Chip Size" ]
          ,[ "flashchiprealsize",         "Flash Chip Real Size" ]
          ,[ "spiffssize",                "SPIFFS Size" ]
          ,[ "flashchipspeed",            "Flash Chip Speed" ]
          ,[ "flashchipmode",             "Flash Chip Mode" ]
          ,[ "boardtype",                 "Board Type" ]
          ,[ "compileoptions",            "Compiler Opties" ]
          ,[ "ssid",                      "WiFi SSID" ]
          ,[ "wifirssi",                  "WiFi RSSI" ]
          ,[ "uptime",                    "Up Time [dagen] - [hh:mm]" ]
          ,[ "reboots",                   "Aantal keer opnieuw opgestart" ]
          ,[ "lastreset",                 "Laatste Reset reden" ]
          
                        ];

/*
***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************
*/
