#ifndef _BUSINESS_DB_H_
#define _BUSINESS_DB_H_

#ifdef  __cplusplus
extern "C" {
#endif

#define BUSINESS_DB            "BUSINESS_DB"
#define BUSINESS_DB_ENV        "BUSINESS_DB"
#define BUSINESS_USERNAME_ENV  "BUSINESS_USERNAME"
#define BUSINESS_USERPWD_ENV   "BUSINESS_USERPWD"
#define BUSINESS_DATABASE_NAME "BUSINESS_DATABASE_NAME"

int BusinessDataBaseOpen();
int BusinessDataBaseClose();

#ifdef  __cplusplus
}
#endif

#endif
