#ifndef _EPOS_ISO8583_H_
#define _EPOS_ISO8583_H_

#define	C_DEF(num,name,size)    unsigned char name[(size)+1];


#define LENGTH_OF_DATA_48   322
#define LENGTH_OF_DATA_55   1
#define LENGTH_OF_DATA_57   128
#define LENGTH_OF_DATA_58   1
#define LENGTH_OF_DATA_59   500
#define	LENGTH_OF_DATA_60   100
#define	LENGTH_OF_DATA_61   255
#define	LENGTH_OF_DATA_62   100
#define	LENGTH_OF_DATA_63   163

typedef struct
{
  C_DEF(-5,tpdu,10)
  C_DEF(-4,message_explain,12)
  C_DEF(-3,message_type,4)
  C_DEF(-2,primary_bitmap, 8)
  C_DEF(2,primary_account_number, 19)
  C_DEF(3,processing_code, 6)
  C_DEF(4,amount_of_transaction, 12)
  C_DEF(5,amount_of_settlement, 12)
  C_DEF(6,amount_of_cardholder_billing, 12)
  C_DEF(7,transmission_date_and_time, 10)
  C_DEF(8,amount_of_cardholder_billing_fee, 7)
  C_DEF(9,conversion_rate_of_settlement, 8)
  C_DEF(10,conversion_rate_of_cardholder_billing, 8)
  C_DEF(11,system_trace_audit_number, 6)
  C_DEF(12,time_of_local_transaction, 6)
  C_DEF(13,date_of_local_transaction, 4)
  C_DEF(14,date_of_expiration, 4)
  C_DEF(15,date_of_settlement, 4)
  C_DEF(16,date_of_conversion, 4)
  C_DEF(17,date_of_capture, 4)
  C_DEF(18,merchant_type, 4)
  C_DEF(19,acquiring_institution_country_code, 3)
  C_DEF(20,primary_account_number_extended_country_code, 3)
  C_DEF(21,forwarding_institution_country_code, 200)
  C_DEF(22,point_of_service_entry_mode, 3)
  C_DEF(23,card_sequence_number, 3)
  C_DEF(24,network_international_id, 3)
  C_DEF(25,point_of_service_condition_code, 2)
  C_DEF(26,point_of_service_PIN_capture_code, 2)
  C_DEF(27,auth_id_response_length, 1)
  C_DEF(28,amount_of_transaction_fee, 8)
  C_DEF(29,amount_of_settlement_fee, 8)
  C_DEF(30,amount_of_transaction_processing_fee, 8)
  C_DEF(31,amount_of_settlement_processing_fee, 8)
  C_DEF(32,acquiring_institution_id_code, 11)
  C_DEF(33,forwarding_institution_id_code, 11)
  C_DEF(34,extended_primary_account_number, 19)
  C_DEF(35,track_2_data, 37)
  C_DEF(36,track_3_data, 104)
  C_DEF(37,retrieval_reference_number, 12)
  C_DEF(38,auth_id_response, 6)
  C_DEF(39,response_code, 2)
  C_DEF(40,service_restriction_code, 3)
  C_DEF(41,card_acceptor_terminal_id, 8)
  C_DEF(42,card_acceptor_id_code, 15)
  C_DEF(43,card_acceptor_name_location, 40)
  C_DEF(44,additional_response_data, 25)
  C_DEF(45,track_1_data, 76)
  C_DEF(48,private_additional_data,LENGTH_OF_DATA_48)
  C_DEF(49,currency_code_of_transaction, 3)
  C_DEF(50,currency_code_of_settlement, 3)
  C_DEF(51,currency_code_of_cardholder_billing, 3)
  C_DEF(52,PIN, 8)
  C_DEF(53,security_related_control_info, 16)
  C_DEF(54,balance_amounts, 20)
  C_DEF(55,pboc_ic_transaction_information,LENGTH_OF_DATA_55)
  C_DEF(57,field_57,LENGTH_OF_DATA_57)
  C_DEF(58,pboc_electronic_data,LENGTH_OF_DATA_58)
  C_DEF(59,field_59,LENGTH_OF_DATA_59)
  C_DEF(60,reserved_private_data_60,LENGTH_OF_DATA_60)
  C_DEF(61,reserved_private_data_61, LENGTH_OF_DATA_61)
  C_DEF(62,reserved_private_data_62, LENGTH_OF_DATA_62)
  C_DEF(63,reserved_private_data_63, LENGTH_OF_DATA_63)
  C_DEF(64,MAC64, 8)
}ISO8583;


