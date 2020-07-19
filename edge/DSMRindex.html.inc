<!--
***************************************************************************  
**  Program  : DSMRindex.html.inc, part of DSMRloggerAPI
**  Version  : v1.2.2
**
**  Copyright (c) 2020 Willem Aandewiel
**  Update : MHendriks : png's loaded from the cdn too
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
-->
    <font face="Arial">
    <!-- <div class="dev-header"> -->
    <div class="header">
      <h1>
        <span id="sysName">DSMR-logger(API)</span> &nbsp; &nbsp; &nbsp;
		<div class="nav-item nav-clock">
		<span id="theTime" style="display: block">n/a</span>
		<span id="devVersion" style="display: block">[version]</span>
		</div>
        <!--span id="devVersion" style='font-size: small;right: 10px;'>[version]</span>
        <span id="devName"    style='font-size: small;'>-</span> &nbsp;
        <span id="source"     style='font-size: small;'> &nbsp; &nbsp; [-]</span>
        <span id='theTime' class='nav-item nav-clock'>00:00</span-->
      </h1>
   </div>
<!--  <div style="clear:both"></div> -->
  </font>
  
  <span id="mainPage" style="display:block">
  
  <!-- <div class="clock" id="theTime">1234567890</div> -->
  <!-- START Tabbladen -->
  <div class="nav-container">
    <div class='nav-left'>
      <a id='bActualTab'    class='nav-item tabButton'>Actueel</a>
      <a id='bHoursTab'     class='nav-item tabButton'>per Uur</a>
      <a id='bDaysTab'      class='nav-item tabButton'>per Dag</a>
      <a id='bMonthsTab'    class='nav-item tabButton'>per Maand</a>
      <a id='bTelegramTab'  class='nav-item tabButton'>Telegram</a>
      <a id='bFieldsTab'    class='nav-item tabButton'>all Fields</a>
      <a id='bSysInfoTab'   class='nav-item tabButton'>Systeem Info</a>
      <a id='bAPIdocTab'    class='nav-item tabButton'>API Docs</a>
      <a id='FSexplorer'    class='nav-img'><span class="mdi mdi-folder-outline" style="font-size: x-large;color: white;"></span></a>
      <a id='Settings'      class='nav-img'><span class="mdi mdi-cog" style="font-size: x-large;color: white;"></span></a>
    </div>
  </div>
  <!-- END Tabbladen -->
  
  <br>
  
  <!-- START ActualTab Tab -->
  <div id="ActualTab" class="tabName" style="display:block">
    <!-- START actueelTab Dummy -->
    <div>
        <span style="font-size: 30px;">Actueel &nbsp; </span>
        <span style="font-size: 15px;">(/api/v2/sm/actual) &nbsp; &nbsp; &nbsp; </span>
        <label for="pType"><b>Presentatie: </b></label>
        Tabel
        <input type="radio" id="aTAB" onchange="setPresentationType('TAB')" checked/>
        &nbsp;&nbsp;
        Grafisch
        <input type="radio" id="aGRAPH" onchange="setPresentationType('GRAPH')"/>
        &nbsp;&nbsp;
    </div>
    <div id="actual" >
      <table id="actualTable">
      <thead>
        <tr>
            <th>Name</th>
            <th>Value</th>
            <th style="text-align: center;">Unit</th>
        </tr>
      </thead>
      <tbody>
      </tbody>
      <tfoot>
        <tr>
            <td colspan=3>Data uit de Slimme Meter</td>
        </tr>
      <tfoot>
      </table>
    </div>
  
  </div>  <!-- END ActualTab Tab -->

  <!-- START HoursTab -->
  <div id="HoursTab" class="tabName">
    <div>
        <span style="font-size: 30px;">Afgelopen Uren &nbsp; </span>
        <span style="font-size: 15px;">(/api/v2/hist/hours) &nbsp; &nbsp; &nbsp; </span>
      <label for="pType"><b>Presentatie: </b></label>
      Tabel
      <input type="radio" id="hTAB" onchange="setPresentationType('TAB')" checked/>
      &nbsp;&nbsp;
      Grafisch
      <input type="radio" id="hGRAPH" onchange="setPresentationType('GRAPH')"/>
      &nbsp;&nbsp;
    </div>
    <div id="lastHours">
      <table id="lastHoursTable">
        <thead>
          <tr>
            <th>Date/Time</th>
            <th>Energie<br>Verbruik<br>(Watt)</th>
            <th>Energie<br>Teruglevering<br>(Watt)</th>
            <th>Gas-<br>verbruik<br>(m3)</th>
        <!--<th style="text-align: right;  border-bottom: 2px solid #ddd;">Kosten</th>-->
          </tr>
        </thead>
        <tbody>
        </tbody>
      </table>
    </div>
    
  </div> <!-- END HoursTab -->

  <!-- START DaysTab -->
  <div id="DaysTab" class="tabName">
    <div>
        <span style="font-size: 30px;">Afgelopen Dagen &nbsp; </span>
        <span style="font-size: 15px;">(/api/v2/hist/days) &nbsp; &nbsp; &nbsp; </span>
      <label for="pType"><b>Presentatie: </b></label>
      Tabel
      <input type="radio" id="dTAB" onchange="setPresentationType('TAB')" checked/>
      &nbsp;&nbsp;
      Grafisch
      <input type="radio" id="dGRAPH" onchange="setPresentationType('GRAPH')"/>
      &nbsp;&nbsp;
    </div>
    <div id='lastDays'>
      <table id="lastDaysTable">
      <thead>
        <tr>
            <th>Date/Time</th>
            <th>Energie<br>Verbruik<br>(Watt)</th>
            <th>Energie<br>Teruglevering<br>(Watt)</th>
            <th>Gas-<br>verbruik<br>(m3)</th>
            <th>Kosten</th>
        </tr>
      </thead>
      <tbody>
      </tbody>
      </table>
    </div>
  </div> <!-- END DaysTab -->

  <!-- START MonthsTab -->
  <div id="MonthsTab" class="tabName">
    <div>
        <span style="font-size: 30px;">Afgelopen Maanden &nbsp; </span>
        <span style="font-size: 15px;">(/api/v2/hist/months) &nbsp; &nbsp; &nbsp; </span>
      <label for="pType"><b>Presentatie: </b></label>
      Tabel
      <input type="radio" id="mTAB" onchange="setPresentationType('TAB')" checked/>
      &nbsp;&nbsp;
      Kosten
      <input type="checkbox" id="mCOST" onchange="setMonthTableType()"/>
      &nbsp;&nbsp;
      Grafisch
      <input type="radio" id="mGRAPH" onchange="setPresentationType('GRAPH')"/>
      &nbsp;&nbsp;
    </div>
    <div id='lastMonths'>
      <table id="lastMonthsTable" style="display:block">
      <thead>
        <tr>
            <th></th>
            <th colspan=4>Verbruik</th>
            <th style="text-align:center" colspan=4>Opgewekt</th>
            <th colspan=4>Gasverbruik</th>
        </tr>
        <tr>
            <th>Maand</th>
            <th>Jaar</th>
            <th>(kWh)</th>
            <th>Jaar</th>
            <th>(kWh)</th>

            <th>Jaar</th>
            <th>(kWh)</th>
            <th>Jaar</th>
            <th>(kWh)</th>
            
            <th>Jaar</th>
            <th>(m3)</th>
            <th>Jaar</th>
            <th>(m3)</th>
        </tr>
      </thead>
      <tbody>
              </tbody>
      </table>
      
      <table id="lastMonthsTableCosts" style="display:none">
      <thead>
        <tr>
            <th style="text-align: right;" width='80px'>Maand</th>
            <th>Jaar</th>
            <th style="text-align: right;" width='70px'>Electra</th>
            <th>Gas</th>
            <th>Netw.</th>
            <th style="text-align: right;" width='70px'>Totaal</th>

            <th>Jaar</th>
            <th style="text-align: right;" width='70px'>Electra</th>
            <th>Gas</th>
            <th>Netw.</th>
            <th style="text-align: right;" width='70px'>Totaal</th>
        </tr>
      </thead>
      <tbody>
      </tbody>
      </table>
    </div>
  </div> <!-- END MonthsTab -->

  <!-- START SysInfo Tab -->
  <div id="SysInfoTab" class="tabName">
    <div>
        <span style="font-size: 30px;">Device Info &nbsp; &nbsp; </span>
        <span style="font-size: 20px;">/api/v2/dev/info</span>
    </div>
    <div id='devInfo'>
      <table id="devInfoTable">
      <thead>
        <tr>
            <th>Name</th>
            <th>Value</th>
            <th>Unit</th>
        </tr>
      </thead>
      <tbody id="tb_info">
      </tbody>
      <tfoot>
        <tr>
            <td colspan=3>Data uit de DSMR-logger</td>
        </tr>
      <tfoot>
      </table>
    </div>
  
  </div>  <!-- END SysInfo Tab -->

  <!-- START TelegramTab -->
  <div id="TelegramTab" class="tabName">
    <div>
        <span style="font-size: 30px;">raw Telegram &nbsp; &nbsp; </span>
        <span style="font-size: 20px;">/api/v2/sm/telegram</span>
    </div>
    <div id="rawTelegram">
    </div>
    
  </div> <!-- END TelegramTab -->

  <!-- START Fields Tab -->
  <div id="FieldsTab" class="tabName">
    <!-- START fieldsTab Dummy -->
    <div>
        <span style="font-size: 30px;">All Fields &nbsp; </span>
        <span style="font-size: 15px;">(/api/v2/sm/fields) &nbsp; &nbsp; &nbsp; </span>
    </div>
    <div id='fields'>
      <table id="fieldsTable">
      <thead>
        <tr>
            <th>DSMR Name</th>
            <th>Human Name</th>
            <th>Value</th>
            <th>Unit</th>
        </tr>
      </thead>
      <tbody>
      </tbody>
      <tfoot>
        <tr>
            <td colspan=4>Data uit de Slimme Meter</td>
        </tr>
      <tfoot>
      </table>
    </div>
  
  </div>  <!-- END fields Tab -->

  <!-- START Api Docs Tab -->
  <div id="APIdocTab" class="tabName">
    <!-- START APIdocTab Dummy -->
    <div>
        <span style="font-size: 30px;">API documentie &nbsp; </span>
    </div>
  
  </div>  <!-- END API docs Tab -->

  <!-- REST -->

  <div style="margin: 30px;">
    <canvas id="dataChart" style="display: none;" width="900" height="300"></canvas>
    <canvas id="gasChart"  style="display: none;" width="900" height="125"></canvas>
  </div>
  </span>
