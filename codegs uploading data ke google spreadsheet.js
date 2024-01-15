function doGet(e) { 
  Logger.log(JSON.stringify(e));
  var result = [];

  if (e && e.parameter) {
    var sheet_id = '1l-GX7R2ZDQOW4u_5ho_uvlplEqD0RyPxCBSVwYa_TNE';  // Spreadsheet ID
    var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();

    if ('temperature' in e.parameter && 'Fan' in e.parameter) {
      // Jika parameter temperature dan Fan diterima, tambahkan data baru ke Spreadsheet
      var newRow = sheet.getLastRow() + 1;						
      var rowData = [];
      var Curr_Date = new Date();
      rowData[0] = Curr_Date; // Date in column A
      var Curr_Time = Utilities.formatDate(Curr_Date, "Asia/Jakarta", 'HH:mm:ss');
      rowData[1] = Curr_Time; // Time in column B

      rowData[2] = e.parameter['temperature']; // Temperature in column C
      rowData[3] = e.parameter['Fan']; // Fan status in column D

      Logger.log(JSON.stringify(rowData));
      var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
      newRange.setValues([rowData]);

      result = 'Temperature and Fan data written successfully.';
    } else {
      // Jika tidak ada parameter temperature dan Fan, baca data dari Spreadsheet
      var lastRow = sheet.getLastRow();						
      for (var i = 2; i <= lastRow; i++) {
        var rowData = [];
        var Curr_Date = sheet.getRange(i, 1).getValue();
        rowData.push(Curr_Date.toISOString()); // Date in column A
        rowData.push(sheet.getRange(i, 2).getValue()); // Temperature in column B
        rowData.push(sheet.getRange(i, 3).getValue()); // Fan status in column C

        result.push(rowData);
      }
    }
  } else {
    result = 'No Parameters';
  }

  // Log respons yang dikirimkan
  Logger.log(JSON.stringify(result));

  // Mengembalikan data sebagai JSON array
  return ContentService.createTextOutput(JSON.stringify(result));
}
