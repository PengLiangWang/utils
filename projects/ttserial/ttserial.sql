rem : tblserial �����Զ�����

CREATE TABLE tblserial
(
  serialName    VARCHAR2(30) NOT NULL,
  serialNo      NUMBER(10) NOT NULL,
  maxSerialNo   NUMBER(10) NOT NULL
);
CREATE UNIQUE INDEX TBLSerial_I0 ON tblserial(serialName);
