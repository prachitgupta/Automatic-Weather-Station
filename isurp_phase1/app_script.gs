function incrementFileCount() {
    var userProperties = PropertiesService.getUserProperties();
    var count = userProperties.getProperty('fileCount');
    if (!count) {
      count = 1;
    } else {
      count = parseInt(count) + 1;
    }
    userProperties.setProperty('fileCount', count);
    return count;
  }
  
  function doPost(e) {
    var folderName = "AWS";
    var blob = e.postData.contents;
  
    var count = incrementFileCount();
    var name = "file" + count + ".txt"; // Construct the file name based on the count
  
    var folders = DriveApp.getFoldersByName(folderName);
    if (folders.hasNext()) {
      var folder = folders.next();
      var file = folder.createFile(name, blob);
      return ContentService.createTextOutput('File uploaded to Google Drive with name ' + name);
    } else {
      return ContentService.createTextOutput('Folder not found in Google Drive');
    }
  }
  
  
  