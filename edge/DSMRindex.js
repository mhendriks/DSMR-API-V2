/*
***************************************************************************  
**  Program  : DSMRindex.js, part of DSMRfirmwareAPI
**  Version  : v3.0.0
**
**  Copyright (c) 2021 Martijn Hendriks / based on DSMR Api Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
  const APIGW='http://'+window.location.host+'/api/';

  "use strict";

  let activeTab             = "bDashTab";
  let PauseAPI				= false; //pause api call when browser is inactive
  let presentationType      = "TAB";
  let tabTimer              = 0;
  let actualTimer           = 0;
  let timeTimer             = 0;
  var GitHubVersion         = 0;
  var GitHubVersion_dspl    = "-";
  var devVersion			= 0;
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
  var hostName            	= "-";  
  var data       			= [];
  var DayEpoch				= 0;
  let monthType        		= "ED";
  let settingFontColor 		= 'white'
                    
  var monthNames 			= [ "indxNul","Januari","Februari","Maart","April","Mei","Juni","Juli","Augustus","September","Oktober","November","December","\0"];
  var FS_no_delete 			= [ "FSexplorer.html", "DSMRindex.html","DSMRsettings.json","DSMRindexEDGE.html","\0"];
  const spinner 			= document.getElementById("loader");

//---- frontend settings
  var AMPS					= 35 	//waarde zekering (meestal 35 of 25 ampere)
  var ShowVoltage			= true 	//toon spannningsverloop op dashboard
  var UseCDN				= true	//ophalen van hulpbestanden lokaal of via cdn //nog niet geimplementeerd
  var Injection				= false	//teruglevering energie false = bepaalt door de data uit slimme meter, true = altijd aan
  var Phases				= 1		//aantal fases (1,2,3) voor de berekening van de totale maximale stroom Imax = fases * AMPS
  var HeeftGas				= false	//gasmeter aanwezig. default=false => door de slimme meter te bepalen -> true door Frontend.json = altijd aan
  var HeeftWater			= false	//watermeter aanwezig. default=false => door de slimme meter te bepalen -> true door Frontend.json = altijd aan
// ---- DASH
// var MaxAmps = 0.0;
var TotalAmps=0.0,minKW = 0.0, maxKW = 0.0,minV = 0.0, maxV = 0.0, Pmax,Gmax, Wmax;
var hist_arrW=[4], hist_arrG=[4], hist_arrPa=[4], hist_arrPi=[4], hist_arrP=[4]; //berekening verbruik
var day = 0;

let GaugeOptions = {
		id: "gauge", // the id of the html element
		value: 0,
		min: 0,
		max: 25,
		decimals: 0,
		gaugeWidthScale: 0.6,
		pointer: true,
		pointerOptions: {
			toplength: -15,
			bottomlength: 12,
			bottomwidth: 12,
			color: '#8e8e93',
			stroke: '#ffffff',
			stroke_width: 3,
			stroke_linecap: 'round'
		}, counter: true,
		customSectors: {
			percents: true, // lo and hi values are in %
			ranges: [{
				color : "#43bf58",
				lo : 0,
				hi : 33
				},
				{
				color : "#fff130",
				lo : 34,
				hi : 75
				},
				{
				color : "#ff3b30",
				lo : 76,
				hi : 100
				}]
		}, 
		label: "Ampere" };

let GaugeOptionsV = {
		id: "gauge-v", // the id of the html element
		value: 230,
		min: 207,
		max: 253,
		decimals: 0,
		gaugeWidthScale: 0.6,
		pointer: true,
		pointerOptions: {
			toplength: -15,
			bottomlength: 12,
			bottomwidth: 12,
			color: '#8e8e93',
			stroke: '#ffffff',
			stroke_width: 3,
			stroke_linecap: 'round'
		}, counter: true,
		customSectors: {
			percents: true, // lo and hi values are in %
			ranges: [{
				color : "#ff3b30",
				lo : 0,
				hi : 10
				},
				{
				color : "#43bf58",
				lo : 11,
				hi : 90
				},
				{
				color : "#ff3b30",
				lo : 91,
				hi : 100
				}]
		}, 
		label: "Volt" };

let TrendG = {
    type: 'doughnut',
    data: {
      labels: ["verbruik", "verschil met hoogste"],
      datasets: [
        {
          label: "vandaag",
          backgroundColor: ["#314b77", "rgba(0,0,0,0.1)"],
        },
        {
          label: "gisteren",
          backgroundColor: ["#316b77", "rgba(0,0,0,0.1)"],
        },
        {
          label: "eergisteren",
          backgroundColor: ["#318b77", "rgba(0,0,0,0.1)"],
        }
      ]
    },
    options: {
    title: {
            display: true,
            text: 'm3',
            position: "bottom",
            padding: -18,
            fontSize: 17,
            fontColor: "#000",
            fontFamily:"Dosis",
        },
      responsive:true,
      circumference: Math.PI,
			rotation: -Math.PI,
      plugins: {
      labels: {
        render: function (args) {
          return args.value + " \u33A5";
        },//render
        arc: true,
        fontColor: ["#fff","rgba(0,0,0,0)"],
      },//labels      
    }, //plugins
    legend: {display: false},
    }, //options
};

let TrendW = {
    type: 'doughnut',
    data: {
      labels: ["verbruik", "verschil met hoogste"],
      datasets: [
        {
          label: "vandaag",
          backgroundColor: ["#314b77", "rgba(0,0,0,0.1)"],
        },
        {
          label: "gisteren",
          backgroundColor: ["#316b77", "rgba(0,0,0,0.1)"],
        },
        {
          label: "eergisteren",
          backgroundColor: ["#318b77", "rgba(0,0,0,0.1)"],
        }
      ]
    },
    options: {
    title: {
            display: true,
            text: 'liter',
            position: "bottom",
            padding: -18,
            fontSize: 17,
            fontColor: "#000",
            fontFamily:"Dosis",
        },
      responsive:true,
      circumference: Math.PI,
			rotation: -Math.PI,
      plugins: {
      labels: {
        render: function (args) {
          return args.value + " ltr";
        },//render
        arc: true,
        fontColor: ["#fff","rgba(0,0,0,0)"],
      },//labels      
    }, //plugins
    legend: {display: false},
    }, //options
};

let optionsP = {
title: {
            display: true,
            text: 'kWh',
            position: "bottom",
            padding: -18,
            fontSize: 17,
            fontColor: "#000",
            fontFamily:"Dosis",
        },
      responsive:true,
      circumference: Math.PI,
		rotation:  - Math.PI,
      plugins: {
      labels: {
        render: function (args) {
          return args.value + " kWh";
        },//render
        arc: true,
        fontColor: ["#fff","rgba(0,0,0,0)"],
      },//labels      
    }, //plugins
      legend: {display: false},
}; 
    
let dataP = {
      labels: ["verbruik", "verschil met hoogste"],
      datasets: [
        {
          label: "vandaag",
          backgroundColor: ["#314b77", "rgba(0,0,0,0.1)"],
        },
        {
          label: "gisteren",
          backgroundColor: ["#316b77", "rgba(0,0,0,0.1)"],
        },
        {
          label: "eergisteren",
          backgroundColor: ["#318b77", "rgba(0,0,0,0.1)"],
        }
      ]
};

let dataPi = {
      labels: ["verbruik", "verschil met hoogste"],
      datasets: [
        {
          label: "vandaag",
          backgroundColor: ["#314b77", "rgba(0,0,0,0.1)"],
        },
        {
          label: "gisteren",
          backgroundColor: ["#316b77", "rgba(0,0,0,0.1)"],
        },
        {
          label: "eergisteren",
          backgroundColor: ["#318b77", "rgba(0,0,0,0.1)"],
        }
      ]
	};

let dataPa = {
      labels: ["verbruik", "verschil met hoogste"],
      datasets: [
        {
          label: "vandaag",
          backgroundColor: ["#314b77", "rgba(0,0,0,0.1)"],
        },
        {
          label: "gisteren",
          backgroundColor: ["#316b77", "rgba(0,0,0,0.1)"],
        },
        {
          label: "eergisteren",
          backgroundColor: ["#318b77", "rgba(0,0,0,0.1)"],
        }
      ]
    };

  window.onload=bootsTrapMain;
  
  // Handle page visibility change events
function visibilityListener() {
  switch(document.visibilityState) {
    case "hidden":
//       console.log("visibilityState: hidden");
    clearInterval(tabTimer);  
    clearInterval(actualTimer);
	clearInterval(timeTimer);  
      PauseAPI=true;
      break;
    case "visible":
//       console.log("visibilityState: visable");
	  PauseAPI=false;
	  timeTimer = setInterval(refreshDevTime, 10 * 1000); // repeat every 10s
	  refreshDevTime();
	  openTab();
      break;
  }
	//alert_message("Connectie met Dongle stond op pauze...nieuwe data ophalen");
}

document.addEventListener("visibilitychange", visibilityListener);

//============================================================================  
  
function UpdateDash()
{	
	// if (PauseAPI) return;
/*** 
	Het dashboard toont verschillende kolommen, namelijk
	- actueel	: actueel verbruik/teruglevering (altijd)
	- totaal 	: afname - injectie(teruglevering) (altijd)
	- afname*3 	: onttrokken van stroomnet
	- injectie*3: teruglevering aan stroomnet
	- gas*2 	: indien er een gasmeter aanwezig is
	- water     : indien watermeter aanwezig 
	- spanning* : spanningsniveau + aantal fases
	

	*3 = alleen getoond indien energy_returned_tariff1 > 0
	* = alleen indien geen teruglevering
*/
	var Parr=[3],Parra=[3],Parri=[3], Garr=[3],Warr=[3];
	console.log("Update dash");
	
	setPresentationType('TAB'); //zet grafische mode uit
	
	//check new day = refresh
	var DayEpochTemp = Math.floor(new Date().getTime() / 86400000.0);
	if (DayEpoch != DayEpochTemp || hist_arrP.length < 4 ) {
		refreshDays(); //load data first visit
		DayEpoch = DayEpochTemp;
	}
	Spinner(true);
	fetch(APIGW+"v2/sm/fields", {"setTimeout": 5000})
	  .then(response => response.json())
	  .then(json => {
//  	  json = JSON.parse('{"timestamp":{"value":"210417094333S"},"energy_delivered_tariff1":{"value":40,"unit":"kWh"},"energy_delivered_tariff2":{"value":40,"unit":"kWh"},"energy_returned_tariff1":{"value":48,"unit":"kWh"},"energy_returned_tariff2":{"value":0,"unit":"kWh"},"power_delivered":{"value":2.015,"unit":"kW"},"power_returned":{"value":1223,"unit":"kW"},"voltage_l1":{"value":227.7,"unit":"V"},"voltage_l2":{"value":224.2,"unit":"V"},"voltage_l3":{"value":226.8,"unit":"V"},"current_l1":{"value":2,"unit":"A"},"current_l2":{"value":6,"unit":"A"},"current_l3":{"value":1,"unit":"A"},"power_delivered_l1":{"value":0.388,"unit":"kW"},"power_delivered_l2":{"value":1.363,"unit":"kW"},"power_delivered_l3":{"value":0.258,"unit":"kW"},"power_returned_l1":{"value":1,"unit":"kW"},"power_returned_l2":{"value":0,"unit":"kW"},"power_returned_l3":{"value":0,"unit":"kW"},"gas_delivered":{"value":"5","unit":"m3"}}');

		//-------CHECKS
		//check of gasmeter beschikbaar is	(indien HeeftGas = true uit Frontend,json of eerdere meeting dan niet meer checken uit meterdata, bij false wel checken in meterdata)
		if (!HeeftGas) HeeftGas = "gas_delivered" in json ? !isNaN(json.gas_delivered.value) : false ;
		if (!HeeftWater) HeeftWater =  "water" in json ? !isNaN(json.water.value) : false ;
		//todo check of slimme meter is gekoppeld of standalone watermeter
		
		//check of p1 gegevens via api binnen komen
		if (json.timestamp.value == "-") {
			console.log("timestamp missing : p1 gegevens correct?");
// 			if (!HeeftWater) return;
			return;
		}
		
		//check of teruglevering actief is 
		var teruglevering = Injection;
		if (!teruglevering) teruglevering = isNaN(json.energy_returned_tariff1.value)?false:json.energy_returned_tariff1.value;

		//-------TOON METERS
		document.getElementById("w8api").style.display = "none"; //hide wait message
		document.getElementById("l1").style.display = "block";
		document.getElementById("l3").style.display = "block";		
		if (teruglevering) {		
			document.getElementById("l5").style.display = "block";
			document.getElementById("l6").style.display = "block";
			document.getElementById("Ph").innerHTML = "Afname-Terug";
		} else
		{
			if (ShowVoltage) document.getElementById("l2").style.display = "block";
		}
		if (HeeftGas) document.getElementById("l4").style.display = "block";
		if (HeeftWater) { 
			document.getElementById("l7").style.display = "block";
			document.getElementById("l2").style.display = "none";
		}

		//-------SPANNING METER				
		if (!teruglevering || !ShowVoltage)
		{
			//aantal fases berekenen
			var fases = 1;
			if (Phases) fases = Phases;
			else {
				//bereken het aantal fases aan de hand van de slimme meter data
				if (!isNaN(json.voltage_l2.value)) fases++;
				if (!isNaN(json.voltage_l3.value)) fases++;
				document.getElementById("fases").innerHTML = fases;
			}
			
			let TotalU = 0 +(isNaN(json.voltage_l1.value)?0:json.voltage_l1.value) + 
			(isNaN(json.voltage_l2.value)?0:json.voltage_l2.value) + 
			(isNaN(json.voltage_l3.value)?0:json.voltage_l3.value);
	
			let Vgem=TotalU/fases;
			
			//min - max waarde
			if (minV == 0.0 || Vgem < minV) { minV = Vgem; }
			if (Vgem > maxV) { maxV = Vgem; }
			document.getElementById(`power_delivered_2max`).innerHTML = Number(maxV.toFixed(0)).toLocaleString();
			document.getElementById(`power_delivered_2min`).innerHTML = Number(minV.toFixed(0)).toLocaleString();   

			//update gauge
			gauge_v.refresh(Vgem);
		}

		//-------ACTUEEL METER		
		//afname of teruglevering bepalen en signaleren
		let TotalKW	= 0;
		if (json.power_returned.value > 0) { 
			TotalKW = json.power_returned.value;
			document.getElementById("power_delivered_l1h").style.backgroundColor = "green";
			document.getElementById("power_delivered_l1h").innerHTML = "Teruglevering";
		} else
		{
			TotalKW = json.power_delivered.value;
			document.getElementById("power_delivered_l1h").innerHTML = "Actueel";
			document.getElementById("power_delivered_l1h").style.backgroundColor = "#314b77";
		}
		
		//update gauge
		TotalAmps = (isNaN(json.current_l1.value)?0:json.current_l1.value) + 
			(isNaN(json.current_l2.value)?0:json.current_l2.value) + 
			(isNaN(json.current_l3.value)?0:json.current_l3.value);
		gauge.refresh(TotalAmps, AMPS * fases);

		//update actuele vermogen			
		document.getElementById("power_delivered").innerHTML = TotalKW.toLocaleString();

		//vermogen min - max bepalen
		let nvKW= json.power_delivered.value; 
		if (minKW == 0.0 || nvKW < minKW) { minKW = nvKW;}
		if (nvKW> maxKW){ maxKW = nvKW; }
		document.getElementById(`power_delivered_1max`).innerHTML = Number(maxKW.toFixed(3)).toLocaleString();                    
		document.getElementById(`power_delivered_1min`).innerHTML = Number(minKW.toFixed(3)).toLocaleString();                        
		
		//-------VERBRUIK METER	
				//bereken verschillen afname, teruglevering en totaal
		for(let i=0;i<3;i++){
			if (i==0) {
				Parra[0]=Number(json.energy_delivered_tariff1.value + json.energy_delivered_tariff2.value - hist_arrPa[1]).toFixed(3);
				Parri[0]=Number(json.energy_returned_tariff1.value + json.energy_returned_tariff2.value - hist_arrPi[1]).toFixed(3);

			} else {
				Parra[i]=Number(hist_arrPa[i] - hist_arrPa[i+1]).toFixed(3);
				Parri[i]=Number(hist_arrPi[i] - hist_arrPi[i+1]).toFixed(3);
			}
			Parr[i]=Number(Parra[i] - Parri[i]).toFixed(3);
// 			if (Parr[i] < 0) Parr[i] = 0;
		}

		//dataset berekenen voor Ptotaal
		Pmax = math.max(Parr);		// maximale waarde bepalen voor de meters
		for(let i=0;i<3;i++){
			trend_p.data.datasets[i].data=[Number(Parr[i]).toFixed(1),Number(Pmax-Parr[i]).toFixed(1)];
		};
		trend_p.update();

		//vermogen vandaag, min - max bepalen
		document.getElementById("P").innerHTML = Number(Parr[0]).toLocaleString();
// 		document.getElementById(`Pmax`).innerHTML = Number(Pmax).toLocaleString();
// 		document.getElementById(`Pmin`).innerHTML = Math.min.apply(Math, Parr).toLocaleString();
		
		if (teruglevering) 
		{
			//-------INTJECTIE METER	
			//data sets berekenen voor de gauges
			var Pmaxi = math.max(Parri);
			for(let i=0;i<3;i++){
				trend_pi.data.datasets[i].data=[Number(Parri[i]).toFixed(1),Number(Pmaxi-Parri[i]).toFixed(1)];
			};
			trend_pi.update();
			//vermogen vandaag, min - max bepalen
			document.getElementById("Pi").innerHTML = Number(Parri[0]).toLocaleString();
// 			document.getElementById(`Pimax`).innerHTML = Number(Pmaxi).toLocaleString();
// 			document.getElementById(`Pimin`).innerHTML = Math.min.apply(Math, Parri).toLocaleString();

			//-------AFNAME METER	
			//data sets berekenen voor de gauges
			var Pmaxa = math.max(Parra);
			for(let i=0;i<3;i++){
				trend_pa.data.datasets[i].data=[Number(Parra[i]).toFixed(1),Number(Pmaxa-Parra[i]).toFixed(1)];
			};
			trend_pa.update();
			//vermogen vandaag, min - max bepalen
			document.getElementById("Pa").innerHTML = Number(Parra[0]).toLocaleString();
// 			document.getElementById(`Pamax`).innerHTML = Number(Pmaxa).toLocaleString();
// 			document.getElementById(`Pamin`).innerHTML = Math.min.apply(Math, Parra).toLocaleString();
		}
		
		//-------GAS METER	
		if (HeeftGas) 
		{
					//bereken verschillen gas, afname, teruglevering en totaal
			for(let i=0;i<3;i++){
				if (i==0) Garr[0]=Number(json.gas_delivered.value - hist_arrG[1]).toFixed(3) ;
				else Garr[i]=Number(hist_arrG[i] - hist_arrG[i+1]).toFixed(3);
				if (Garr[i] < 0) Garr[i] = 0;
			}

			Gmax = math.max(Garr);
			for(let i=0;i<3;i++){
				trend_g.data.datasets[i].data=[Number(Garr[i]).toFixed(1),Number(Gmax-Garr[i]).toFixed(1)];
			};
			trend_g.update();
			document.getElementById("G").innerHTML = Number(Garr[0]).toLocaleString();
		}
		
		//-------WATER METER	
		if (HeeftWater) 
		{
					//bereken verschillen gas, afname, teruglevering en totaal
			for(let i=0;i<3;i++){
				if (i==0) Warr[0]=Number(json.water.value - hist_arrW[1])*1000 ;
				else Warr[i]=Number(hist_arrW[i] - hist_arrW[i+1])*1000;
				if (Warr[i] < 0) Warr[i] = 0;
			}

			Wmax = math.max(Warr);
			for(let i=0;i<3;i++){
				trend_w.data.datasets[i].data=[Number(Warr[i]).toFixed(),Number(Wmax-Warr[i]).toFixed()];
			};
			trend_w.update();
			document.getElementById("W").innerHTML = Number(Warr[0]).toLocaleString();
		}
		
								
		Spinner(false);
		}); //end fetch fields
}
	
  //============================================================================  
    
