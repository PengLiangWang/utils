rem*  This File "DB_HsmKeyBox.sql"
rem*  Genenated by
rem*  Application dbaction V3.0 for Oracle
rem*  with the action file "DB_HsmKeyBox.act".
rem*  Create: Fri Aug 17 13:33:54 2012
 
CREATE TABLE HsmKeyBoxEx 
  (
	keyIndex	 NUMBER(10) NOT NULL,
	keyType	 VARCHAR2(1) NOT NULL,
	keyFlag	 VARCHAR2(1) NOT NULL,
	keyLen	 NUMBER(10) NOT NULL,
	keyDataA	 VARCHAR2(256),
	keyDataB	 VARCHAR2(256),
	keyDac	 VARCHAR2(32) NOT NULL
  ); 
CREATE UNIQUE INDEX HsmKeyBoxEx_I0 ON HsmKeyBoxEx(keyIndex);