#define POS_BIT_tpdu                                   -5
#define POS_BIT_message_explain                        -4
#define POS_BIT_message_type                           -3
#define POS_BIT_primary_bitmap                         -2
#define POS_BIT_primary_account_number                  2
#define POS_BIT_processing_code                         3
#define POS_BIT_amount_of_transaction                   4
#define POS_BIT_amount_of_settlement                    5
#define POS_BIT_amount_of_cardholder_billing            6
#define POS_BIT_transmission_date_and_time              7
#define POS_BIT_amount_of_cardholder_billing_fee        8
#define POS_BIT_conversion_rate_of_settlement           9
#define POS_BIT_conversion_rate_of_cardholder_billing  10
#define POS_BIT_system_trace_audit_number              11
#define POS_BIT_time_of_local_transaction              12
#define POS_BIT_date_of_local_transaction              13
#define POS_BIT_date_of_expiration                     14
#define POS_BIT_date_of_settlement                     15
#define POS_BIT_date_of_conversion                     16
#define POS_BIT_date_of_capture                        17
#define POS_BIT_merchant_type                          18
#define POS_BIT_acquiring_institution_country_code     19
#define POS_BIT_primary_account_number_extended_country_code  20
#define POS_BIT_forwarding_institution_country_code    21
#define POS_BIT_point_of_service_entry_mode            22
#define POS_BIT_card_sequence_number                   23
#define POS_BIT_network_international_id               24
#define POS_BIT_point_of_service_condition_code        25
#define POS_BIT_point_of_service_PIN_capture_code      26
#define POS_BIT_auth_id_response_length                27
#define POS_BIT_amount_of_transaction_fee              28
#define POS_BIT_amount_of_settlement_fee               29
#define POS_BIT_amount_of_transaction_processing_fee   30
#define POS_BIT_amount_of_settlement_processing_fee    31
#define POS_BIT_acquiring_institution_id_code          32
#define POS_BIT_forwarding_institution_id_code         33
#define POS_BIT_extended_primary_account_number        34
#define POS_BIT_track_2_data                           35
#define POS_BIT_track_3_data                           36
#define POS_BIT_retrieval_reference_number             37
#define POS_BIT_auth_id_response                       38
#define POS_BIT_response_code                          39
#define POS_BIT_service_restriction_code               40
#define POS_BIT_card_acceptor_terminal_id              41
#define POS_BIT_card_acceptor_id_code                  42
#define POS_BIT_card_acceptor_name_location            43
#define POS_BIT_additional_response_data               44
#define POS_BIT_track_1_data                           45
#define POS_BIT_private_additional_data                48
#define POS_BIT_currency_code_of_transaction           49
#define POS_BIT_currency_code_of_settlement            50
#define POS_BIT_currency_code_of_cardholder_billing    51
#define POS_BIT_PIN                                    52
#define POS_BIT_security_related_control_info          53
#define POS_BIT_balance_amounts                        54
#define POS_BIT_pboc_ic_transaction_information        55
#define POS_BIT_field_57                               57
#define POS_BIT_pboc_electronic_data                   58
#define POS_BIT_field_59                               59
#define POS_BIT_reserved_private_data_60               60
#define POS_BIT_reserved_private_data_61               61
#define POS_BIT_reserved_private_data_62               62
#define POS_BIT_reserved_private_data_63               63
#define POS_BIT_MAC64                                  64

int check_bitmap(ISO8583 *data, int bit);
int set_bitmap(ISO8583 *data, int bit);
int clear_bitmap(ISO8583 *data);
int ISO8583_set_field(int flag);
int ISO8583_hton(ISO8583 *data,unsigned char *buf,int *bufSize);
int ISO8583_ntoh(ISO8583 *data,unsigned char *buf);
int ISO8583_dump(char *title,ISO8583 *data);
int ISOBUFF_dump(char *title,unsigned char *buffer,int size);

#endif
