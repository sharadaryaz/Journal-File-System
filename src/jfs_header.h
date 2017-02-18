#ifndef _JFS_HEADER_H_
#define _JFS_HEADER_H_

typedef enum {
	PENDING = 10,
	COMMITTED,
	ABORTED,
	MARKED,
	UNMARKED
} record_state;

struct version 
{
	char data[15];
	int action_id;
	int data_id;
};

struct outcome
{
	int action_id;
	record_state state;
	record_state mark_state;
};

typedef struct version data_version;
typedef struct outcome outcome_record;

int new_outcome_record();
void new_version(int, int);
void mark_point_announce(int);
int begin_transaction();
int check_for_marked(int);
void display_menu(int array[4], int);
void write_value(int, int, int);
void commit(int);
void read_current_value(int, int);
int if_committed(int t_id);
void recover(int, int);
void commit_fail_stop(int);
void recover_fail_stop();

#endif //_JFS_HEADER_H_