<!--------------------------------------------------------------------------------------->
<!--------------------------------------------------------------------------------------->
<!--------------------------------------------------------------------------------------->
<!--------------------------------------------------------------------------------------->
  <span id="settingsPage" style="display:none">

  <div class="nav-container">
    <div class='nav-left'>
      <a id='bTerug'        class='nav-item tabButton'>Terug</a>
      <a id='bEditMonths'   class='nav-item tabButton'>Meterstanden</a>
      <a id='bEditSettings' class='nav-item tabButton'>Settings</a>
      <a id='bUndo'         class='nav-item tabButton'>Herstel</a>
      <a id='bSave'         class='nav-item tabButton'>Opslaan</a>
    </div>
  </div>  <!-- END nav-container -->

  <!-- START Tabbladen -->
  <div id="tabEditMonths" class="tabName">
    <label for="Period"><b>Meterstand: </b></label>
      Energy Delivered
      <input type="radio" name="editType" id="MED" onchange="setEditType('ED')" checked/>
      &nbsp;&nbsp;
      Energy Returned
      <input type="radio" name="editType" id="MER" onchange="setEditType('ER')"/>
      &nbsp;&nbsp;
      Gas Delivered
      <input type="radio" name="editType" id="MGD" onchange="setEditType('GD')"/>
      &nbsp;&nbsp;
  
  <div id="tabMaanden" class="tabName outer-div" style="display:none">
    <!-- START edit tabMonths -->
    <br>
      <div id="editMonths" class="inner-div">
      </div> 
  </div>  <!-- END tabMonthss -->

  </div> <!-- tabMonths -->
  
  
  <div id="tabEditSettings" class="tabName outer-div" style="display:none">
    <!-- START settingsTab Settings -->
    <br>
      <div id="settings" class="inner-div" style="width: 500px;">
      </div> 
  </div>  <!-- END tabSettings -->
  
  <!-- END Tabbladen -->

  </span> <!-- end Settings Page -->
  
  <!--
  <script src="https://cdn.jsdelivr.net/gh/mhendriks/DSMR-API@master/edge/DSMRedge.js"></script>
  <script src="https://cdn.jsdelivr.net/gh/mhendriks/DSMR-API@master/edge/DSMRgraphics.min.js"></script>
  -->

<!--
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
-->