function menu() {
  var x = document.getElementById("myTopnav");
  if (x.className === "main-navigation") {
    x.className += " responsive";
    
  } else {
    x.className = "main-navigation";
  }
//change menu icon
    var menu = document.getElementById("menuid");
	menu.classList.toggle("mdi-close");
	menu.classList.toggle("mdi-menu");
}
   
  //============================================================================  

function handle_menu_click()
{	
	var btns = document.getElementsByClassName("nav-item");
	for (let i = 0; i < btns.length; i++) {
  		btns[i].addEventListener("click", function() {
			//reset hamburger menu icon
			var menu = document.getElementById("menuid");
			menu.classList.remove("mdi-close");
			menu.classList.add("mdi-menu");
	
			//remove active classes
			let current = document.getElementById("myTopnav").getElementsByClassName("active");	
			for (let j=current.length-1; j>=0; j--) {
	// 					console.log("remove active ["+j+"] current:"+current[j]);
				current[j].classList.remove("active");
		}
		//add new active classes
			var closest = this.closest("ul");
			if (closest && !closest.previousElementSibling.classList.contains("active","topcorner","mdi")) {
				console.log("contains: subnav, closest:" +closest.className);
				closest.previousElementSibling.classList.add("active");
			}
			if(!this.classList.contains("active","nav-img")) 
			this.classList.add("active");

			activeTab = this.id;
			//console.log("ActiveID - " + activeTab );
			openTab();  		
  		});
	}
}

  //============================================================================  
  
  function bootsTrapMain() {
    console.log("bootsTrapMain()");
// 	console.log("hash:"+ location.hash);
	gauge = new JustGage(GaugeOptions); // initialize gauge
	gauge_v = new JustGage(GaugeOptionsV); // initialize gauge
	trend_g = new Chart(document.getElementById("container-4"), TrendG);
	trend_p = new Chart(document.getElementById("container-3"), {type: 'doughnut', data:dataP, options: optionsP});
	trend_pi = new Chart(document.getElementById("container-5"), {type: 'doughnut', data:dataPi, options: optionsP});
	trend_pa = new Chart(document.getElementById("container-6"), {type: 'doughnut', data:dataPa, options: optionsP} );
	trend_w = new Chart(document.getElementById("container-7"), TrendW);
	
	handle_menu_click();
	FrontendConfig();
    refreshDevTime();
    clearInterval(timeTimer);  
    timeTimer = setInterval(refreshDevTime, 10 * 1000); // repeat every 10s

    setMonthTableType();
	getDevSettings();
    refreshDevInfo();
    openTab();
    initActualGraph();
    setPresentationType('TAB');
	//after loading ... flow the #target url just for FSExplorer
// 	console.log("location-hash: " + location.hash );
// 	console.log("location-pathname: " + location.pathname );
// 	console.log("location-msg: " + location.hash.split('msg=')[1]);
// 	console.log("location-hash-split: " + location.hash.split('#')[1].split('?')[0]);
	//goto tab after reload 
	if (location.hash == "#FileExplorer") { document.getElementById('bFSexplorer').click(); }
	if (location.hash.split('#')[1].split('?')[0] == "Redirect") { handleRedirect(); }

	//reselect Dash when Home icon has been clicked
 	document.getElementById("Home").addEventListener("click", function() { document.getElementById('bDashTab').click(); });
  } // bootsTrapMain()
  
  
  //============================================================================  
  function handleRedirect(){
	console.log("location-handle: " + location.hash.split('msg=')[1]);
	//close all sections
   	var elements = document.getElementsByClassName("tabName");
    for (var i = 0; i < elements.length; i++){
        elements[i].style.display = "none";
    }
    //open only redirect section
	document.getElementById("Redirect").style.display = "block";
   	
   	if (location.hash.split('msg=')[1] == "RebootOnly"){  document.getElementById("RedirectMessage").innerHTML = "Het systeem wordt opnieuw gestart...";}
   	else if (location.hash.split('msg=')[1] == "RebootResetWifi"){  document.getElementById("RedirectMessage").innerHTML = "Het systeem wordt opnieuw gestart... Stel hierna het wifi netwerk opnieuw in. zie handleiding.";}   	
   	else if (location.hash.split('msg=')[1] == "NoUpdateServer"){  document.getElementById("RedirectMessage").innerHTML = "Het systeem wordt opnieuw gestart... Er is geen update mogelijkheid beschikbaar";} 
   	else if (location.hash.split('msg=')[1] == "Update"){  document.getElementById("RedirectMessage").innerHTML = "Het systeem wordt vernieuwd en daarna opnieuw gestart";} 

	setInterval(function() {
		var div = document.querySelector('#counter');
		var count = div.textContent * 1 - 1;
		div.textContent = count;
		if (count <= 0) {
			window.location.replace("/");
		}
	}, 1000);
}

  //============================================================================  
  
  function Spinner(show) {
	if (show) {
		document.getElementById("loader").removeAttribute('hidden');
		setTimeout(() => { document.getElementById("loader").setAttribute('hidden', '');}, 5000);
	} else document.getElementById("loader").setAttribute('hidden', '');
  }
  
  //============================================================================  
  function show_hide_column(table, col_no, do_show) {
   var tbl = document.getElementById(table);
   var col = tbl.getElementsByTagName('col')[col_no];
   if (col) {
     col.style.visibility=do_show?"":"collapse";
   }
}
  //============================================================================  
  function openTab() {

    console.log("openTab : " + activeTab );
   document.getElementById("myTopnav").className = "main-navigation"; //close dropdown menu 
    clearInterval(tabTimer);  
    clearInterval(actualTimer);  

	//--- hide canvas -------
    document.getElementById("dataChart").style.display = "none";
    document.getElementById("gasChart").style.display  = "none";
	document.getElementById("waterChart").style.display  = "none";
	show_hide_column('lastHoursTable',4,HeeftWater);
	show_hide_column('lastDaysTable',4,HeeftWater);
	show_hide_column('lastMonthsTable',13,HeeftWater);
	show_hide_column('lastMonthsTable',14,HeeftWater);
	show_hide_column('lastMonthsTable',15,HeeftWater);
	show_hide_column('lastMonthsTable',16,HeeftWater);
    
    if (activeTab != "bActualTab") {
      actualTimer = setInterval(refreshSmActual, 60 * 1000);                  // repeat every 60s
    }
    if (activeTab == "bActualTab") {
      refreshSmActual();
      if (tlgrmInterval < 10)
            actualTimer = setInterval(refreshSmActual, 10 * 1000);            // repeat every 10s
      else  actualTimer = setInterval(refreshSmActual, tlgrmInterval * 1000); // repeat every tlgrmInterval seconds

    } else if (activeTab == "bHoursTab") {
      refreshHours();
      clearInterval(tabTimer);
      tabTimer = setInterval(refreshHours, 58 * 1000); // repeat every 58s

    } else if (activeTab == "bDaysTab") {
      refreshDays();
      clearInterval(tabTimer);
      tabTimer = setInterval(refreshDays, 58 * 1000); // repeat every 58s

    } else if (activeTab == "bMonthsTab") {
      refreshMonths();
      clearInterval(tabTimer);
      tabTimer = setInterval(refreshMonths, 118 * 1000); // repeat every 118s
    
    } else if (activeTab == "bInfo_SysInfo") {
      refreshDevInfo();
      clearInterval(tabTimer);
      tabTimer = setInterval(refreshDevInfo, 58 * 1000); // repeat every 58s

    } else if (activeTab == "bInfo_Fields") {
      refreshSmFields();
      clearInterval(tabTimer);
      tabTimer = setInterval(refreshSmFields, 58 * 1000); // repeat every 58s

    } else if (activeTab == "bInfo_Telegram") {
      refreshSmTelegram();
      clearInterval(tabTimer); // do not repeat!

    } else if (activeTab == "bInfo_APIdoc") {
      //do nothing = static html
    } else if (activeTab == "bDashTab") {
       readGitHubVersion();
       UpdateDash();
       clearInterval(tabTimer);
       clearInterval(actualTimer);  
       tabTimer = setInterval(UpdateDash, 10 * 1000); // repeat every 10s
    } else if (activeTab == "bFSexplorer") {
      FSExplorer();
    } else if (activeTab == "bEditMonths") {
      document.getElementById('tabEditMonths').style.display = "block";
      document.getElementById('tabEditSettings').style.display = "none";
      clearInterval(actualTimer); //otherwise the data blok is overwritten bij actual data
      getMonths();

    } else if (activeTab == "bSettings" || activeTab == "bEditSettings") {
	  refreshDevTime();
	  //refreshDevInfo();
	  data = {};
      document.getElementById('tabEditSettings').style.display = 'block';
      document.getElementById('tabEditMonths').style.display = "none";
	  clearInterval(actualTimer); //otherwise the data blok is overwritten bij actual data
      refreshSettings();
      getDevSettings();
      activeTab = "bEditSettings";
    } else if (activeTab == "bSysInfoTab") {
	  data = {};
	  clearInterval(actualTimer); //otherwise the data blok is overwritten bij actual data
      refreshDevInfo();
	  tabTimer = setInterval(refreshDevInfo, 10 * 1000); // repeat every 10s
    } else if (activeTab == "bInfo_frontend") {
	  data = {};
	  clearInterval(actualTimer); //otherwise the data blok is overwritten bij actual data
      FrontendConfig();
    } 
  } // openTab()
  
    //============================================================================  

  function FSExplorer() {
	 let span = document.querySelector('span');
	 let main = document.querySelector('main');
	 let fileSize = document.querySelector('fileSize');

	 Spinner(true);
	 fetch('api/listfiles', {"setTimeout": 5000}).then(function (response) {
		 return response.json();
	 }).then(function (json) {
	
	//clear previous content	 
	 var list = document.getElementById("FSmain");
	 while (list.hasChildNodes()) {  
	   list.removeChild(list.firstChild);
	 }

	   let dir = '<table id="FSTable" width=90%>';
	   for (var i = 0; i < json.length - 1; i++) {
		 dir += "<tr>";
		 dir += `<td width=250px nowrap><a href ="${json[i].name}" target="_blank">${json[i].name}</a></td>`;
		 dir += `<td width=100px nowrap><small>${json[i].size}</small></td>`;
		 dir += `<td width=100px nowrap><a href ="${json[i].name}"download="${json[i].name}"> Download </a></td>`;
		 if (  FS_no_delete.indexOf(json[i].name) < 0 ) {
		   dir += `<td width=100px nowrap><a href ="${json[i].name}?delete=/${json[i].name}"> Delete </a></td>`;
		   if (json[i].name == '!format') 
		   {
			 document.getElementById('FormatSPIFFS').disabled = false;
		   }
		 } else {
		   dir += `<td width=100px nowrap> </td>`;
		 }                 	 
		 dir += "</tr>";
	   }	// for ..
	   main.insertAdjacentHTML('beforeend', dir);
	   document.querySelectorAll('[href*=delete]').forEach((node) => {
			 node.addEventListener('click', () => {
					 if (!confirm('Weet je zeker dat je dit bestand wilt verwijderen?!')) event.preventDefault();  
			 });
	   });
	   main.insertAdjacentHTML('beforeend', '</table>');
	   main.insertAdjacentHTML('beforeend', `<p id="FSFree">Opslag: <b>${json[i].usedBytes} gebruikt</b> | ${json[i].totalBytes} totaal`);
	   free = json[i].freeBytes;
	   fileSize.innerHTML = "<b> &nbsp; </b><p>";    // spacer                
	   Spinner(false);
	 });	// function(json)
	 
	 document.getElementById('Ifile').addEventListener('change', () => {
		 let nBytes = document.getElementById('Ifile').files[0].size, output = `${nBytes} Byte`;
		 for (var aMultiples = [
			 ' KB',
			 ' MB'
			], i = 0, nApprox = nBytes / 1024; nApprox > 1; nApprox /= 1024, i++) {
			  output = nApprox.toFixed(2) + aMultiples[i];
			}
			if (nBytes > free) {
			  fileSize.innerHTML = `<p><small> Bestand Grootte: ${output}</small><strong style="color: red;"> niet genoeg ruimte! </strong><p>`;
			  document.getElementById('Iupload').setAttribute('disabled', 'disabled');
			} 
			else {
			  fileSize.innerHTML = `<b>Bestand grootte:</b> ${output}<p>`;
			  document.getElementById('Iupload').removeAttribute('disabled');
			}
	 });	
  }
  
  //============================================================================  
  function refreshDevInfo()
  { Spinner(true);
    fetch(APIGW+"v2/dev/info", {"setTimeout": 5000})
      .then(response => response.json())
      .then(json => {
        console.log("parsed .., data is ["+ JSON.stringify(json)+"]");
		Spinner(false);
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
           if (k == "fwversion"){
			   console.log("fwversion: " + obj[k] );
			   devVersion = obj[k];
           }
        } //for loop      
	  //new fwversion detection
  	  document.getElementById('devVersion').innerHTML = obj.fwversion;
	  var tmpFW = devVersion;
	  firmwareVersion_dspl = tmpFW;
	  tmpFW = tmpFW.replace("+", " ");
	  tmpFW = tmpFW.replace("v", "");
	  console.log("tmpFW: " + tmpFW);
	  tmpX = tmpFW.substring(0, tmpFW.indexOf(' '));
// 	  console.log("tmpX: " + tmpX);
	  tmpN = tmpX.split(".");
// 	  	  console.log("tmpN: " + tmpN);
	  firmwareVersion = tmpN[0]*10000+tmpN[1]*100+tmpN[2]*1;
	  console.log("firmwareVersion["+firmwareVersion+"] >= GitHubVersion["+GitHubVersion+"]");
	  if (GitHubVersion == 0 || firmwareVersion >= GitHubVersion)
			newVersionMsg = "";
	  else  newVersionMsg = " nieuwere versie ("+GitHubVersion_dspl+") beschikbaar";
	  document.getElementById('message').innerHTML = newVersionMsg;
	  console.log(newVersionMsg);

	  tlgrmInterval = obj.telegraminterval;
      if (firmwareVersion > 20000) document.getElementById("resetWifi").removeAttribute('hidden');
      if (firmwareVersion > 20102) document.getElementById("update").removeAttribute('hidden');

      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(   document.createTextNode('Error: ' + error.message)  );
      });
  } // refreshDevInfo()

