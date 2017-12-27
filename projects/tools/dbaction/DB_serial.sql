rem*  This File "DB_serial.sql"
rem*  Genenated by
rem*  Application dbaction V3.0 for Oracle
rem*  with the action file "DB_serial.act".
rem*  Create: Mon Oct 15 14:24:29 2012
 
CREATE TABLE sys_serial 
  (
	id	 NUMBER(10) NOT NULL,
	serial_no	 NUMBER(10) NOT NULL
  ); 
CREATE UNIQUE INDEX Sys_Serial_I0 ON Sys_Serial(id);
