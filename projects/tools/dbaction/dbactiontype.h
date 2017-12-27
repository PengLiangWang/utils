#define K_end			0
#define K_action		1
#define K_read_by		2
#define K_read_lock_by		3
#define K_add			4
#define K_del_by		5
#define K_update_by		6
#define K_open_select		7
#define K_open_select_by	8
#define K_fetch_select		9
#define K_close_select		10
#define K_free_lock		11
#define K_double		12
#define K_long			13
#define K_str			14
#define K_YYYYMMDD		15
#define K_HHMISS		16
#define K_short 		17
#define K_int			18
#define K_index_by		19
#define K_index_by_unique	20
#define K_notnull		21
#define K_update_by_select      22	
#define K_update_by_lock        23	
#define K_open_select_for_update     24
#define K_open_select_for_update_by  25
#define K_by                         26
#define K_sum_for                    27
#define K_avg_for                    28
#define K_count			     29
#define K_max_for                    30 
#define K_min_for                    31 
#define K_avg_distinct_for           32 
#define K_count_distinct_for         33 
#define K_index_unique_by            34
#define K_read			     35		
#define K_update		     36		
#define K_print_report		     37		
#define K_order_by_		     38
#define K_read_lock		     39
#define K_count_by		     40
#define K_del_all			41
#define K_or_				42
#define K_open_select_distinct_for	43
#define	K_date				44

typedef struct FieldDef_t
	{
		struct FieldDef_t	*next;
		char			*name;
		int			type;
		int			size;
		int			scale;
                int                     notnull;
	} FieldDef;

typedef struct ArgDef_t
	{
		struct ArgDef_t 	*next;
		char			*name;
		char			operate[16];
		int			n;
	} ArgDef;

typedef struct
	{
		FieldDef	*head;
		FieldDef	*tail;
		int		cnt;
	} FieldList;

typedef struct
	{
		ArgDef		*head;
		ArgDef		*tail;
		int		cnt;
	} ArgList;