//============================================================================  
  function FrontendConfig()  {
    console.log("Read Frontend config");
	Spinner(true);
	fetch(APIGW+"../Frontend.json", {"setTimeout": 5000})
      .then(response => response.json())
      .then(json => {
		  data = json;
          //console.log("parsed frontend config: ["+ JSON.stringify(json)+"]");
          //{"ShowVoltage":true,"Injection":false,"Phases":1,"Fuse":35,"cdn":true}
          AMPS=json.Fuse;
          ShowVoltage=json.ShowVoltage;
          UseCDN=json.cdn;
          Injection=json.Injection;
          Phases=json.Phases;   
          HeeftGas=json.GasAvailable;
          
          for (var item in data) 
          {
//           	console.log("config item: " +item);
//           	console.log("config data[item].value: " +data[item].value);
            var tableRef = document.getElementById('frontendTable').getElementsByTagName('tbody')[0];
            if( ( document.getElementById("frontendTable_"+item)) == null )
            {
              var newRow   = tableRef.insertRow();
              newRow.setAttribute("id", "frontendTable_"+item, 0);
              // Insert a cell in the row at index 0
              var newCell  = newRow.insertCell(0);                  // name
              var newText  = document.createTextNode('');
              newCell.appendChild(newText);
              newCell  = newRow.insertCell(1);                      // humanName
              newCell.appendChild(newText);
              newCell  = newRow.insertCell(2);                      // value
              newCell.appendChild(newText);
            }
            tableCells = document.getElementById("frontendTable_"+item).cells;
            tableCells[0].innerHTML = item;
            tableCells[1].innerHTML = translateToHuman(item);
            tableCells[2].innerHTML = data[item];
          }
         Spinner(false);
      }) //json
  }

  //============================================================================  
  function refreshDevTime()
  {
	alert_message("");
    console.log("Refresh api/v2/dev/time ..");
    
	let controller = new AbortController();
	setTimeout(() => controller.abort(), 5000);    
    fetch(APIGW+"v2/dev/time", { signal: controller.signal})
      .then(response => response.json())
      .then(json => {
              document.getElementById('theTime').innerHTML = json.time;
              console.log("parsed .., data is ["+ JSON.stringify(json)+"]");

	  //after reboot checks of the server is up and running and redirects to home
      if ((document.querySelector('#counter').textContent < 40) && (document.querySelector('#counter').textContent > 0)) window.location.replace("/");
      })
      .catch(function(error) {    
		if (error.name === "AbortError") {console.log("time abort error")}
//         var p = document.createElement('p');
//         p.appendChild( document.createTextNode('Error: ' + error.message) );
        alert_message("Datum/tijd kan niet opgehaald worden");
      });     
      
    document.getElementById('message').innerHTML = newVersionMsg;

  } // refreshDevTime()
    
  //============================================================================  
  function refreshSmActual()
  { 
  	Spinner(true);
    fetch(APIGW+"v2/sm/actual", {"setTimeout": 5000})
      .then(response => response.json())
      .then(json => {
          console.log("actual parsed .., fields is ["+ JSON.stringify(json)+"]");
          data = json;
          copyActualToChart(data);
          if (presentationType == "TAB")
                showActualTable(data);
          else  showActualGraph(data);
        //console.log("-->done..");
         Spinner(false);
      }) //json
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild( document.createTextNode('Error: ' + error.message) );
      }); //catch
  };  // refreshSmActual()
  
  
  //============================================================================  
  function refreshSmFields()
  { Spinner(true);
    fetch(APIGW+"v2/sm/fields", {"setTimeout": 5000})
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
        Spinner(false);

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
    if (activeTab == "bHoursTab") {  
    
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
		data.data[i].water  = (data.data[i].values[5]  - data.data[slotbefore].values[5]).toFixed(3);

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
		data.data[i].water      = (data.data[i].values[5]).toFixed(3);
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
  function alert_message(msg) {
  	if (msg==""){
  		document.getElementById('messages').style="display:none";
	
  	} else {  
	document.getElementById('messages').style="display:block";
	document.getElementById('messages').innerHTML = msg;
	} 
  }
  //============================================================================  
  function refreshHours()
  { Spinner(true);
    console.log("fetch("+APIGW+"../RNGhours.json)");

    fetch(APIGW+"../RNGhours.json", {"setTimeout": 5000})
      .then(function (response) {
		if (response.status !== 200) {
			throw new Error(response.status);
		} else {
			return response.json();
		}
	})
	.then(function (json) {
        //console.log(json);
        data = json;
        expandData(data);
        if (presentationType == "TAB")
              showHistTable(data, "Hours");
        else  showHistGraph(data, "Hours");
	 Spinner(false);

      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild( document.createTextNode('Error: ' + error.message) );
    	alert_message("Fout bij ophalen van de historische uurgegevens");
      }); 
  } // resfreshHours()
  
  
  //============================================================================  
  function refreshDays()
  {
	// if (PauseAPI) return;
	Spinner(true);
    console.log("fetch("+APIGW+"../RNGdays.json)");
    fetch(APIGW+"../RNGdays.json", {"setTimeout": 5000})
    .then(function (response) {
		if (response.status !== 200) {
			throw new Error(response.status);
		} else {
			return response.json();
		}
	})
	.then(function (json) {
		data = json;
        expandData(data);
        if (presentationType == "TAB")
              showHistTable(data, "Days");
        else  showHistGraph(data, "Days");
		//voor dashboard
        var act_slot = data.actSlot;
//         console.log("Refreshdays - actSlot: " + act_slot);
		for (let i=0;i<4;i++)
		{	let tempslot = math.mod(act_slot-i,15);
			hist_arrG[i] = json.data[tempslot].values[4];
			hist_arrW[i] = json.data[tempslot].values[5];
			hist_arrPa[i] = json.data[tempslot].values[0] + json.data[tempslot].values[1];
			hist_arrPi[i] = json.data[tempslot].values[2] + json.data[tempslot].values[3];
		};
	    Spinner(false);
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
    	console.log(error);
    	alert_message("Fout bij ophalen van de historische daggegevens");
      });
  } // resfreshDays()
  
  
  //============================================================================  
  function refreshMonths()
  {
  	Spinner(true);
    console.log("fetch("+APIGW+"../RNGmonths.json)");
    fetch(APIGW+"../RNGmonths.json", {"setTimeout": 5000})
      .then(function (response) {
		if (response.status !== 200) {
			throw new Error(response.status);
		} else {
			return response.json();
		}
	})
	.then(function (json) {
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
        Spinner(false);
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
    	alert_message("Fout bij ophalen van de historische maandgegevens");

      });
  } // resfreshMonths()

    
  //============================================================================  
  function refreshSmTelegram()
  { Spinner(true);
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
        Spinner(false);
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
    if (activeTab != "bActualTab") return;

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
	document.getElementById("waterChart").style.display  = "none";
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

      var tableRef = document.getElementById('last'+type+'Table');
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
		newCell  = newRow.insertCell(4);
        newCell.appendChild(newText);        if (type == "Days")
        {
          newCell  = newRow.insertCell(5);
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
      if (data.data[index].water >= 0) tableCells[4].innerHTML = data.data[index].water;

      if (type == "Days")
      {
        tableCells[5].innerHTML = ( (data.data[index].costs_e + data.data[index].costs_g) * 1.0).toFixed(2);
      }
    };

    //--- hide canvas
    document.getElementById("dataChart").style.display = "none";
    document.getElementById("gasChart").style.display  = "none";
	document.getElementById("waterChart").style.display  = "none";
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

        newCell  = newRow.insertCell(13);             // jaar
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(14);             // gas
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(15);             // jaar
        newCell.appendChild(newText);
        newCell  = newRow.insertCell(16);             // gas
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
      
		tableCells[13].innerHTML = "20"+data.data[i].date.substring(0,2);          // jaar
      if (data.data[i].water >= 0) tableCells[14].innerHTML = data.data[i].water;                        // water
      
      tableCells[15].innerHTML = "20"+data.data[slotyearbefore].date.substring(0,2);      // jaar
      if (data.data[slotyearbefore].water >= 0) tableCells[16].innerHTML = data.data[slotyearbefore].water;                     // water
      
    };
    
    //--- hide canvas
    document.getElementById("dataChart").style.display  = "none";
    document.getElementById("gasChart").style.display   = "none";
	document.getElementById("waterChart").style.display   = "none";
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
	document.getElementById("waterChart").style.display   = "none";
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
  { Spinner(true);
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
        Spinner(false);
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

//    document.getElementById("APIdocTab").style.display = "none";

    if (activeTab == "bActualTab")  refreshSmActual();
    if (activeTab == "bHoursTab")   refreshHours();
    if (activeTab == "bDaysTab")    refreshDays();
    if (activeTab == "bMonthsTab")  refreshMonths();

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
  function refreshSettings()
  {
    console.log("refreshSettings() ..");
    Spinner(true);
    data = {};
    fetch(APIGW+"v2/dev/settings")
      .then(response => response.json())
      .then(json => {
        console.log("then(json => ..)");
        data = json;
        for( let i in data )
        {
          console.log("["+i+"]=>["+data[i].value+"]");
          var settings = document.getElementById('settings_table');
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
         Spinner(false);

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
  {	Spinner(true);
    console.log("fetch("+APIGW+"v2/hist/months)");
    fetch(APIGW+"v2/hist/months", {"setTimeout": 5000})
      .then(response => response.json())
      .then(json => {
        //console.log(response);
        data = json;
        expandDataSettings(data);
        showMonths(data, monthType);
        Spinner(false);
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
    var dlength = data.data.length;
    console.log("Now fill the DOM!");    
    console.log("data.data.length: " + dlength);
  
    for (let index=data.actSlot + dlength; index>data.actSlot; index--)
    {  let i = index % dlength;

      //console.log("["+i+"] >>>["+data.data[i].EEYY+"-"+data.data[i].MM+"]");
      
      var em = document.getElementById('editMonths');

      if( ( document.getElementById("em_R"+i)) == null )
      {
        var div1 = document.createElement("div");
            div1.setAttribute("class", "settingDiv");
            div1.setAttribute("id", "em_R"+i);
            if (i == (data.data.length -1))  // last row
            {
            }
            var span2 = document.createElement("span");
              //--- create input for EEYY
              var sInput = document.createElement("INPUT");
              sInput.setAttribute("id", "em_YY_"+i);
              sInput.setAttribute("class", "tab_editMonth");
              sInput.setAttribute("type", "number");
              sInput.setAttribute("min", 2000);
              sInput.setAttribute("max", 2099);
              sInput.size              = 5;
              sInput.addEventListener('change',
                      function() { setNewValue(i, "EEYY", "em_YY_"+i); }, false);
              span2.appendChild(sInput);
              //--- create input for months
              var sInput = document.createElement("INPUT");
              sInput.setAttribute("id", "em_MM_"+i);
              sInput.setAttribute("class", "tab_editMonth");
              sInput.setAttribute("type", "number");
              sInput.setAttribute("min", 1);
              sInput.setAttribute("max", 12);
              sInput.size              = 3;
              sInput.addEventListener('change',
                      function() { setNewValue(i, "MM", "em_MM_"+i); }, false);
              span2.appendChild(sInput);
              //--- create input for data column 1
              sInput = document.createElement("INPUT");
              sInput.setAttribute("id", "em_in1_"+i);
              sInput.setAttribute("class", "tab_editMonth");
              sInput.setAttribute("type", "number");
              sInput.setAttribute("step", 0.001);
              
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
                sInput.setAttribute("class", "tab_editMonth");        
                sInput.setAttribute("type", "number");
                sInput.setAttribute("step", 0.001);
                sInput.addEventListener('change',
                      function() { setNewValue(i, "edt2", "em_in2_"+i); }, false);
                span2.appendChild(sInput);
              }
              else if (type == "ER")
              {
                //console.log("add input for ert2..");
                var sInput = document.createElement("INPUT");
                sInput.setAttribute("id", "em_in2_"+i);
              sInput.setAttribute("class", "tab_editMonth");
                sInput.setAttribute("type", "number");
                sInput.setAttribute("step", 0.001);
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
    
    console.log("Now sequence EEYY/MM values ..");
    //--- sequence EEYY and MM data
// 	console.log("actslot: " + data.actSlot);

    var changed = false;
    for (let index=data.actSlot+dlength; index>data.actSlot; index--)
    {  let i = index % dlength;
       let next = math.mod(i-1,dlength);
    	console.log("index: "+index+" i: "+i+" next: "+next);
      //--- month
      if (data.data[next].MM == 0)
      {
        data.data[next].MM    = data.data[i].MM -1;
        changed = true;
        if (data.data[next].MM < 1) {
          data.data[next].MM   = 12;
		 data.data[next].EEYY = data.data[i].EEYY -1;
		 document.getElementById("em_YY_"+(next)).value = data.data[next].EEYY;
        } else {data.data[next].EEYY = data.data[i].EEYY}
        document.getElementById("em_MM_"+(next)).value = data.data[next].MM;        
  	 	document.getElementById("em_YY_"+(next)).value = data.data[next].EEYY;
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
    if (activeTab == "bEditMonths") {
      console.log("getMonths");
      getMonths();
    } else if (activeTab == "bEditSettings") {
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

    if (activeTab == "bEditSettings")
    {
      saveSettings();
    } 
    else if (activeTab == "bEditMonths")
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
    console.log("send JSON:["+JSON.stringify(jsonString)+"]");
    const other_params = {
        headers : { "content-type" : "application/json; charset=UTF-8"},
        body : JSON.stringify(jsonString),
        method : "POST",
        //aangepast cors -> no-cors
        mode : "no-cors"
    };
	Spinner(true);
    fetch(APIGW+"v2/dev/settings", other_params)
      .then(function(response) {
            //console.log(response.status );    //=> number 100–599
            //console.log(response.statusText); //=> String
            //console.log(response.headers);    //=> Headers
            //console.log(response.url);        //=> String
            //console.log(response.text());
            //return response.text()
            Spinner(false);
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
    
    const jsonString = {"recid": recId, "edt1": 1 * row[i].values[0], "edt2": 1 * row[i].values[1],
                         "ert1": 1 * row[i].values[2],  "ert2": 1 * row[i].values[3], "gdt":  1 * row[i].values[4] };
	console.log ("JsonString: "+JSON.stringify(jsonString));
    const other_params = {
        headers : { "content-type" : "application/json; charset=UTF-8"},
        body : JSON.stringify(jsonString),
        method : "POST",
        mode : "cors"
    };
    Spinner(true);
    fetch(APIGW+"v2/hist/months", other_params)
      .then(function(response) {
      Spinner(false);
      }, function(error) {
        console.log("Error["+error.message+"]"); //=> String
      });
      
  } // sendPostReading()

  
  //============================================================================  
  function readGitHubVersion()
  {
    if (GitHubVersion != 0) return;
    fetch("https://cdn.jsdelivr.net/gh/mhendriks/DSMR-API-V2@master/edge/DSMRversion.dat")
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
        GitHubVersion = tmpN[0]*10000 + tmpN[1]*100 + tmpN[2]*1;
        
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
          ,[ "spiffssize",                "Spiffs Size" ]
          ,[ "FSsize", 					  "File System Size" ]
          ,[ "flashchipspeed",            "Flash Chip Speed" ]
          ,[ "flashchipmode",             "Flash Chip Mode" ]
          ,[ "boardtype",                 "Board Type" ]
          ,[ "compileoptions",            "Compiler Opties" ]
          ,[ "ssid",                      "WiFi SSID" ]
          ,[ "wifirssi",                  "WiFi RSSI" ]
          ,[ "uptime",                    "Up Time [dagen] - [hh:mm]" ]
          ,[ "reboots",                   "Aantal keer opnieuw opgestart" ]
          ,[ "lastreset",                 "Laatste Reset reden" ]
          ,[ "smr_version",               "NL of BE Slimme Meter" ]
          ,[ "ShowVoltage",               "Toon spanningmeter in Dashboard" ]
          ,[ "Injection",                 "Wordt er stroom opgewekt (bv zonnecellen)" ]
          ,[ "Phases",                    "Hoeveel Fases heeft de meter [0-3]<br>[0=check op basis van meterdata]" ]
          ,[ "Fuse",                      "Wat is de waarde van de hoofdzekering(en)" ]
          ,[ "cdn",               		  "Frontend html/css uit de cloud" ]
          ,["GasAvailable",				  "Gasmeter beschikbaar? <br>[True = geen check op basis van meterdata]<br>[False = wel checken]"]
          ,["water",				  	  "Watermeter"]
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