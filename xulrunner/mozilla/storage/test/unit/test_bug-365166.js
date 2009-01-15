/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/licenses/publicdomain/  */ 

// Testcase for bug 365166 - crash [@ strlen] calling
// mozIStorageStatement::getColumnName of a statement created with
// "PRAGMA user_version" or "PRAGMA schema_version"
function run_test() {
  var file = Components.classes["@mozilla.org/file/directory_service;1"]
                       .getService(Components.interfaces.nsIProperties)
                       .get("TmpD", Components.interfaces.nsIFile);
  file.append("bug-365166.sqlite");
  if (file.exists())
    file.remove(false);

  test('user');
  test('schema');

  function test(param)
  {
    var colName = param + "_version";
    var sql = "PRAGMA " + colName;

    var storageService = Components.classes["@mozilla.org/storage/service;1"].
                         getService(Components.interfaces.mozIStorageService);
    var conn = storageService.openDatabase(file); 
    var statement = conn.createStatement(sql);
    try {
      // This shouldn't crash:
      do_check_eq(statement.getColumnName(0), colName);

      // OK, if the above statement didn't crash, check that initializing a
      // wrapper doesn't crash either:
      var wrapper = Components.classes["@mozilla.org/storage/statement-wrapper;1"]
                              .createInstance(Components.interfaces.mozIStorageStatementWrapper);
      wrapper.initialize(statement);
    } finally {
      statement.reset();
      statement.finalize();
    }
  }
}
